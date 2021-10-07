
#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>

#define DERIVE_TRIVIAL_PRINTER(StrongType)                                     \
    static inline ::std::ostream& operator<<(std::ostream& os, StrongType i) { \
        os << std::dec << (StrongType::Representation)i;                       \
        return os;                                                             \
    }

#define DERIVE_TRIVIAL_HASHER(StrongType)                 \
    namespace std {                                       \
    template <>                                           \
    struct hash<StrongType> {                             \
        size_t operator()(StrongType i) const {           \
            auto j = (StrongType::Representation)i;       \
            return hash<StrongType::Representation>()(j); \
        }                                                 \
    };                                                    \
    }

#define DERIVE_STRONG_INT(StrongType, Repr, suffix)                                \
    struct StrongType##Diff {};                                                    \
    using StrongType = StrongInt<Repr, StrongType##Diff>;                          \
                                                                                   \
    DERIVE_TRIVIAL_PRINTER(StrongType)                                             \
    DERIVE_TRIVIAL_HASHER(StrongType)                                              \
                                                                                   \
    constexpr static inline StrongType operator""_##suffix(long long unsigned x) { \
        return StrongType(static_cast<Repr>(x));                                   \
    }

#define DERIVE_STRONGER_INT(StrongType, Repr, suffix)                              \
    struct StrongType##Diff {};                                                    \
    using StrongType = StrongerInt<Repr, StrongType##Diff>;                        \
                                                                                   \
    DERIVE_TRIVIAL_PRINTER(StrongType)                                             \
    DERIVE_TRIVIAL_HASHER(StrongType)                                              \
                                                                                   \
    constexpr static inline StrongType operator""_##suffix(long long unsigned x) { \
        return StrongType(static_cast<Repr>(x));                                   \
    }

// Diff differentiates types with the same repr
template <typename Repr, typename Diff>
class StrongInt {
public:
    using Representation = Repr;
    using Differentiator = Diff;

    constexpr StrongInt() = default;

    constexpr explicit StrongInt(Repr _data) : data(_data) {}
    constexpr explicit operator Repr() const { return data; }
    constexpr Repr get() const { return data; }

    constexpr bool operator==(StrongInt rhs) const { return this->data == rhs.data; }
    constexpr bool operator!=(StrongInt rhs) const { return this->data != rhs.data; }

    constexpr bool operator<(StrongInt rhs) const { return this->data < rhs.data; }
    constexpr bool operator>(StrongInt rhs) const { return this->data > rhs.data; }

    constexpr bool operator<=(StrongInt rhs) const { return this->data <= rhs.data; }
    constexpr bool operator>=(StrongInt rhs) const { return this->data >= rhs.data; }

    constexpr StrongInt operator+(StrongInt rhs) const {
        return StrongInt(this->data + rhs.data);
    }
    constexpr StrongInt operator-(StrongInt rhs) const {
        return StrongInt(this->data - rhs.data);
    }
    constexpr StrongInt operator*(StrongInt rhs) const {
        return StrongInt(this->data * rhs.data);
    }
    constexpr StrongInt operator/(StrongInt rhs) const {
        return StrongInt(this->data / rhs.data);
    }
    constexpr StrongInt operator%(StrongInt rhs) const {
        return StrongInt(this->data % rhs.data);
    }
    constexpr StrongInt operator>>(StrongInt rhs) const {
        return StrongInt(this->data >> rhs.data);
    }
    constexpr StrongInt operator<<(StrongInt rhs) const {
        return StrongInt(this->data << rhs.data);
    }

    constexpr StrongInt operator+=(StrongInt rhs) {
        this->data += rhs.data;
        return *this;
    }
    constexpr StrongInt operator-=(StrongInt rhs) {
        this->data -= rhs.data;
        return *this;
    }
    constexpr StrongInt operator*=(StrongInt rhs) {
        this->data *= rhs.data;
        return *this;
    }
    constexpr StrongInt operator/=(StrongInt rhs) {
        this->data /= rhs.data;
        return *this;
    }
    constexpr StrongInt operator%=(StrongInt rhs) {
        this->data %= rhs.data;
        return *this;
    }
    constexpr StrongInt operator>>=(StrongInt rhs) {
        this->data >>= rhs.data;
        return *this;
    }
    constexpr StrongInt operator<<=(StrongInt rhs) {
        this->data <<= rhs.data;
        return *this;
    }

    constexpr StrongInt operator++() {
        ++this->data;
        return *this;
    }
    constexpr StrongInt operator--() {
        --this->data;
        return *this;
    }

    constexpr StrongInt operator++(int) {
        auto tmp = *this;
        ++this->data;
        return tmp;
    }
    constexpr StrongInt operator--(int) {
        auto tmp = *this;
        --this->data;
        return tmp;
    }

protected:
    Repr data = 0;
};

template <typename Repr, typename Diff>
class StrongerInt : public StrongInt<Repr, Diff> {
public:
    static constexpr StrongerInt<Repr, Diff> INVALID{(Repr)-1};

    constexpr explicit StrongerInt() : StrongInt<Repr, Diff>(INVALID) {}
    constexpr explicit StrongerInt(Repr x) : StrongInt<Repr, Diff>(x) {}

    bool isValid() const { return this->data != INVALID.data; }

    constexpr StrongerInt operator+(StrongerInt rhs) const {
        return StrongerInt(this->data + rhs.data);
    }
    constexpr StrongerInt operator-(StrongerInt rhs) const {
        return StrongerInt(this->data - rhs.data);
    }
    constexpr StrongerInt operator*(StrongerInt rhs) const {
        return StrongerInt(this->data * rhs.data);
    }
    constexpr StrongerInt operator/(StrongerInt rhs) const {
        return StrongerInt(this->data / rhs.data);
    }
    constexpr StrongerInt operator%(StrongerInt rhs) const {
        return StrongerInt(this->data % rhs.data);
    }
    constexpr StrongerInt operator>>(StrongerInt rhs) const {
        return StrongerInt(this->data >> rhs.data);
    }
    constexpr StrongerInt operator<<(StrongerInt rhs) const {
        return StrongerInt(this->data << rhs.data);
    }

    constexpr StrongerInt operator+=(StrongerInt rhs) {
        this->data += rhs.data;
        return *this;
    }
    constexpr StrongerInt operator-=(StrongerInt rhs) {
        this->data -= rhs.data;
        return *this;
    }
    constexpr StrongerInt operator*=(StrongerInt rhs) {
        this->data *= rhs.data;
        return *this;
    }
    constexpr StrongerInt operator/=(StrongerInt rhs) {
        this->data /= rhs.data;
        return *this;
    }
    constexpr StrongerInt operator%=(StrongerInt rhs) {
        this->data %= rhs.data;
        return *this;
    }
    constexpr StrongerInt operator>>=(StrongerInt rhs) {
        this->data >>= rhs.data;
        return *this;
    }
    constexpr StrongerInt operator<<=(StrongerInt rhs) {
        this->data <<= rhs.data;
        return *this;
    }

    constexpr StrongerInt operator++() {
        ++this->data;
        return *this;
    }
    constexpr StrongerInt operator--() {
        --this->data;
        return *this;
    }

    constexpr StrongerInt operator++(int) {
        auto tmp = *this;
        ++this->data;
        return tmp;
    }
    constexpr StrongerInt operator--(int) {
        auto tmp = *this;
        --this->data;
        return tmp;
    }
};
