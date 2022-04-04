#include "cgra_network.h"
#include "cgra_pe.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

#if 0
/* nzb: This is a magic network. Rename. */
void BusNetwork::sendToken(PeIdx peid, TokenStore::Token tok) {
    ProcessingElement * pe = cgra->getProcessingElement(peid);
    if (!pe->acceptToken(tok)) {
        qassert(false);
        Cycles newEventTime = cgra->now() + cgra->networkDelay + cgra->setTokenFailDelay;
        CgraEvent* event =  new SendTokenCgraEvent(newEventTime, peid, tok);
        cgra->pushEvent(event);
    }
}
#endif

void BusNetwork::sendToken(NetworkPort* src, const std::vector<Location>& dsts, Word value, CbIdx cbidx) {
    CgraEvent * event = new BusEvent{bandwidthPort.grab(delay), cgra, this, src, dsts, value, cbidx};
    cgra->pushEvent(event);
}

void BusNetwork::BusEvent::go() {
    for (size_t i = dsts.size() - 1; i < dsts.size(); i--) {
        auto dst = dsts[i];
        TokenStore::Token tok{dst.pos, value, dst.inst, cbidx};
        auto* dstPe = cgra->getProcessingElement(dst.pe);
        if (dstPe->acceptToken(tok)) {
            dsts.pop_back();
        }
    }

    bool retry = !dsts.empty();
    if (retry) {
        timestamp = network->bandwidthPort.grab(network->delay);
        cgra->pushEvent(this);
    } else {
            src->acknowledgeToken();
        }
}

}
}
}
