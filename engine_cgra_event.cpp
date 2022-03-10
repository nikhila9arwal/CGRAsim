#include "engine_cgra_event.h"
#include "engine_cgra.h"


namespace platy {
namespace sim {
namespace ms {

void ExecuteCgraEvent::go(CgraEngine* cgra) const { cgra->executeInstruction(pe, tag); }

void SendTokenCgraEvent::go(CgraEngine* cgra) const { cgra->sendToken(pe, tok); }


}
}
}