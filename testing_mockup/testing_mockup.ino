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

full_state_t updateFSM(full_state_t currState,
                       int numRounds,
                       ButtonGrid buttons,
                       unsigned long clock)
{
    const auto moleStartMs = currState.moleStartMs;
    const auto moleDurationMs = currState.moleDurationMs;
    const auto moleXy = currState.moleXy;
    const auto currentRound = currState.currentRound;
    const auto score = currState.score;
    const auto fsm_state = currState.fsmState;

    full_state_t ret = currState;

    switch (fsm_state)
    {
    // ------------------------------------
    // 1. INIT → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_INIT:
        if (numRounds != 0) {   // t1-2
            ret.currentRound = 1;
            ret.score = 0;
            ret.moleDurationMs = currState.moleDurationMs; // already constant
            ret.moleStartMs = 0;
            ret.fsmState = FsmState::s_CHOOSE_MOLE;
        }
        break;

    // ------------------------------------
    // 2. CHOOSE_MOLE → RAISE_MOLE or GAME_OVER
    // ------------------------------------
    case FsmState::s_CHOOSE_MOLE:

        // check for game over first (t2-9)
        if (currentRound > numRounds) {
            displayLCD("Game Over!", ret.score);
            ret.fsmState = FsmState::s_GAME_OVER;
            break;
        }

        // Otherwise choose mole (t2-3)
        ret.moleXy = { random(0,3), random(0,3) }; 
        ret.moleStartMs = clock;
        // set motor to raise target
        setHeight(ret.moleXy.x, ret.moleXy.y, /* target height */);

        ret.fsmState = FsmState::s_RAISE_MOLE;
        break;

    // ------------------------------------
    // 3. RAISE_MOLE → WAIT
    // ------------------------------------
    case FsmState::s_RAISE_MOLE:
        // Wait until motor reaches height
        // Example: if (stepper[moleXy.x][moleXy.y].distanceToGo() == 0)
        if (distanceToGo(moleXy) == 0) {
            ret.fsmState = FsmState::s_WAIT;
        }
        break;

    // ------------------------------------
    // 4. WAIT → HIT, MISS, or TIME_EXPIRED
    // ------------------------------------
    case FsmState::s_WAIT: {
        bool anyButtonPressed = false;
        bool hitCorrect = false;

        // scan grid
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                if (buttons[y][x]) {
                    anyButtonPressed = true;
                    if (x == moleXy.x && y == moleXy.y) hitCorrect = true;
                }
            }
        }

        // (t4-5) HIT
        if (anyButtonPressed && hitCorrect) {
            ret.score = score + 1;
            ret.fsmState = FsmState::s_HIT_MOLE;
        }
        // (t4-6) MISS
        else if (anyButtonPressed && !hitCorrect) {
            ret.score = score - 1;
            ret.fsmState = FsmState::s_MISS_HIT;
        }
        // (t4-7) TIME_EXPIRED
        else if (clock - moleStartMs > moleDurationMs) {
            ret.fsmState = FsmState::s_TIME_EXPIRED;
        }

        break;
    }

    // ------------------------------------
    // 5, 6, 7 → CLEAR_MOLE
    // ------------------------------------
    case FsmState::s_HIT_MOLE:
    case FsmState::s_MISS_HIT:
    case FsmState::s_TIME_EXPIRED:
        // all automatically transition to CLEAR
        setHeight(moleXy.x, moleXy.y, 0);  // send motor down
        ret.fsmState = FsmState::s_CLEAR_MOLE;
        break;

    // ------------------------------------
    // 8. CLEAR_MOLE → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_CLEAR_MOLE:
        // Wait for motor to reach bottom
        if (distanceToGo(moleXy) == 0) {
            ret.currentRound = currentRound + 1;
            ret.fsmState = FsmState::s_CHOOSE_MOLE;
        }
        break;

    // ------------------------------------
    // 9. GAME_OVER (terminal)
    // ------------------------------------
    case FsmState::s_GAME_OVER:
        // no transitions
        break;

    default:
        Serial.println("Invalid state");
        break;
    }

    return ret;
}
