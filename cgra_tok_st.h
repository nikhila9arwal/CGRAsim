
#pragma once

#include "cgra_defs.h"

namespace platy {
namespace sim {
namespace cgra {

class TokenStore {
public:
    struct Tag {
        Tag(InstrMemIdx _instIdx, CbIdx _cbid);
        ~Tag() {}
        InstrMemIdx instIdx; // TODO (nzb): Why Idx vs id ?
        CbIdx cbid; // TODO (nzb): Make TaskIdx instead
        bool operator==(const struct Tag& other) const;
        struct HashFn
        {
            std::size_t operator() (const Tag &node) const;
        };

    };

    //return here;


 
    struct Token {
        Token(PosIdx _posid, Word _value, InstrMemIdx _inst, CbIdx _cbid);
        ~Token () {}
        PosIdx posid;
        Word value;
        Tag tag;
    };
    
    struct Entry {
        Entry(Token tok);
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

    TokenStore(uint32_t _capacity);
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

    bool isEmpty();
    bool isFull();

private:
    std::unordered_map<Tag, EntryPtr, Tag::HashFn> tokenStore;
    uint32_t capacity;
};

}
}
}
