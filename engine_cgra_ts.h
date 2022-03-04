
#pragma once

#include "engine_cgra_defs.h"

namespace platy {
namespace sim {
namespace ms {


class TokenStore {
public:
    struct Tag {
        Tag(InstrMemIdx _instIdx, CbIdx _cbid) : instIdx(_instIdx), cbid(_cbid) {}
        InstrMemIdx instIdx;
        CbIdx cbid;
        bool operator==(const struct Tag& other) const {  // <  ==> > for min heap
            return instIdx == other.instIdx && cbid == other.cbid;
        }
    };
    struct Token {
        Token(PosIdx _posid, Word _value, InstrMemIdx _inst, CbIdx _cbid)
            : posid(_posid), value(_value), tag(_inst, _cbid) {}
        PosIdx posid;
        Word value;
        Tag tag;
    };
    struct TokenStoreEntry {
        TokenStoreEntry(Token tok)
            : tag(tok.tag), lhsValid(false), rhsValid(false), predicateValid(false) {
            setToken(tok);
        }
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

    TokenStoreEntry* setToken(Token tok);

    inline bool isNotFull() { return tokenStore.size() <= size; }

    TokenStoreEntry* getTokenStoreEntry(Tag tag);

private:
    std::unordered_map<Tag, TokenStoreEntry> tokenStore;
    uint32_t size;
};

}
}
}