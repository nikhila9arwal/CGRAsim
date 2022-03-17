#pragma once

class Port {
  public:
    Port(int width) : available(width, 0) {}
    ~Port() {}

    /* @brief Return the first time that this Port will be available.
     */
    Cycles grab(Cycles occupancy = 1_cycles) {
        auto time = acquire();
        release(occupancy);
        return time;
    }

    Cycles acquire() {
        qassert(!empty());
        auto time = std::max(time::now() /* TODO (nzb): How to get this??? */, available.front());
        available.pop_front();
        return time;
    }

    void release(Cycles occupancy = 1_cycles) {
        available.push_back(time::now() + occupancy);
    }

    bool empty() {
        return available.empty();
    }

  private:
    std::deque<Cycles> available;
};
