
#include "engine_cgra_ts.h"

namespace platy {
namespace sim {
namespace ms {

void TokenStore::TokenStoreEntry::setToken(Token tok) {
    switch (tok.posid) {
        case PosIdx::LHS:
            lhs = tok.value;
            lhsValid = true;
            break;
        case PosIdx::RHS:
            rhs = tok.value;
            rhsValid = true;
            break;
        case PosIdx::PREDICATE:
            predicate = tok.value;
            predicateValid = true;
            break;
    }
}

TokenStore::TokenStoreEntry* TokenStore::setToken(Token tok) {
    auto entry = getTokenStoreEntry(tok.tag);
    if (entry != nullptr) {
        entry->setToken(tok);
        return entry;
    }
    if (isNotFull()) {
        tokenStore[tok.tag] = TokenStoreEntry(tok);
        return &tokenStore[tok.tag];
    }
    return nullptr;
}

TokenStore::TokenStoreEntry* TokenStore::getTokenStoreEntry(Tag tag) {
    auto entry = tokenStore.find(tag);
    if (entry == tokenStore.end()) {
        return nullptr;
    }
    return &entry->second;
}

}
}
}