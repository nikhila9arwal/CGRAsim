
#include "cgra_tok_st.h"

namespace platy {
namespace sim {
namespace cgra {

TokenStore::Tag::Tag(InstrMemIdx _instIdx, CbIdx _cbid) 
: instIdx(_instIdx), cbid(_cbid) {}

bool TokenStore::Tag::operator==(const struct Tag& other) const{  // <  ==> > for min heap
    return instIdx == other.instIdx && cbid == other.cbid;
}

std::size_t TokenStore::Tag::HashFn::operator() (const Tag &node) const
{
    std::size_t h1 = std::hash<InstrMemIdx>()(node.instIdx);
    std::size_t h2 = std::hash<CbIdx>()(node.cbid);
    return h1 ^ h2;
}

TokenStore::Token::Token(PosIdx _posid, Word _value, InstrMemIdx _inst, CbIdx _cbid)
    : posid(_posid), value(_value), tag(_inst, _cbid) {}


TokenStore::Entry::Entry(Token tok)
    : lhsValid(false), rhsValid(false), predicateValid(false), tag(tok.tag) {
    setToken(tok);
}

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

TokenStore::TokenStore(uint32_t _capacity) 
: capacity(_capacity) { }

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

bool TokenStore::isEmpty() { return tokenStore.empty(); }

bool TokenStore::isFull() { return tokenStore.size() == capacity; }
}
}
}
