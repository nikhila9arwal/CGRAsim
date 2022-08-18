
#pragma once

#include "port.h"
#include "cgra_defs.h"
#include "cgra_event.h"

namespace platy {
namespace sim {
namespace cgra {

class Cgra;
class NetworkPort;

class Network {
public:
    Network(Cgra* _cgra) : cgra(_cgra) {}
    virtual ~Network() {}
    // TODO (nikhil): make this templated
    virtual void sendToken(NetworkPort* src, const std::vector<Location>& dsts, Word word,
            CbIdx cbidx) = 0;

protected:
    Cgra* cgra;

};

class BusNetwork : public Network {
public:
    BusNetwork(Cgra* _cgra, int _bandwidth);
    void sendToken(NetworkPort* src, const std::vector<Location>& dsts, Word word,
            CbIdx cbidx) override;
    const Cycles delay = 1_cycles;
    Port bandwidthPort;

private:
    class BusEvent : public CgraEvent {
    public:
        BusEvent(Cgra* _cgra, BusNetwork* _network, NetworkPort* _src,
                 const std::vector<Location> &_dsts, Word _value, CbIdx _cbidx);
        void go() override;
        void printInfo() override;

    private:
        Cgra* cgra;
        BusNetwork* network;
        NetworkPort* src;
        std::deque<Location> dsts; // TODO (nzb): This can probably
                                    // just be source, in a snooping
                                    // implementation
        Word value;
        CbIdx cbidx;
    };

};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
