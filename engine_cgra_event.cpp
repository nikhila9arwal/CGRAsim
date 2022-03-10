#include "engine_cgra_event.h"
#include "engine_cgra.h"


namespace platy {
namespace sim {
namespace ms {

void ExecuteCgraEvent::go(CgraEngine* cgra) const { cgra->executeInstruction(pe, tag); }

//TODO (nikhil): have access to network instead of cgra
void SendTokenCgraEvent::go(CgraEngine* cgra) const { cgra->sendToken(pe, tok); }


}
}
}