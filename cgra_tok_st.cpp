
#include "cgra_tok_st.h"

namespace platy {
namespace sim {
namespace cgra {

void TokenStore::Entry::setToken(Token tok) {
    switch (tok.posid) {
        case PosIdx::LHS: {
            lhs = tok.value;
            lhsValid = true;
        } break;
            
        case PosIdx::RHS: {
            rhs = tok.value;
            rhsValid = true;
        } break;
            
        case PosIdx::PREDICATE: {
            predicate = tok.value;
            predicateValid = true;
        } break;
    }
}

TokenStore::EntryPtr TokenStore::acceptToken(Token tok) {
    auto entry = at(tok.tag);
    if (entry != nullptr) {
        entry->setToken(tok);
        return entry;
    } else if (isFull()) {
        return nullptr;
    } else {
        auto ptr = std::make_shared<TokenStore::Entry>(tok);
        tokenStore.insert(std::make_pair(tok.tag, ptr));
        return ptr;
    }
}

TokenStore::EntryPtr TokenStore::at(Tag tag) {
    auto entry = tokenStore.find(tag);
    if (entry == tokenStore.end()) {
        return nullptr;
    } else {
        return entry->second;
    }
}

void TokenStore::erase(Tag tag) {
    auto nremoved = tokenStore.erase(tag);
    assert (nremoved == 1);
}

}
}
}
