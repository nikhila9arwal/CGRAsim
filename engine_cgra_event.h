
#pragma once

#include "engine_cgra_defs.h"
#include "engine_cgra_ts.h"

namespace platy {
namespace sim {
namespace ms {

class CgraEngine;
class CgraEvent {
public:
    CgraEvent(uint32_t _timestamp) : timestamp(_timestamp) {}

    bool operator<(const class CgraEvent& other) const  // <  ==> > for min heap
    {
        return timestamp > other.timestamp;
    }

    virtual void go(CgraEngine* cgra) const = 0;
    uint32_t timestamp;
};


class ExecuteCgraEvent : public CgraEvent {
public:
    ExecuteCgraEvent(uint32_t _timestamp, PeIdx _pe, InstrMemIdx _instrId, CbIdx _cb)
        : CgraEvent(_timestamp), pe(_pe), tag(_instrId, _cb) {}
    ExecuteCgraEvent(uint32_t _timestamp, PeIdx _pe, TokenStore::Tag _tag)
        : CgraEvent(_timestamp), pe(_pe), tag(_tag) {}
    void go(CgraEngine* cgra) const;

private:
    PeIdx pe;
    TokenStore::Tag tag;
};

class SendTokenCgraEvent : public CgraEvent {
public:
    SendTokenCgraEvent(uint32_t _timestamp, PeIdx _pe, TokenStore::Token _tok)
        : CgraEvent(_timestamp), pe(_pe), tok(_tok) {}
    void go(CgraEngine* cgra) const;

private:
    PeIdx pe;
    TokenStore::Token tok;
};

}
}
}
