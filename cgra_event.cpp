#include "cgra_event.h"
#include "cgra.h"
#include "cgra_pe.h"
#include "cgra_network.h"


namespace platy {
namespace sim {
namespace cgra {

void ExecuteCgraEvent::go(Cgra* cgra) const { 
    ProcessingElement* pe = cgra->getProcessingElement(peid);
    pe->executeInstruction(tsEntry); 
}

void SendTokenCgraEvent::go(Cgra* cgra) const { 
    Network * network = cgra->getNetwork();
    network->sendToken(peid, tok); 
}


}
}
}