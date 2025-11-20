#include "shared.h"

// Keep all inputs together
typedef struct
{
    int numRounds;
    ButtonGrid buttons;
    long clock;
} state_inputs_t;

// This forces the compiler to recognize the function uses the struct defined above.
bool testTransition(full_state_t start, full_state_t end, state_inputs_t inputs, bool verbos);

const full_state_t testStatesInitial[17] = {{1, 1, {1, 2}, 4, 10, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(2)}, {1, 1, {1, 2}, 3, 3, static_cast<FsmState>(3)}, {3, 1, {1, 2}, 4, 4, static_cast<FsmState>(4)}, {3, 1, {1, 2}, 1, 1, static_cast<FsmState>(5)}, {2, 1, {1, 2}, 4, 4, static_cast<FsmState>(3)}, {3, 1, {1, 2}, 1, 1, static_cast<FsmState>(1)}, {2, 1, {1, 2}, 1, 1, static_cast<FsmState>(1)}, {3, 1, {1, 2}, 5, 4, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(1)}, {1, 1, {1, 1}, 1, 1, static_cast<FsmState>(1)}, {2, 1, {1, 1}, 10, 5, static_cast<FsmState>(1)}, {3, 1, {1, 1}, 2, 2, static_cast<FsmState>(1)}, {3, 1, {1, 2}, 2, 6, static_cast<FsmState>(1)}, {5, 1, {2, 2}, 2, 2, static_cast<FsmState>(1)}};
const full_state_t testStatesExpected[17] = {{1, 1, {1, 2}, 4, 10, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 2, 2, static_cast<FsmState>(2)}, {1, 1, {1, 2}, 3, 3, static_cast<FsmState>(4)}, {0, 1, {1, 2}, 4, 4, static_cast<FsmState>(4)}, {3, 1, {1, 2}, 1, 1, static_cast<FsmState>(5)}, {3, 1, {1, 2}, 4, 4, static_cast<FsmState>(2)}, {4, 1, {1, 2}, 1, 1, static_cast<FsmState>(1)}, {2, 1, {1, 2}, 1, 1, static_cast<FsmState>(1)}, {3, 1, {1, 2}, 5, 4, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(1)}, {1, 1, {1, 2}, 4, 4, static_cast<FsmState>(1)}, {1, 1, {1, 1}, 1, 1, static_cast<FsmState>(1)}, {2, 1, {1, 1}, 10, 5, static_cast<FsmState>(1)}, {3, 1, {1, 1}, 2, 2, static_cast<FsmState>(1)}, {3, 1, {1, 2}, 2, 6, static_cast<FsmState>(1)}, {5, 1, {2, 2}, 2, 2, static_cast<FsmState>(3)}};
const state_inputs_t testInputs[17] = {{9, {{{{false, false, false}}, {{false, false, false}}, {{false, true, false}}}}, 5}, {10, {{{{false, false, false}}, {{false, false, true}}, {{false, false, false}}}}, 4}, {9, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 6}, {1, {{{{false, false, false}}, {{false, false, false}}, {{false, true, false}}}}, 10}, {4, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 5}, {8, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 2}, {1, {{{{false, false, false}}, {{false, false, true}}, {{false, false, false}}}}, 2}, {7, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 3}, {9, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 8}, {6, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 6}, {7, {{{{false, false, false}}, {{false, false, false}}, {{false, true, false}}}}, 2}, {5, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 3}, {4, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 7}, {7, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 8}, {6, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 8}, {1, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 10}, {4, {{{{false, false, false}}, {{false, false, false}}, {{false, false, true}}}}, 7}};
const int numTests = 17;

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
        Serial.println("Passed some test");
        return true;
    }
    else
    {
        Serial.println("todo!() failed but fill in info");
        return false;
    }
}

bool testAll() {
    #ifndef TESTING
    Serial.println("Testing not compiled. Need to #define TESTING");
    return false;
    #else
    for (int i = 0; i < numTests; ++i) {
        Serial.println("Running test");
        if (!testTransition(testStatesInitial[i], testStatesExpected[i], testInputs[i], true)) {
            return false;
        }
        Serial.println();
    }
    #endif
}