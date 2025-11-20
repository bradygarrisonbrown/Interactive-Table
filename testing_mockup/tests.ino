#include "shared.h"

// Keep all inputs together
typedef struct
{
    bool emergencyStop;
    bool hasNewConfiguration;
    configuration_t newestConfiguration;
    unsigned long clock;
} state_inputs_t;

// This forces the compiler to recognize the function uses the struct defined above.
bool testTransition(full_state_t start, full_state_t end, state_inputs_t inputs, bool verbos);

const full_state_t testStatesInitial[17] = {{{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 10}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 3}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 4}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 1}, {{1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 4}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 1}, {{1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 1}, {{1, {{9, 9, 9}, {9, 9, 9}, {9, 9, 9}}}, 10}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 1}, {{1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 6}, {{1, {{4, 4, 4}, {4, 4, 4}, {4, 4, 4}}}, 10}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 1}, {{2, {{4, 4, 4}, {4, 4, 4}, {4, 4, 4}}}, 2}};
const full_state_t testStatesExpected[17] = {{{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 10}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 2}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 3}, {{1, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}}, 4}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 1}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 4}, {{1, {{4, 4, 4}, {4, 4, 4}, {4, 4, 4}}}, 1}, {{1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 1}, {{1, {{9, 9, 9}, {9, 9, 9}, {9, 9, 9}}}, 10}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 4}, {{1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 1}, {{1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 6}, {{1, {{4, 4, 4}, {4, 4, 4}, {4, 4, 4}}}, 10}, {{1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 1}, {{1, {{4, 4, 4}, {4, 4, 4}, {4, 4, 4}}}, 2}};
const state_inputs_t testInputs[17] = {{false, true, {1, {{6, 6, 6}, {6, 6, 6}, {6, 6, 6}}}, 4}, {false, true, {1, {{7, 7, 7}, {7, 7, 7}, {7, 7, 7}}}, 7}, {false, false, {1, {{3, 3, 3}, {3, 3, 3}, {3, 3, 3}}}, 3}, {true, true, {1, {{10, 10, 10}, {10, 10, 10}, {10, 10, 10}}}, 3}, {true, true, {1, {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}, 2}, {false, false, {1, {{10, 10, 10}, {10, 10, 10}, {10, 10, 10}}}, 10}, {false, false, {1, {{7, 7, 7}, {7, 7, 7}, {7, 7, 7}}}, 6}, {true, true, {1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 6}, {false, false, {1, {{10, 10, 10}, {10, 10, 10}, {10, 10, 10}}}, 3}, {true, true, {1, {{8, 8, 8}, {8, 8, 8}, {8, 8, 8}}}, 9}, {true, true, {1, {{9, 9, 9}, {9, 9, 9}, {9, 9, 9}}}, 3}, {true, false, {1, {{2, 2, 2}, {2, 2, 2}, {2, 2, 2}}}, 4}, {false, false, {1, {{8, 8, 8}, {8, 8, 8}, {8, 8, 8}}}, 3}, {false, false, {1, {{8, 8, 8}, {8, 8, 8}, {8, 8, 8}}}, 7}, {false, true, {1, {{9, 9, 9}, {9, 9, 9}, {9, 9, 9}}}, 10}, {true, true, {1, {{8, 8, 8}, {8, 8, 8}, {8, 8, 8}}}, 9}, {false, false, {1, {{4, 4, 4}, {4, 4, 4}, {4, 4, 4}}}, 7}};
const int numTests = 17;

bool testTransition(full_state_t start, full_state_t end, state_inputs_t inputs, bool verbos)
{
    const auto actual = updateFSM(start, inputs.clock, inputs.emergencyStop, inputs.hasNewConfiguration, inputs.newestConfiguration);

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