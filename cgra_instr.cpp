#include "cgra_instr.h"
#include "cgra.h"

namespace platy {
namespace sim {
namespace cgra {

Instruction::Instruction()
    : isPredicated{false},
      isLhsImm{false},
      isRhsImm{false},
      lhsImm{},
      rhsImm{} {
}

Instruction::~Instruction() {
}

void Instruction::loadBitstream(Config& bitstream, std::string key, void* functionPtr, Cgra* cgra) {
    std::string type = bitstream.get<const char*>(key + ".type", "");
    qassert(type != "");
    decode(type);
    // TODO (nikhil): change in cfg to lhsImm
    if (bitstream.exists(key + ".imm_lhs")) {
        lhsImm = bitstream.get<int32_t>(key + ".imm_lhs");
        isLhsImm = true;
    }
    // TODO (nikhil): change in cfg to rhsImm
    if (bitstream.exists(key + ".imm_rhs")) {
        rhsImm = bitstream.get<int32_t>(key + ".imm_rhs");
        isRhsImm = true;
    }
    // TODO (nikhil): change in cfg to isTriggered
    if (bitstream.exists(key + ".isPredicated")) {
        isPredicated = (bool)bitstream.get<int32_t>(key + ".isPredicated");
    }
    for (int i = 0;; i++) {
        if (bitstream.exists(key + qformat(".dest_{}", i))) {
            Location destination(bitstream, key + qformat(".dest_{}", i));
            PhysicalInstAddr paddr = cgra->translateVirtualInstAddr(VirtualInstAddr{functionPtr, destination.pe, destination.inst});
            destination.pe = paddr.peidx;
            destination.inst = paddr.instidx;
            destinations.push_back(destination);
        } else {
            break;
        }
    }
}

void Instruction::decode(std::string type) {
    if (type == "NOP") {
        applyFn = [](Word lhs, Word rhs) { return lhs + rhs; };
    } else if (type == "NOT") {
        applyFn = [](Word lhs, Word rhs) { return ~(lhs + rhs); };
    } else if (type == "AND") {
        applyFn = [](Word lhs, Word rhs) { return lhs & rhs; };
    } else if (type == "OR") {
        applyFn = [](Word lhs, Word rhs) { return lhs | rhs; };
    } else if (type == "XOR") {
        applyFn = [](Word lhs, Word rhs) { return lhs ^ rhs; };
    } else if (type == "LSHIFT") {
        applyFn = [](Word lhs, Word rhs) { return lhs << rhs; };
    } else if (type == "RSHIFT") {
        applyFn = [](Word lhs, Word rhs) { return lhs >> rhs; };
    } else if (type == "ADD") {
        applyFn = [](Word lhs, Word rhs) { return lhs + rhs; };
    } else if (type == "SUB") {
        applyFn = [](Word lhs, Word rhs) { return lhs - rhs; };
    } else if (type == "MUL") {
        applyFn = [](Word lhs, Word rhs) { return lhs * rhs; };
    } else if (type == "DIV") {
        applyFn = [](Word lhs, Word rhs) { return lhs / rhs; };
    } else if (type == "LESSTHAN") {
        applyFn = [](Word lhs, Word rhs) { return lhs < rhs ? (Word)1 : (Word)0; };
    } else if (type == "GREATERTHAN") {
        applyFn = [](Word lhs, Word rhs) { return lhs > rhs ? (Word)1 : (Word)0; };
    } else if (type == "EQUALTO") {
        applyFn = [](Word lhs, Word rhs) { return lhs == rhs ? (Word)1 : (Word)0; };
    } else if (type == "LOAD") {  // TODO (nikhil): load using simulator method --
                                  // readFromApp(ProcIdx proc, const void* appPtr,
                                  // T& val) in rw_app.h. proc = 1
        // TODO (nikhil): Do a cache request. Look at simple core.
        // Stall till reply. Reply gives permission to do readFromApp.
        applyFn = [](Word lhs, Word rhs) {
            return  * ((Word*)lhs+ rhs);
            // Word readVal;
            // readFromApp<Word>(1_pid, (void*)((Word*)lhs + rhs), readVal);
            // return readVal;
            // lhs++; //unused param
            // return rhs;
        };
    } else if (type == "STORE") {  // TODO (nikhil): load using simulator method
                                   // --
        // writeToApp(ProcIdx proc, const void* appPtr, T& val)
        // in rw_app.h. proc = 1
        // TODO (nikhil): Do an exclusive cache request. Look at simple core.
        // Stall till reply. Reply gives permission to do writeToApp.
        applyFn = [](Word lhs, Word rhs) {
            *(Word*)lhs  = rhs; return rhs;
            // Word readVal;
            // writeToApp<Word>(1_pid, (void*)((Word*)lhs), rhs);
            // lhs++;// unused param
            // return rhs;
        };
    }

    // else if ( type == "SELECT"){
    //     applyFn = [](Word lhs, Word rhs){if (lhs) return rhs; else return -1;};
    // }//else if ( type == "MERGE"){
    //     applyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    // }else if ( type == "SPLIT"){
    //     applyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    // }else if ( type == "PRINT"){
    //     applyFn = [](Word lhs, Word rhs){return lhs * rhs;};
    // }
}

void Instruction::setStaticParam(Location& loc, Word param) {
    qassert(loc.pos == PosIdx::LHS || loc.pos == PosIdx::RHS);
    if (loc.pos == PosIdx::LHS) {
        isLhsImm = true;
        lhsImm = param;
    } else if (loc.pos == PosIdx::RHS) {
        isRhsImm = true;
        rhsImm = param;
    }
}

} // namespace cgra
} // namespace sim
} // namespace platy
