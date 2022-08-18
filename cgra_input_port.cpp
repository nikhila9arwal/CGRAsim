
#include "cgra_defs.h"
#include "cgra_network.h"
#include "cgra.h"
#include "cgra_input_port.h"
#include "port.cpp"

namespace platy {
namespace sim {
namespace cgra {

InputPort::InputPort(Cgra* _cgra, Network * _network) 
: cgra(_cgra), network(_network), inputPort(1, _cgra){}

InputPort::Input::Input(Word _value, std::vector<Location> _destinations, CbIdx _cbid) 
: value(_value), destinations(_destinations), cbid(_cbid) {}

bool InputPort::acceptToken(Word value, std::vector<Location>destinations, CbIdx cbid){
    //should acquire a port and send token
    //if port busy push to queue}
    if (runtimeInputsQueueSize == runtimeInputsQueue.size()) {
        return false;
    }
    if (inputPort.isAvailable()) {
        inputPort.acquire();
        network->sendToken(this, destinations, value, cbid);
        // Cycles execTime = execStage.acquire() + frontEndLatency;
    } else {
        // return here;
        // TODO (nikhil): push destinations also somewhere;
        runtimeInputsQueue.push_back(Input(value, destinations, cbid));
    }

    return true;

}

//should free the port. if queue not empty send the first token
//reacquire the port
void InputPort::acknowledgeToken(){
    qassert(!inputPort.isAvailable());

    inputPort.release(0_cycles);
    if (!runtimeInputsQueue.empty()) {
        auto input = runtimeInputsQueue.front();
        runtimeInputsQueue.pop_front();
        inputPort.acquire();
        network->sendToken(this, input.destinations, input.value, input.cbid);
    }
}
}
}
}