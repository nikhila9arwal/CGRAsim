#pragma once

// #include "../engine.h"

namespace platy {
namespace sim {
namespace cgra {

class NetworkPort{
public:
    // TODO (nikhil): This shouldn't be part of the base class since since 
    // arguments would vary between inputPort and Pe
    // virtual bool acceptToken(TokenStore::Token tok) = 0;
    virtual void acknowledgeToken() = 0;
    virtual int getId() = 0;

};

}
}
}