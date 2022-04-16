
#pragma once

#include <array> 
#include <queue>
#include <string>
#include <unordered_map>
#include <bits/stdc++.h>
#include <stdint.h>
#include "strong_int.h"
#include "strong_vec.h"
#include "config.h"
#include <assert.h>
#include "qassert.h"
#include <memory>
#include <algorithm>
#include <utility>  // for std::pair

using std::cout; // TODO (by): Remove when done with debugging?

// TODO (nzb): Remove when merged.
DERIVE_STRONGER_INT(Cycles, int32_t, cycles)
const size_t MAX_ARGS = 6;
const size_t MAX_ARG_BYTES = MAX_ARGS * sizeof(uint64_t);
DERIVE_STRONGER_INT(ProcIdx, int32_t, pid);
DERIVE_STRONGER_INT(ThreadIdx, int32_t, tid);

DERIVE_STRONGER_INT(PeIdx, int32_t, peid)
DERIVE_STRONGER_INT(OpIdx, int32_t, opid)
DERIVE_STRONGER_INT(CbIdx, int32_t, cbid)
DERIVE_STRONGER_INT(InstrMemIdx, int32_t, instid)
DERIVE_STRONGER_INT(TokenStIdx, int32_t, tokenstid)

namespace platy {
namespace sim {
namespace cgra {

enum PosIdx { LHS, RHS, PREDICATE };
enum InstType {
    NOP,
    NOT,
    AND,
    OR,
    XOR,
    LSHIFT,
    RSHIFT,
    ADD,
    SUB,
    MUL,
    DIV,
    LESSTHAN,
    GREATERTHAN,
    EQUALTO,
    LOAD,
    STORE
};

typedef int64_t Word;  // TODO (nikhil): uint64_t maybe?
// static const size_t NUM_VALUE_ELEMENTS = LINE_SIZE / sizeof(Word);
// typedef std::array<Word, NUM_VALUE_ELEMENTS> Value;

// pass CGRA* to procElem so it can use these methods

struct Location {
    PeIdx pe;
    InstrMemIdx inst;
    PosIdx pos;  // TODO (nikhil): Use enum here -- 0 for lhs, 1 for rhs, 2 for predicate

    // TODO (nzb): Move implementation to cpp?
    Location(PeIdx _pe, InstrMemIdx _inst, PosIdx _pos);
    Location(Config& bitstream, std::string key);
    inline void loadBitstream(Config& bitstream, std::string key);
};

//TODO(nikhil): Remove when merged
struct FunctionConfiguration {
    std::string filename;  // Description of hardware (e.g., CGRA) configuration.
    void* functionPtr;     // Userspace C++ function pointer.
    // If the function is a member function of a class, then "context" must point to
    // an instance of that class. Otherwise, "context" should be nullptr.
    //
    // If it is a member function, then
    // (1) Engines using "functionPtr" will always execute on "context", or
    // (2) Engines using "filename" will extract relevant metadata from the "context"
    //     during "configure()" based on the compiled configuration.
    //
    // Currently, Livia does not use member functions but Wunderpus does.
    bool isMemberFunction;
    void* context;
};

//TODO(nikhil): Remove when merged
class TaskReq {
public:
    TaskReq(
        const ProcIdx _pid,
        const ThreadIdx _tid,
        void* _task,
        const uint64_t* const _args,
        const uint32_t _argBytes)
        : pid(_pid), tid(_tid), task(_task), argBytes(_argBytes) {//,cv(nullptr) {
        memcpy(args, _args, argBytes);
    }

    virtual size_t getMessageSize() const {
        // An actual task message contains a
        // - Task pointer: 8 bytes
        // - Args: argBytes
        const auto bytes = sizeof(void*) + argBytes;
        return bytes * 8;
    }

    // Size in *bits* for compatability with network::send
    ProcIdx pid;
    ThreadIdx tid;

    // Identifies the function (and context, if needed) to execute on an engine. If
    // running on a CoreEngine, then the actual function pointer is executed in userspace
    // using a memory-service thread. Otherwise, the function pointer is used as an
    // identifier for a previously configured function on the engine.
    void* task;

    // Contains all arguments to be passed to the function in userspace. Each specific
    // memory service system will subclass TaskReq in order to provide getter function for
    // needed arguments within platy.
    uint64_t args[MAX_ARGS];

    // Size of the arguments in bytes. The arguments for this task can be less than
    // MAX_ARG_BYTES.
    uint64_t argBytes;

    // TODO (nikhil): This condition variable is notified when the task completes.
    // ConditionVariable* cv;
};

}  // namespace cgra
}  // namespace sim
}  // namespace platy
