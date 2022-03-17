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
        Cycles newEventTime = cgra->currentTime + cgra->networkDelay + cgra->setTokenFailDelay;
        CgraEvent* event =  new SendTokenCgraEvent(newEventTime, peid, tok);
        cgra->pushEvent(event);
    }
}
#endif

void BusNetwork::sendToken(PeIdx src, const std::vector<Location>& dsts, Word word, CbIdx cbidx) {
    auto event = new BusEvent{this, src, dsts, word, cbidx};
    pushEvent(event);
}

void BusNetwork::BusEvent::BusEvent(...) {
    timestamp = network->port.grab(network->delay);
}

void BusNetwork::BusEvent::go(Cgra* cgra) {
    for (size_t i = dsts.size() - 1; i < dsts.size(); i--) {
        auto dst = dsts[i];
        
        TokenStore::Token tok{dst.pos, word, dst.inst, cbidx};
        auto* dstPe = cgra->getProcessingElement(destination.pe);
        if (pe->acceptToken(tok)) {
            dsts.pop_back();
        }
    }

    bool retry = !dsts.empty();
    if (retry) {
        timestamp = network->port.grab(network->delay);
        cgra->pushEvent(this);
    } else {
        auto* srcPe = cgra->getProcessingElement(src);
        srcPe->acknowledgeToken();
    }
}

}
}
}
