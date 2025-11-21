#include "shared.h"

// Keep all inputs together
typedef struct
{
    int numRounds;
    ButtonGrid buttons;
    long distanceToGo;
    long clock;
} state_inputs_t;

char* s2str(FsmState s) {
  switch(s) {
    case FsmState::s_INIT:
        return "(1) INIT";
    case FsmState::s_CHOOSE_MOLE:
        return "(2) CHOOSE_MOLE";
    case FsmState::s_RAISE_MOLE:
        return "(3) RAISE_MOLE";
    case FsmState::s_WAIT:
        return "(4) WAIT";
    case FsmState::s_HIT_MOLE:
        return "(5) HIT_MOLE";
    case FsmState::s_MISS_HIT:
        return "(6) MISS_HIT";
    case FsmState::s_TIME_EXPIRED:
        return "(7) TIME_EXPIRED";
    case FsmState::s_CLEAR_MOLE:
        return "(8) CLEAR_MOLE";
    case FsmState::s_GAME_OVER:
        return "(9) GAME_OVER";
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

const full_state_t testStatesInitial[13] = {{10, 3, {2, 2}, 3, 2,9, static_cast<FsmState>(1)}, {1, 3, {0, 0}, 3, 8,3, static_cast<FsmState>(2)}, {1, 8, {2, 2}, 4, 3,8, static_cast<FsmState>(2)}, {3, 5, {2, 2}, 3, 10,7, static_cast<FsmState>(3)}, {7, 8, {1, 2}, 5, 7,5, static_cast<FsmState>(3)}, {50, 2, {1, 1}, 8, 6,5, static_cast<FsmState>(4)}, {3, 1, {0, 0}, 5, 4,1, static_cast<FsmState>(4)}, {100, 100, {1, 2}, 10, 3,8, static_cast<FsmState>(4)}, {9, 7, {2, 2}, 1, 4,9, static_cast<FsmState>(5)}, {6, 1, {2, 2}, 9, 6,5, static_cast<FsmState>(6)}, {6, 4, {2, 2}, 6, 8,4, static_cast<FsmState>(7)}, {9, 10, {1, 2}, 4, 1,3, static_cast<FsmState>(8)}, {4, 9, {2, 1}, 3, 1,9, static_cast<FsmState>(8)}};
const full_state_t testStatesExpected[13] = {{0, 0, {-1, -1}, 1, 2,0, static_cast<FsmState>(2)}, {1, 1500, {1, 1}, 3, 8,3, static_cast<FsmState>(3)}, {1, 8, {2, 2}, 4, 3,8, static_cast<FsmState>(9)}, {3, 5, {2, 2}, 3, 10,7, static_cast<FsmState>(3)}, {150, 8, {1, 2}, 5, 7,5, static_cast<FsmState>(4)}, {50, 2, {1, 1}, 8, 6,6, static_cast<FsmState>(5)}, {3, 1, {0, 0}, 5, 4,0, static_cast<FsmState>(6)}, {100, 100, {1, 2}, 10, 3,8, static_cast<FsmState>(7)}, {9, 7, {2, 2}, 1, 4,9, static_cast<FsmState>(8)}, {6, 1, {2, 2}, 9, 6,5, static_cast<FsmState>(8)}, {6, 4, {2, 2}, 6, 8,4, static_cast<FsmState>(8)}, {9, 10, {1, 2}, 5, 1,3, static_cast<FsmState>(2)}, {4, 9, {2, 1}, 3, 1,9, static_cast<FsmState>(8)}};
const state_inputs_t testInputs[13] = {{4, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 6, 9}, {8, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 10, 100}, {4, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 7, 3}, {7, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 100, 9}, {7, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 0, 150}, {7, {{{{false, false, false}}, {{false, true, false}}, {{false, false, false}}}}, 5, 100}, {4, {{{{false, false, false}}, {{false, true, false}}, {{false, false, false}}}}, 10, 100}, {7, {{{{false, false, false}}, {{false, false, false}}, {{false, false, false}}}}, 7, 300}, {9, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 4, 7}, {6, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 7, 7}, {5, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 4, 1}, {5, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 0, 6}, {5, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 100, 2}};
const int numTests = 13;

bool testTransition(full_state_t start, full_state_t end, state_inputs_t inputs, bool verbos)
{
    const auto actual = updateFSM(start, inputs.numRounds, inputs.buttons, inputs.distanceToGo, inputs.clock);

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
        sprintf(outStr, "Inputs: numRounds %d, button x: %d, button y: %d, distanceToGo: %ld, clock: %ld", inputs.numRounds, xy.x, xy.y, inputs.distanceToGo, inputs.clock);
        Serial.println(outStr);

        sprintf(outStr, "          %12s | %14s | %6s | %6s | %12s | %12s | %6s |", "moleStartMs", "moleDurationMs", "mole.x", "mole.y", "currentRound", "totalRounds", "score");
        Serial.println(outStr);

        sprintf(outStr, "initial:  %12ld | %14ld | %6d | %6d | %12d | %12d | %6d |", start.moleStartMs, start.moleDurationMs, start.moleXy.x, start.moleXy.y, start.currentRound, start.totalRounds, start.score);
        Serial.println(outStr);

        sprintf(outStr, "expected: %12ld | %14ld | %6d | %6d | %12d | %12d | %6d |", end.moleStartMs, end.moleDurationMs, end.moleXy.x, end.moleXy.y, end.currentRound, end.totalRounds, end.score);
        Serial.println(outStr);

        sprintf(outStr, "actual:   %12ld | %14ld | %6d | %6d | %12d | %12d | %6d |", actual.moleStartMs, actual.moleDurationMs, actual.moleXy.x, actual.moleXy.y, actual.currentRound, actual.totalRounds, actual.score);
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
    Serial.println("All tests passed!");
    return true;
    #endif
}