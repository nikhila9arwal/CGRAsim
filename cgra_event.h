
#pragma once

#include "cgra_defs.h"
#include "cgra_ts.h"

namespace platy {
namespace sim {
namespace cgra {

class Cgra;
class CgraEvent {
public:
    CgraEvent(Cycles _timestamp) : timestamp(_timestamp) {}

    bool operator<(const class CgraEvent& other) const // <  ==> > for min heap
    {
        return timestamp > other.timestamp;
    }

    virtual void go(Cgra* cgra) const = 0;
    Cycles timestamp;
};


class ExecuteCgraEvent : public CgraEvent {
public:
    ExecuteCgraEvent(Cycles _timestamp, PeIdx _peid, TokenStore::EntryPtr _tsEntry)
        : CgraEvent(_timestamp), peid(_peid), tsEntry(_tsEntry) {}
    void go(Cgra* cgra) const;


private:
    PeIdx peid;
    TokenStore::EntryPtr tsEntry;
    // TokenStore::Tag tag;
};

class SendTokenCgraEvent : public CgraEvent {
public:
    SendTokenCgraEvent(Cycles _timestamp, PeIdx _peid, TokenStore::Token _tok)
        : CgraEvent(_timestamp), peid(_peid), tok(_tok) {}
    void go(Cgra* cgra) const;

private:
    PeIdx peid;
    TokenStore::Token tok;
};

struct Cmprtr {
    bool operator()(const CgraEvent* first, const CgraEvent* second) const // <  ==> > for min heap
        {
            return first->timestamp > second->timestamp;
        }
};

}
}
}
