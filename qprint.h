
#pragma once

#include <assert.h>

#include <cstring>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

namespace qp {

const bool SHOWBASE = true;

const char PH_OPEN = '{';
const char PH_CLOSE = '}';

//////////////////////////////////////////////////////////////////////////////////
// Helper Classes
//////////////////////////////////////////////////////////////////////////////////

class StringView {
public:
    StringView() = default;

    StringView(const char* str) : start(str), end(start + strlen(str)) {}
    StringView(const std::string& str) : start(str.c_str()), end(start + str.size()) {}

    StringView(const char* str, size_t off, size_t len = npos)
        : start(str + off), end(start + len) {
        if (len == npos) {
            end = str + strlen(str);
        } else {
            assert(len <= strlen(str) - off);
        }
    }

    StringView(const std::string& str, size_t off, size_t len = npos)
        : start(str.c_str() + off), end(start + len) {
        if (len == npos) {
            end = str.c_str() + str.size();
        } else {
            assert(len <= str.size() - off);
        }
    }

    StringView(const StringView&) = default;

    StringView(const StringView sv, size_t off, size_t len = npos) {
        *this = sv.substr(off, len);
    }

    StringView substr(size_t off, size_t len = npos) const {
        assert(start + off + len <= end);
        return StringView(start, off, len);
    }

    size_t size() const { return (size_t)(end - start); }
    bool empty() const { return size() == 0; }

    bool operator==(const StringView& rhs) const { return this->compare(rhs) == 0; }

    bool operator!=(const StringView& rhs) const { return this->compare(rhs) != 0; }

    int compare(const StringView& rhs) const {
        for (size_t i = 0; i <= size(); ++i) {
            // Got to the end of both -> equal
            if (i == this->size() && i == rhs.size()) {
                return 0;

                // lhs finished first -> lhs is greater
            } else if (i == this->size()) {
                return 1;

                // lhs finished first -> rhs is greater
            } else if (i == rhs.size()) {
                return -1;
            }

            auto diff = this->at(i) - rhs[i];
            if (diff != 0) {
                return diff;
            }
        }

        assert(false && "Unreachable");
    }

    // I should write something more general, but this is enough for now...
    size_t find(char ch, size_t off = 0) const {
        auto* p = start + off;
        while (p != end) {
            if (*p == ch) {
                return (size_t)(p - start);
            }
            ++p;
        }
        return npos;
    }

    char operator[](size_t idx) const {
        assert(start + idx < end);
        return *(start + idx);
    }

    char at(size_t idx) const { return (*this)[idx]; }

    std::string str() const { return std::string(start, end); }

    friend bool operator<(const StringView& lhs, const StringView& rhs) {
        return lhs.compare(rhs) < 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const StringView& sv) {
        auto* p = sv.start;
        while (p != sv.end) {
            os << *p;
            ++p;
        }

        return os;
    }

    static const size_t npos = (size_t)-1;

private:
    const char* start = nullptr;
    const char* end = nullptr;
};

// based on
// https://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/
class ConstStr {
public:
    template <unsigned long long N>
    constexpr ConstStr(const char (&arr)[N]) : start(arr), thesize(N - 1) {
        static_assert(N >= 1, "not a string literal");
    }

    constexpr char operator[](size_t i) const {
        return requires_inrange(i, thesize), start[i];
    }

    constexpr char at(size_t i) const { return (*this)[i]; }

    constexpr unsigned size() const { return thesize; }

    constexpr size_t count_placeholders(
        size_t i = 0, size_t num = 0, bool in = false) const {
        return i == thesize ? num : this->at(i) == PH_OPEN ? requires_true(!in),
               count_placeholders(i + 1, num + 1, true)
               : this->at(i) == PH_CLOSE
               ? requires_true(in),
               count_placeholders(i + 1, num, false) : count_placeholders(i + 1, num, in);
    }

    operator std::string() const { return std::string(start); }

private:
    const char* start = nullptr;
    size_t thesize = -1;

    static constexpr size_t requires_inrange(size_t i, size_t len) {
        return i >= len ? throw std::out_of_range("") : i;
    }

    static constexpr bool requires_true(bool b) { return b ? b : throw std::exception(); }
};

////////////////////////////////////////////////////////////////////////////////
// Template Metaprogramming tools
////////////////////////////////////////////////////////////////////////////////

// detect an iterable type
// https://stackoverflow.com/a/29634934
using std::begin;
using std::end;

template <typename T>
auto constexpr is_iterable_impl(int) -> decltype(
    begin(std::declval<T&>()) != end(std::declval<T&>()),    // begin/end and operator !=
    void(),                                                  // Handle evil operator ,
    ++std::declval<decltype(begin(std::declval<T&>()))&>(),  // operator ++
    void(*begin(std::declval<T&>())),                        // operator*
    std::true_type{});

template <typename T>
std::false_type constexpr is_iterable_impl(...);

template <typename T>
using is_iterable = decltype(is_iterable_impl<T>(0));

// detect a printable type
template <typename T>
auto constexpr is_printable_impl(int)
    -> decltype(std::declval<std::ostream&>() << std::declval<T>(), std::true_type{});

template <typename T>
std::false_type constexpr is_printable_impl(...);

template <typename T>
using is_printable = decltype(is_printable_impl<T>(0));

// regular case
template <typename T>
typename std::enable_if<!is_iterable<T>::value && is_printable<T>::value, void>::type
put_to(std::ostream& os, T&& v) {
    os << v;
}

template <typename T>
typename std::enable_if<is_iterable<T>::value && is_printable<T>::value, void>::type
put_to(std::ostream& os, T&& v) {
    os << v;
}

template <typename T1, typename T2>
void put_to(std::ostream& os, const std::pair<T1, T2>& v);

template <typename T>
void put_to(std::ostream& os, const std::optional<T>& v);

// iterable case
template <typename T>
typename std::enable_if<is_iterable<T>::value && !is_printable<T>::value, void>::type
put_to(std::ostream& os, T&& v) {
    os << "{";

    auto it = std::begin(v);
    if (it != std::end(v)) {
        put_to(os, *it);
        ++it;
    }
    while (it != std::end(v)) {
        os << ", ";
        put_to(os, *it);
        ++it;
    }

    os << "}";
}

// pair case
// TODO: only if not printable - want to allow user overrides
template <typename T1, typename T2>
void put_to(std::ostream& os, const std::pair<T1, T2>& v) {
    os << "(";
    put_to(os, v.first);
    os << ", ";
    put_to(os, v.second);
    os << ")";
}

// optional case
// TODO: only if not printable - want to allow user overrides
template <typename T>
void put_to(std::ostream& os, const std::optional<T>& v) {
    if (v.has_value()) {
        put_to(os, *v);
    } else {
        os << "None";
    }
}

// detect a printable type
template <typename T>
auto constexpr is_qprintable_impl(int) -> decltype(
    put_to(std::declval<std::ostream&>(), std::declval<T>()), std::true_type{});

template <typename T>
std::false_type constexpr is_qprintable_impl(...);

template <typename T>
using is_qprintable = decltype(is_qprintable_impl<T>(0));

////////////////////////////////////////////////////////////////////////////////
// qformat
// prints format string (with substituted values) to ss (first argument)
//////////////////////////////////////////////////////////////////////////////////

// Base case: just print string s
static inline void qformat_rec(std::stringstream& ss, const StringView s) {
    ss << s;
    assert(s.find(PH_OPEN) == std::string::npos && "More placeholders than arguments");
}

static inline bool apply_format(
    std::stringstream& ss, const StringView& sv, std::ios& saved) {
    if (sv.empty()) {
        return false;
    }
    saved.copyfmt(ss);

    auto first = sv[0];

    if (first == 'x') {
        if (SHOWBASE) {
            ss << std::showbase;
        }
        ss << std::hex;

    } else if (first == 'd') {
        ss << std::dec;

    } else if (first == 'o') {
        if (SHOWBASE) {
            ss << std::showbase;
        }
        ss << std::oct;

    } else if (first == 'f') {
        ss << std::fixed;

    } else if (first == 'e') {
        ss << std::scientific;

    } else if (first == 'b') {
        ss << std::boolalpha;

    } else if (first == ',') {
        static auto locale = std::locale("");
        ss.imbue(locale);

    } else {
        std::cerr << "Unrecognized format string (first character inavlid format) \""
                  << sv << "\"\n";
        assert(false);
    }

    if (sv.size() > 1) {
        if (sv.size() > 2) {
            std::cerr << "Unrecognized format string (too long) \"" << sv << "\"\n";
        }
        if (sv[1] == ',') {
            if (sv[0] == ',') {
                std::cerr << "Unrecognized format string (two commas) \"" << sv << "\"\n";
            }

            static auto locale = std::locale("");
            ss.imbue(locale);
        }
    }

    return true;
}

// Recusive case: print first value and recurse
template <typename T, typename... Ts>
static inline void qformat_rec(
    std::stringstream& ss, const StringView s, T&& v, Ts&&... vs) {
    auto open = s.find(PH_OPEN);
    assert(open != StringView::npos && "More arguments than placeholders");

    auto close = s.find(PH_CLOSE, open + 1);
    assert(close != std::string::npos);

    auto format = s.substr(open + 1, close - (open + 1));

    ss << StringView(s, 0, open);

    std::ios old_state(nullptr);
    bool did = apply_format(ss, format, old_state);

    put_to(ss, std::forward<T>(v));

    if (did) {
        ss.copyfmt(old_state);
    }

    qformat_rec(ss, s.substr(close + 1), std::forward<Ts>(vs)...);
}

template <typename... Ts>
static inline std::string qformat_impl(const std::string&& s, Ts&&... vs) {
    std::stringstream ss;
    qformat_rec(ss, StringView(s), std::forward<Ts>(vs)...);
    return ss.str();
}

template <typename... Ts>
static inline void qprint_os_impl(std::ostream& os, const std::string&& s, Ts&&... vs) {
    std::stringstream ss;
    qformat_rec(ss, StringView(s), std::forward<Ts>(vs)...);
    os << ss.str();
}

template <typename... Ts>
static inline void qprint_impl(const std::string& s, Ts&&... vs) {
    qprint_os_impl(
        std::cout, std::forward<const std::string>(s), std::forward<Ts>(vs)...);
}

template <typename... Ts>
static inline void qerr_impl(const std::string& s, Ts&&... vs) {
    qprint_os_impl(
        std::cerr, std::forward<const std::string>(s), std::forward<Ts>(vs)...);
}

}  // namespace qp

////////////////////////////////////////////////////////////////////////////////
// qformat and specialization
// uses qformat to print to an ostream, stdout, or stderr
//////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__) || defined(__clang__)

// https://stackoverflow.com/a/33349105
#define qp_check_args(s, ...)                                                  \
    static_assert(                                                             \
        qp::ConstStr(s).count_placeholders()                                   \
            == std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value, \
        "Number of values does not match number of placeholders")

#define qformat(s, ...)                     \
    ({                                      \
        qp_check_args(s, ##__VA_ARGS__);    \
        qp::qformat_impl(s, ##__VA_ARGS__); \
    })

#define qprint_os(os, s, ...)                     \
    do {                                          \
        qp_check_args(s, ##__VA_ARGS__);          \
        qp::qprint_os_impl(os, s, ##__VA_ARGS__); \
    } while (0)

#define qprint(s, ...)                     \
    do {                                   \
        qp_check_args(s, ##__VA_ARGS__);   \
        qp::qprint_impl(s, ##__VA_ARGS__); \
    } while (0)

#define qerr(s, ...)                     \
    do {                                 \
        qp_check_args(s, ##__VA_ARGS__); \
        qp::qerr_impl(s, ##__VA_ARGS__); \
    } while (0)

#else  // defined(__GNUC_) || defined(__clang__)

#define qformat(...) qp::qformat_impl(__VA_ARGS__)

#define qprint_os(...) qp::qprint_os_impl(__VA_ARGS__)

#define qprint(...) qp::qprint_impl(__VA_ARGS__)

#define qerr(...) qp::qerr_impl(__VA_ARGS__)

#endif  // defined(__GNUC_) || defined(__clang__)

// Pretty printing of <class>::<method>(). Assumes there are at least two namespaces
// (e.g., class + one more), which is a fair assumption in platy. Based on
// https://stackoverflow.com/questions/1666802/is-there-a-class-macro-in-c
inline std::string methodName(const std::string& prettyFunction) {
    size_t colons = prettyFunction.find("::");

    // Cut off the return type.
    size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1;

    // Cut off the parameters.
    size_t end = prettyFunction.rfind("(");

    // Cut off all namespaces except for the class name.
    size_t lastColon = prettyFunction.substr(begin, end - begin).rfind("::");
    size_t secondLastColon = prettyFunction.substr(begin, lastColon).rfind("::");

    // Start right after the second to last colon.
    begin += secondLastColon + 2;

    return prettyFunction.substr(begin, end - begin) + "()";
}

#define __CLASS_METHOD__ methodName(__PRETTY_FUNCTION__)
