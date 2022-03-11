
#include "cgra_ts.h"

namespace platy {
namespace sim {
namespace cgra {

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

std::shared_ptr<TokenStore::TokenStoreEntry> TokenStore::acceptToken(Token tok) {
    auto entry = getTokenStoreEntry(tok.tag);
    if (entry != nullptr) {
        entry->setToken(tok);
        return entry;
    }
    if (isEmpty()) {
        std::pair<Tag,TokenStoreEntry> toInsert(tok.tag, tok);
        // tokenStore[tok.tag] = TokenStoreEntry(tok);
        // return &tokenStore[tok.tag];
        tokenStore.insert(toInsert);
        return std::shared_ptr<TokenStore::TokenStoreEntry> (&tokenStore.at(tok.tag));
    }
    return nullptr;
}

std::shared_ptr<TokenStore::TokenStoreEntry> TokenStore::getTokenStoreEntry(Tag tag) {
    auto entry = tokenStore.find(tag);
    if (entry == tokenStore.end()) {
        return nullptr;
    }
    // return &tokenStore[tag];
    return std::shared_ptr<TokenStore::TokenStoreEntry> (&tokenStore.at(tag));
}

}
}
}