
#pragma once

#include <unistd.h>

#include <vector>

// This is a vector that's indexed by a strong type
// Its still a work in progress, so if there's some method from std::vector
//  you need that's not here, feel free to implement it yourself!
//
// NOT GLOBALLOC: if you need for pinfo, subclass with GlobAlloc and an glob array
// allocator

template <typename K, typename V, typename Alloc = std::allocator<V>>
class StrongVec {
public:
    using Representation = std::vector<V, Alloc>;
    using key_type = K;
    using value_type = V;
    using allocator_type = Alloc;
    using iterator = typename Representation::iterator;
    using const_iterator = typename Representation::const_iterator;

    StrongVec(const allocator_type& alloc = allocator_type()) : vec(alloc) {}

    StrongVec(
        key_type n,
        const value_type& val = value_type(),
        const allocator_type& alloc = allocator_type())
        : vec((typename key_type::Representation)n, val, alloc) {}

    template <typename InputIt>
    StrongVec(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
        : vec(first, last, alloc) {}

    StrongVec(std::initializer_list<V> init) : vec(init, allocator_type()) {}

    void reserve(key_type n) { vec.reserve(toSize_t(n)); }

    key_type size() const { return toKeyType(vec.size()); }

    bool empty() const { return vec.size() == 0; }

    value_type& operator[](key_type i) { return vec.at(toSize_t(i)); }

    value_type& at(key_type i) { return vec.at(toSize_t(i)); }

    const value_type& operator[](key_type i) const { return vec.at(toSize_t(i)); }

    const value_type& at(key_type i) const { return vec.at(toSize_t(i)); }

    void resize(key_type n) { vec.resize(toSize_t(n)); }

    void resize(key_type n, value_type val) { vec.resize(toSize_t(n), val); }

    void push_back(const value_type& val) { vec.push_back(val); }

    void emplace_back() { vec.emplace_back(); }

    template <typename... T>
    void emplace_back(T&&... t) {
        vec.emplace_back(std::forward(t)...);
    }

    iterator insert(const_iterator pos, const value_type& val) {
        return vec.insert(pos, val);
    }

    iterator insert(const_iterator pos, value_type&& val) { return vec.insert(pos, val); }

    iterator insert(const_iterator pos, key_type n, const value_type& val) {
        return vec.insert(pos, toSize_t(n), val);
    }

    template <typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return vec.insert(pos, first, last);
    }

    iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
        vec.insert(pos, ilist);
    }

    value_type& front() {
        assert(!empty());
        return vec.front();
    }
    const value_type& front() const {
        assert(!empty());
        return vec.front();
    }

    value_type& back() {
        assert(!empty());
        return vec.back();
    }
    const value_type& back() const {
        assert(!empty());
        return vec.back();
    }

    iterator begin() { return vec.begin(); }
    iterator end() { return vec.end(); }

    const_iterator begin() const { return vec.begin(); }
    const_iterator end() const { return vec.end(); }

    const_iterator cbegin() const { return vec.cbegin(); }
    const_iterator cend() const { return vec.cend(); }

    const Representation& getRepr() const { return vec; }

protected:
    // Composition rather than inheritance because we don't expose the interfaces,
    //  which take standards ints, defeating the purpose
    Representation vec;

    // TODO: when should this be range checked?
    // Assuming this gets inlined
    static constexpr size_t toSize_t(key_type i) {
        return (size_t)(typename key_type::Representation)i;
    }

    static constexpr key_type toKeyType(size_t i) {
        return (key_type)(typename key_type::Representation)i;
    }
};
