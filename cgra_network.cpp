#include "cgra_network.h"
#include "cgra_pe.h"
#include "cgra.h"

#define DEBUG_CGRA_NETWORK
#ifdef DEBUG_CGRA_NETWORK
#define DBG(...) qlog(__VA_ARGS__)
#else
#define DBG(...)
#endif

namespace platy {
namespace sim {
namespace cgra {

#if 0
/* nzb: This is a magic network. Rename. */
void BusNetwork::sendToken(PeIdx peid, TokenStore::Token tok) {
    ProcessingElement * pe = cgra->getProcessingElement(peid);
    if (!pe->acceptToken(tok)) {
        qassert(false);
        Cycles newEventTime = events::now() + cgra->networkDelay + cgra->setTokenFailDelay;
        CgraEvent* event =  new SendTokenCgraEvent(newEventTime, peid, tok);
        cgra->pushEvent(event);
    }
}
#endif

BusNetwork::BusNetwork(Cgra* _cgra, int _bandwidth) 
: Network{_cgra}, bandwidthPort{_bandwidth, _cgra} {}

void BusNetwork::sendToken(NetworkPort* src, const std::vector<Location>& dsts, Word value, CbIdx cbidx) {
    Cycles timestamp = bandwidthPort.grab(delay);
    CgraEvent* event = new BusEvent{cgra, this, src, dsts, value, cbidx};
    cgra->pushEvent(event, timestamp);
}

BusNetwork::BusEvent::BusEvent(Cgra* _cgra, BusNetwork* _network, NetworkPort* _src,
                 const std::vector<Location> &_dsts, Word _value, CbIdx _cbidx)
            : CgraEvent(), cgra(_cgra), network(_network), src(_src),
              dsts(_dsts.begin(), _dsts.end()), value(_value), cbidx(_cbidx) {}

void BusNetwork::BusEvent::printInfo(){
    DBG("BusEvent Source = {} \n", src->getId());
}

void BusNetwork::BusEvent::go() {
    for (size_t i = 0; i < dsts.size(); i++) {
        auto dst = dsts.front();
        TokenStore::Token tok{dst.pos, value, dst.inst, cbidx};
        auto* dstPe = cgra->getProcessingElement(dst.pe);
        dsts.pop_front();
        if (!dstPe->acceptToken(tok)) {
            dsts.push_back(dst);
        }
    }

    bool retry = !dsts.empty();
    if (retry) {
        Cycles timestamp = network->bandwidthPort.grab(network->delay, 1_cycles);
        cgra->pushEvent(this, timestamp);
    } else {
        src->acknowledgeToken();
    }
}

}
}
}
