#include "shared.h"

// Keep all inputs together
typedef struct
{
    int numRounds;
    ButtonGrid buttons;
    long clock;
} state_inputs_t;

char* s2str(FsmState s) {
  switch(s) {
    case FsmState::s_INIT:
        return "(1) INIT";
    case FsmState::s_CHOOSE_MOLE:
        return "(2) CHOOSE_MOLE";
    case FsmState::s_WAIT:
        return "(3) WAIT";
    case FsmState::s_HIT_MOLE:
        return "(4) HIT_MOLE";
    case FsmState::s_MISS_HIT:
        return "(5) MISS_HIT";
    case FsmState::s_TIME_EXPIRED:
        return "(6) TIME_EXPIRED";
    case FsmState::s_CLEAR_MOLE:
        return "(7) CLEAR_MOLE";
    case FsmState::s_GAME_OVER:
        return "(8) GAME_OVER";
    default:
    return "???";
  }
}

xy_t gridToXY(const ButtonGrid &grid) {
    for (int y = 0; y < grid.size(); ++y) {
        for (int x = 0; x < grid[0].size(); ++x) {
            if (grid[y][x]) {
                return {x, y};
            }
        }
    }
    return {-1, -1};
}

// This forces the compiler to recognize the function uses the struct defined above.
bool testTransition(full_state_t start, full_state_t end, state_inputs_t inputs, bool verbos);

const full_state_t testStatesInitial[10] = {{3, 3, {2, 2}, 7, 2, static_cast<FsmState>(1)}, {5, 3, {0, 0}, 2, 5, static_cast<FsmState>(2)}, {9, 8, {2, 2}, 5, 8, static_cast<FsmState>(2)}, {50, 100, {1, 1}, 10, 5, static_cast<FsmState>(3)}, {3, 7, {0, 0}, 5, 1, static_cast<FsmState>(3)}, {100, 100, {1, 2}, 8, 3, static_cast<FsmState>(3)}, {7, 9, {2, 2}, 4, 8, static_cast<FsmState>(4)}, {3, 4, {2, 2}, 1, 1, static_cast<FsmState>(5)}, {7, 10, {2, 2}, 7, 1, static_cast<FsmState>(6)}, {6, 7, {1, 2}, 4, 10, static_cast<FsmState>(7)}};
const full_state_t testStatesExpected[10] = {{3, 3, {2, 2}, 1, 2, static_cast<FsmState>(2)}, {100, 2, {0, 0}, 2, 5, static_cast<FsmState>(3)}, {9, 8, {2, 2}, 5, 8, static_cast<FsmState>(8)}, {50, 100, {1, 1}, 10, 6, static_cast<FsmState>(4)}, {3, 7, {0, 0}, 5, 0, static_cast<FsmState>(5)}, {100, 100, {1, 2}, 8, 3, static_cast<FsmState>(6)}, {7, 9, {2, 2}, 4, 8, static_cast<FsmState>(7)}, {3, 4, {2, 2}, 1, 1, static_cast<FsmState>(7)}, {7, 10, {2, 2}, 7, 1, static_cast<FsmState>(7)}, {6, 7, {1, 2}, 5, 10, static_cast<FsmState>(2)}};
const state_inputs_t testInputs[10] = {{4, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 6}, {2, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 100}, {6, {{{{false, false, false}}, {{false, false, true}}, {{false, false, false}}}}, 6}, {6, {{{{false, false, false}}, {{false, true, false}}, {{false, false, false}}}}, 100}, {2, {{{{false, false, false}}, {{false, true, false}}, {{false, false, false}}}}, 100}, {2, {{{{false, false, false}}, {{false, false, false}}, {{false, false, false}}}}, 300}, {2, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 10}, {5, {{{{false, false, false}}, {{false, false, true}}, {{false, false, false}}}}, 1}, {8, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 10}, {3, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 3}};
const int numTests = 10;

bool testTransition(full_state_t start, full_state_t end, state_inputs_t inputs, bool verbos)
{
    // const auto actual = updateFSM(start, inputs.clock, inputs.emergencyStop, inputs.hasNewConfiguration, inputs.newestConfiguration);
    const auto actual = updateFSM(start, inputs.numRounds, inputs.buttons, inputs.clock);

    bool passedTest = (actual == end);

    if (!verbos)
    {
        return passedTest;
    }
    else if (passedTest)
    {
        char sToPrint[200];
        sprintf(sToPrint, "Test from %s to %s PASSED", s2str(start.fsmState), s2str(end.fsmState));
        Serial.println(sToPrint);
        return true;
    }
    else
    {
        char outStr[200];

        sprintf(outStr, "Test from %s to %s FAILED", s2str(start.fsmState), s2str(end.fsmState));
        Serial.println(outStr);

        sprintf(outStr, "End state expected: %s | actual %s", s2str(end.fsmState), s2str(actual.fsmState));
        Serial.println(outStr);

        const auto xy = gridToXY(inputs.buttons);
        sprintf(outStr, "Inputs: numRounds %d, button x: %d, button y: %d, clock: %ld", inputs.numRounds, xy.x, xy.y, inputs.clock);
        Serial.println(outStr);

        sprintf(outStr, "          %12s | %14s | %6s | %6s | %12s | %6s |", "moleStartMs", "moleDurationMs", "mole.x", "mole.y", "currentRound", "score");
        Serial.println(outStr);

        sprintf(outStr, "initial:  %12ld | %14ld | %6d | %6d | %12d | %6d |", start.moleStartMs, start.moleDurationMs, start.moleXy.x, start.moleXy.y, start.currentRound, start.score);
        Serial.println(outStr);

        sprintf(outStr, "expected: %12ld | %14ld | %6d | %6d | %12d | %6d |", end.moleStartMs, end.moleDurationMs, end.moleXy.x, end.moleXy.y, end.currentRound, end.score);
        Serial.println(outStr);

        Serial.println("");

        return false;
    }
}

bool testAll() {
    #ifndef TESTING
    Serial.println("Testing not compiled. Need to #define TESTING");
    return false;
    #else
    for (int i = 0; i < numTests; ++i) {
        Serial.print("Running test ");
        Serial.print(i + 1);
        Serial.print(" of ");
        Serial.println(numTests);

        if (!testTransition(testStatesInitial[i], testStatesExpected[i], testInputs[i], true)) {
            return false;
        }
        Serial.println();
    }
    #endif
}