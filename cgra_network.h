
#pragma once

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
    BusNetwork(Cgra* _cgra, int _bandwidth) : Network{_cgra}, port{_bandwidth} {}
    void sendToken(PeIdx src, const std::vector<Location>& dsts, Word word, CbIdx cbidx) override;

  private:
    class BusEvent : public CgraEvent {
      public:
        BusEvent(BusNetwork* _network, PeIdx _src, std::vector<Location> _dsts, Word _word, CbIdx _cbidx);
        void go(Cgra* cgra);

      private:
        BusNetwork* network;
        PeIdx src;
        std::vector<Location> dsts; // TODO (nzb): This can probably
                                    // just be source, in a snooping
                                    // implementation
        Word word;
        CbIdx cbidx;
    };

    Port port;
};

}
}
}
