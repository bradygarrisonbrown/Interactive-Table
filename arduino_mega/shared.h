#ifndef SHARED_H
#define SHARED_H

// UNCOMMENT to run tests!
#define TESTING

// COMMENT OUT for testing!!
// #define ENABLE_WDT

// UNCOMMENT when testing, no calibration
#define NEED_CALIBRATION

// Dimensions of our grid
namespace Constants
{
    inline constexpr int WIDTH = 3;
    inline constexpr int HEIGHT = 3;
    // TODO: find correct value
    inline constexpr long MOLE_RISE_HEIGHT = 4096 * 3;
    inline constexpr long DEFAULT_MOLE_DURATION = 3000;
}

// Need a struct because you can't return arrays in old C++ (?)
struct ButtonGrid {
  bool buttons[Constants::HEIGHT][Constants::WIDTH];
};

struct xy_t {
    int x;
    int y;

    void print() {
        Serial.print("{x: ");
        Serial.print(x);
        Serial.print(", y: ");
        Serial.print(y);
        Serial.print("}");
    }
};

inline bool operator==(const xy_t &lhs, const xy_t &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

enum class FsmState {
    s_INIT = 1,
    s_CHOOSE_MOLE = 2,
    s_RAISE_MOLE = 3,
    s_WAIT = 4,
    s_HIT_MOLE = 5,
    s_MISS_HIT = 6,
    s_TIME_EXPIRED = 7,
    s_CLEAR_MOLE = 8,
    s_GAME_OVER = 9,
};

struct full_state_t
{
    long moleStartMs;
    long moleDurationMs;
    xy_t moleXy;
    int currentRound;
    int totalRounds;
    int score;
    FsmState fsmState;
};

inline bool operator==(const full_state_t &lhs, const full_state_t &rhs) {
    return lhs.moleStartMs == rhs.moleStartMs &&
    lhs.moleDurationMs == rhs.moleDurationMs &&
    lhs.moleXy == rhs.moleXy &&
    lhs.currentRound == rhs.currentRound &&
    lhs.score == rhs.score &&
    lhs.fsmState == rhs.fsmState;
}

full_state_t updateFSM(full_state_t currState,
                       int numRounds,
                       ButtonGrid buttons,
                       long moleDistanceToGo,
                       unsigned long clock);

#endif
