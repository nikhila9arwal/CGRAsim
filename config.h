
#pragma once

/* Thin wrapper around libconfig to:
 * - Reduce and simplify init code (tailored interface, not type BS, ...)
 * - Strict config: type errors, warnings on unused variables, panic on different defaults
 * - Produce a full configuration file with all the variables, including defaults (for
 * config parsing, comparison, etc.)
 */

#include <stdint.h>

#include <string>
#include <vector>

namespace libconfig {
class Config;
class Setting;
};  // namespace libconfig

class Config {
private:
    libconfig::Config* inCfg;
    libconfig::Config* outCfg;

public:
    explicit Config(const char* inFile);
    ~Config();

    // Called when initialization ends. Writes output config, and emits warnings for
    // unused input settings
    void writeAndClose(const char* outFile, bool strictCheck);

    bool exists(const char* key);
    bool exists(const std::string& key) { return exists(key.c_str()); }

    // Access interface
    // T can be uint32_t, uint64_t, bool, or const char*. Instantiations are in the cpp
    // file

    // Mandatory values (no default, panics if setting does not exist)
    template <typename T>
    T get(const char* key) const;
    template <typename T>
    T get(const std::string& key) const {
        return get<T>(key.c_str());
    }

    // Optional values (default)
    template <typename T>
    T get(const char* key, T def) const;
    template <typename T>
    T get(const std::string& key, T def) const {
        return get<T>(key.c_str(), def);
    }

    // Enable iteration over a group
    void keys(const char* key, std::vector<const char*>& outKeys);
    void keys(const std::string& key, std::vector<const char*>& outKeys) {
        keys(key.c_str(), outKeys);
    }

    // Get subgroups in a specific key
    void subgroups(const char* key, std::vector<const char*>& grps);
    void subgroups(const std::string& key, std::vector<const char*>& grps) {
        subgroups(key.c_str(), grps);
    }

private:
    template <typename T>
    T genericGet(const char* key) const;
    template <typename T>
    T genericGet(const char* key, T def) const;
};

/* Parsing functions used for configuration */

std::vector<bool> ParseMask(const std::string& maskStr, uint32_t maskSize);

/* Parses a delimiter-separated list of T's (typically ints, see/add specializtions in
 * .cpp) 0-elem lists are OK panics on parsing and size-violation errors
 */
template <typename T>
std::vector<T> ParseList(const std::string& listStr, const char* delimiters);

template <typename T>
std::vector<T> ParseList(const std::string& listStr) {
    return ParseList<T>(listStr, " ");
}

// fills remaining elems till maxSize with fillValue
template <typename T>
std::vector<T> ParseList(
    const std::string& listStr, uint32_t maxSize, uint32_t fillValue) {
    std::vector<T> res = ParseList<T>(listStr);
    if (res.size() > maxSize)
        panic("ParseList: Too many elements, max %d, got %ld", maxSize, res.size());
    while (res.size() < maxSize) res.push_back(fillValue);
    return res;
}

void Tokenize(
    const std::string& str,
    std::vector<std::string>& tokens,
    const std::string& delimiters);
