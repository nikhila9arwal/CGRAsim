#pragma once

#include "cgra_defs.h"
#include "cgra_network_port.h"
#include "port.h"



namespace platy {
namespace sim {
namespace cgra {

// class Cgra;
class Network;

class InputPort : public NetworkPort{
public:

    InputPort(Network * _network);
    bool acceptToken(Word value, std::vector<Location>destinations, CbIdx cbid);
    void acknowledgeToken() override;
    struct Input{
        Input(Word _value, std::vector<Location> _destinations, CbIdx _cbid);
        Word value;
        std::vector<Location> destinations;
        CbIdx cbid;
    };

    int getId() override {return -1;}

private:
    const unsigned runtimeInputsQueueSize = 100;
    std::deque<Input> runtimeInputsQueue;
    // Cgra * cgra;
    Network * network;
    Port inputPort;
};


}
}
}