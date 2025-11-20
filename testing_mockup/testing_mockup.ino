#include "shared.h"

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    testAll();
    while(true);
}

void loop()
{
}

full_state_t updateFSM(full_state_t currState, int numRounds, ButtonGrid buttons, unsigned long clock)
{
    const auto moleStartMs = currState.moleStartMs;
    const auto moleDurationMs = currState.moleDurationMs;
    const auto moleXy = currState.moleXy;
    const auto currentRound = currState.currentRound;
    const auto score = currState.score;
    const auto fsm_state = currState.fsmState;

    auto ret = currState;

    switch (fsm_state) {
    default:
        Serial.println("Invalid state");
    }

    return ret;
}
