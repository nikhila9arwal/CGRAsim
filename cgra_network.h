
#pragma once

#include "port.h"
#include "cgra_defs.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

class Network {
public:
    Network(Cgra* _cgra) : cgra(_cgra) {}
    virtual ~Network() {}
    // TODO (nikhil): make this templated
    virtual void sendToken(PeIdx src, const std::vector<Location>& dsts, Word word, CbIdx cbidx) = 0;

protected:
    Cgra* cgra;

};

class BusNetwork : public Network {
public:
    BusNetwork(Cgra* _cgra, int _bandwidth) : Network{_cgra}, bandwidthPort{_bandwidth, _cgra} {}
    void sendToken(PeIdx src, const std::vector<Location>& dsts, Word word, CbIdx cbidx);
    const Cycles delay = 1_cycles;
    Port bandwidthPort;

  private:
    class BusEvent : public CgraEvent {
      public:
        BusEvent(Cycles _timestamp, Cgra* _cgra, BusNetwork * _network, PeIdx _src, const std::vector<Location>& _dsts, Word _value, CbIdx _cbidx)
        : CgraEvent(_timestamp), cgra(_cgra), network(_network), src(_src), dsts(_dsts), value(_value), cbidx(_cbidx) {}
        void go();

      private:
        Cgra* cgra;
        BusNetwork* network;
        PeIdx src;
        std::vector<Location> dsts; // TODO (nzb): This can probably
                                    // just be source, in a snooping
                                    // implementation
        Word value;
        CbIdx cbidx;
    };

};

}
}
}
