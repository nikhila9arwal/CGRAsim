#pragma once

#include "cgra_defs.h"
#include "cgra_network_port.h"
#include "port.h"


namespace platy {
namespace sim {
namespace cgra {

class Cgra;
class Network;

class InputPort : public NetworkPort{
public:

    InputPort(Cgra* _cgra, Network * _network) : cgra(_cgra), network(_network), inputPort(1, _cgra){}
    bool acceptToken(Word value, std::vector<Location>destinations, CbIdx cbid);
    void acknowledgeToken();
    struct Input{
        Input(Word _value, std::vector<Location> _destinations, CbIdx _cbid) : value(_value), destinations(_destinations), cbid(_cbid) {}
        Word value;
        std::vector<Location> destinations;
        CbIdx cbid;
    };

private:
    const unsigned runtimeInputsQueueSize = 100;
    std::deque<Input> runtimeInputsQueue;
    Cgra * cgra;
    Network * network;
    Port inputPort;
};


}
}
}