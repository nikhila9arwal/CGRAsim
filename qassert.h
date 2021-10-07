
#pragma once

#include <cassert>

#include "print_expr.h"
#include "qprint.h"

#define PANIC_EXIT_CODE (112)

// The macros interfered with boost
inline bool likely(bool x) { return __builtin_expect((x), 1); }
inline bool unlikely(bool x) { return __builtin_expect((x), 0); }

inline bool IMPLIES(bool x, bool y) { return !x || y; }

#ifndef NASSERT

// Externed variables are declared in each separate process.
const int QLOG_SIZE = 1024;
extern std::string qlogbuffer[QLOG_SIZE];
extern int qlogindex;

#define qlog(args...)                            \
    do {                                         \
        qlogbuffer[qlogindex] = qformat(args);   \
        qlogindex = (qlogindex + 1) % QLOG_SIZE; \
    } while (0)

#define qlogdump()                                          \
    do {                                                    \
        std::cerr << "--- Start log dump ---" << std::endl; \
        for (int i = 0; i < QLOG_SIZE; i++) {               \
            int idx = (qlogindex + i) % QLOG_SIZE;          \
            if (!qlogbuffer[idx].empty()) {                 \
                std::cerr << qlogbuffer[idx] << std::endl;  \
            }                                               \
        }                                                   \
        std::cerr << "--- End log dump ---" << std::endl;   \
    } while (0)

#define qassert(expr)                                                                  \
    if (unlikely(!(expr))) {                                                           \
        std::stringstream __assert_ss__LINE__;                                         \
        (PrintExpr(__assert_ss__LINE__)->*expr);                                       \
        qlogdump();                                                                    \
        qerr(                                                                          \
            "Failed assertion on {}:{} '{}' (with '{}')\n", __FILE__, __LINE__, #expr, \
            __assert_ss__LINE__.str());                                                \
        std::cerr.flush();                                                             \
        *reinterpret_cast<int*>(0L) = 42; /*SIGSEGVs*/                                 \
        std::abort();                     /*silence "not reached" warning*/            \
    };

#define assert_msg(cond, args...)                                               \
    do {                                                                        \
        if (!(cond)) {                                                          \
            qlogdump();                                                         \
            qp::qerr_impl("ASSERTION FAILED: {}\n", qformat(args));             \
            qerr("Failed assertion on {}:{}\n", __FILE__, __LINE__);            \
            std::cerr.flush();                                                  \
            *reinterpret_cast<int*>(0L) = 42; /*SIGSEGVs*/                      \
            std::abort();                     /*silence "not reached" warning*/ \
        }                                                                       \
    } while (0)

std::string decorateQassert();

#define info(args...)                                                      \
    do {                                                                   \
        qp::qprint_impl("{}INFO: {}\n", decorateQassert(), qformat(args)); \
    } while (0)

#define warn(args...)                                                   \
    do {                                                                \
        qp::qerr_impl("{}WARN {}\n", decorateQassert(), qformat(args)); \
    } while (0)

#define panic(args...)                                                       \
    do {                                                                     \
        qp::qerr_impl(                                                       \
            "{}PANIC {}:{}:{}: {}\n", decorateQassert(), __FILE__, __LINE__, \
            __FUNCTION__, qformat(args));                                    \
        exit(PANIC_EXIT_CODE);                                               \
    } while (0)

#define checkpoint()                                          \
    do {                                                      \
        qerr("{}:{} {}\n", __FILE__, __LINE__, __FUNCTION__); \
    } while (0)

// Prefer works like assert, but produces a warning rather than an abort if violated
#define prefer(expr)                                                                \
    if (unlikely(!(expr))) {                                                        \
        std::stringstream __assert_ss__LINE__;                                      \
        (PrintExpr(__assert_ss__LINE__)->*expr);                                    \
        qerr(                                                                       \
            "Prefer failed on {}:{} '{}' (with '{}')\n", __FILE__, __LINE__, #expr, \
            __assert_ss__LINE__.str());                                             \
    };

#define prefer_msg(cond, args...)                                \
    do {                                                         \
        if (!(cond)) {                                           \
            qp::qerr_impl("PREFER failed: {}\n", qformat(args)); \
        }                                                        \
    } while (0)
#else

#define qassert(...)
#define assert_msg(cond, args...)

#define info(args...)
#define warn(args...)
#define panic(args...)
#define checkpoint()
#define prefer(args...)
#define prefer_msg(args...)

#endif

/* we should implement something like this eventually... */
#define trace(args...)
