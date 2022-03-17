
#pragma once

#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

class TokenStore {
public:
    struct Tag {
        Tag(InstrMemIdx _instIdx, CbIdx _cbid) : instIdx(_instIdx), cbid(_cbid) {}
        ~Tag() {}
        InstrMemIdx instIdx; // TODO (nzb): Why Idx vs id ?
        CbIdx cbid; // TODO (nzb): Make TaskIdx instead
        bool operator==(const struct Tag& other) const {  // <  ==> > for min heap
            return instIdx == other.instIdx && cbid == other.cbid;
        }
        bool operator<(const struct Tag& other) const {  // <  ==> > for min heap
            return int32_t(instIdx) + int32_t(cbid) < int32_t(other.instIdx) + int32_t(other.cbid);
        }
    };
    
    struct Token {
        Token(PosIdx _posid, Word _value, InstrMemIdx _inst, CbIdx _cbid)
            : posid(_posid), value(_value), tag(_inst, _cbid) {}
        ~Token () {}
        PosIdx posid;
        Word value;
        Tag tag;
    };
    
    struct Entry {
        Entry(Token tok)
            : lhsValid(false), rhsValid(false), predicateValid(false), tag(tok.tag) {
            setToken(tok);
        }
        ~Entry() {}
        void setToken(Token tok);
        // TODO (nikhil): change to generic val, pos type structure.
        Word lhs;
        bool lhsValid;
        Word rhs;
        bool rhsValid;
        Word predicate;
        bool predicateValid;
        Tag tag;
    };
    typedef std::shared_ptr<Entry> EntryPtr;

    TokenStore(uint32_t _capacity) : capacity(_capacity) { }
    ~TokenStore() {}

    /**
     * @brief Push a new token into the TokenStore. Can fail if there
     * is not enough space.
     *
     * @param tok Incoming token.
     *
     * @return A pointer to the token store entry if the token is
     * accepted. Null if the token cannot be accepted.
     */
    EntryPtr acceptToken(Token tok);

    /**
     * @brief Returns the entry, if present, or null otherwise.
     *
     * @param tag The tag to look up.
     *
     * @return A pointer to the token store entry if found, null
     * otherwise.
     */
    EntryPtr at(Tag tag);

    /**
     * @brief Delete a token from the store. Asserts that the
     * requested tag is valid.
     *
     * @param tag The tag to delete.
     */
    void erase(Tag tag);

    inline bool isEmpty() { return tokenStore.empty(); }
    inline bool isFull() { return tokenStore.size() == capacity; }

private:
    std::map<Tag, EntryPtr> tokenStore;
    uint32_t capacity;
};

}
}
}
