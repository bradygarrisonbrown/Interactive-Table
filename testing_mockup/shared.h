#pragma once

#define TESTING

#include <array>

// Dimensions of our grid
namespace Constants
{
    inline constexpr int WIDTH = 3;
    inline constexpr int HEIGHT = 3;
}

using ButtonGrid = std::array<std::array<bool, 3>, 3>;

typedef struct {
    int x;
    int y;
} xy_t;

inline bool operator==(const xy_t &lhs, const xy_t &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

enum class FsmState {
    s_INIT = 0,
    s_CHOOSE_MOLE = 1,
    s_RAISE_MOLE = 2,
    s_WAIT = 3,
    s_HIT_MOLE = 4,
    s_MISS_HIT = 5,
    s_TIME_EXPIRED = 6,
    s_CLEAR_MOLE = 7,
    s_GAME_OVER = 8,
};

struct full_state_t
{
    long moleStartMs;
    long moleDurationMs;
    xy_t moleXy;
    int currentRound;
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

full_state_t updateFSM(full_state_t currState, int numRounds, ButtonGrid buttons, unsigned long clock);
