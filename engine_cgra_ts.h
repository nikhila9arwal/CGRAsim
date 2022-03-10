
#pragma once

#include "engine_cgra_defs.h"

namespace platy {
namespace sim {
namespace ms {


class TokenStore {
public:
    struct Tag {
        Tag(InstrMemIdx _instIdx, CbIdx _cbid) : instIdx(_instIdx), cbid(_cbid) {}
        ~Tag() {}
        InstrMemIdx instIdx;
        CbIdx cbid;
        bool operator==(const struct Tag& other) const {  // <  ==> > for min heap
            return instIdx == other.instIdx && cbid == other.cbid;
        }
        //TODO(nikhil): Ask beckmann what to do about this
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
    struct TokenStoreEntry {
        TokenStoreEntry(Token tok)
            : lhsValid(false), rhsValid(false), predicateValid(false), tag(tok.tag) {
            setToken(tok);
        }
        ~TokenStoreEntry() {}
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

    TokenStore(uint32_t _size) : size(_size) { }
    ~TokenStore() {}

    // TODO (nikhil) : change to acceptToken
    shared_ptr<TokenStoreEntry> setToken(Token tok);

    // TODO (nikhil) : isNotFull ==> isEmpty
    inline bool isNotFull() { return tokenStore.size() <= size; }

    shared_ptr<TokenStoreEntry> getTokenStoreEntry(Tag tag);

private:
    std::map<Tag, TokenStoreEntry> tokenStore;
    uint32_t size;
};

}
}
}