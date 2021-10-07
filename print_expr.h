
#pragma once

#include <type_traits>

#include "qprint.h"

/* Helper class to print expression with values
 * Inpired by Phil Nash's CATCH, https://github.com/philsquared/Catch
 * const enough that asserts that use this are still optimized through
 * loop-invariant code motion
 */
class PrintExpr {
private:
    std::stringstream& ss;

    // TODO: consider bailing if not printable
    template <typename T>
    void print(T t) const {
        if constexpr (qp::is_qprintable<T>::value) {
            // ss << t;
            qp::put_to(ss, t);
        } else {
            (void)t;
            ss << "?";
        }
    }

public:
    PrintExpr(std::stringstream& _ss) : ss(_ss) {}

    // Start capturing values
    template <typename T>
    const PrintExpr operator->*(T t) const {
        print(t);
        return *this;
    }

    // Overloads for all lower-precedence operators
    template <typename T>
    const PrintExpr operator==(T t) const {
        ss << " == ";
        print(t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator!=(T t) const {
        ss << " != ";
        print(t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator<=(T t) const {
        ss << " <= ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator>=(T t) const {
        ss << " >= ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator<(T t) const {
        ss << " < ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator>(T t) const {
        ss << " > ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator&(T t) const {
        ss << " & ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator|(T t) const {
        ss << " | ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator^(T t) const {
        ss << " ^ ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator&&(T t) const {
        ss << " && ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator||(T t) const {
        ss << " || ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator+(T t) const {
        ss << " + ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator-(T t) const {
        ss << " - ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator*(T t) const {
        ss << " * ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator/(T t) const {
        ss << " / ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator%(T t) const {
        ss << " % ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator<<(T t) const {
        ss << " << ";
        qp::put_to(ss, t);
        return *this;
    }
    template <typename T>
    const PrintExpr operator>>(T t) const {
        ss << " >> ";
        qp::put_to(ss, t);
        return *this;
    }

    // std::nullptr_t overloads (for nullptr's in assertions)
    // Only a few are needed, since most ops w/ nullptr are invalid
    const PrintExpr operator->*(std::nullptr_t t __attribute__((unused))) const {
        ss << "nullptr";
        return *this;
    }
    const PrintExpr operator==(std::nullptr_t t __attribute__((unused))) const {
        ss << " == nullptr";
        return *this;
    }
    const PrintExpr operator!=(std::nullptr_t t __attribute__((unused))) const {
        ss << " != nullptr";
        return *this;
    }

private:
    template <typename T>
    const PrintExpr operator=(T t) const;  // will fail, can't assign in assertion
};
