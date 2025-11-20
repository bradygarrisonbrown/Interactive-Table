#include "shared.h"

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    testAll();
    while(true);
}

void loop() {
    xy_t m = fs.moleXy;

    long dist = stepper[m.x][m.y].distanceToGo();

    fs = updateFSM(fs,
                   numRounds,
                   readButtons(),
                   millis(),
                   dist);   
}

full_state_t updateFSM(full_state_t currState,
                       int numRounds,
                       ButtonGrid buttons,
                       unsigned long clock,
                       long distanceToGoForMole)   // NEW
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
    // INIT → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_INIT:
        if (numRounds != 0) {
            ret.currentRound = 1;
            ret.score = 0;
            ret.moleStartMs = 0;
            ret.fsmState = FsmState::s_CHOOSE_MOLE;
        }
        break;

    // ------------------------------------
    // CHOOSE_MOLE → RAISE_MOLE or GAME_OVER
    // ------------------------------------
    case FsmState::s_CHOOSE_MOLE:

        if (currentRound > numRounds) {
            displayLCD("Game Over!", ret.score);
            ret.fsmState = FsmState::s_GAME_OVER;
            break;
        }

        ret.moleXy = { random(0,3), random(0,3) };
        ret.moleStartMs = clock;

        // Trigger raising the mole now
        setHeight(ret.moleXy.x, ret.moleXy.y, TARGET_RISE_HEIGHT);

        ret.fsmState = FsmState::s_RAISE_MOLE;
        break;

    // ------------------------------------
    // RAISE_MOLE → WAIT
    // ------------------------------------
    case FsmState::s_RAISE_MOLE:
        // CHECK USING PROVIDED distanceToGoForMole
        if (distanceToGoForMole == 0) {
            ret.fsmState = FsmState::s_WAIT;
        }
        break;

    // ------------------------------------
    // WAIT → HIT, MISS, TIME_EXPIRED
    // ------------------------------------
    case FsmState::s_WAIT: {
        bool anyPress = false;
        bool correct = false;

        for (int y = 0; y < 3; y++)
            for (int x = 0; x < 3; x++)
                if (buttons[y][x]) {
                    anyPress = true;
                    if (x == moleXy.x && y == moleXy.y) correct = true;
                }

        if (anyPress && correct) {
            ret.score = score + 1;
            ret.fsmState = FsmState::s_HIT_MOLE;
        }
        else if (anyPress && !correct) {
            ret.score = score - 1;
            ret.fsmState = FsmState::s_MISS_HIT;
        }
        else if (clock - moleStartMs > moleDurationMs) {
            ret.fsmState = FsmState::s_TIME_EXPIRED;
        }
        break;
    }

    // ------------------------------------
    // HIT / MISS / TIME_EXPIRED → CLEAR_MOLE
    // ------------------------------------
    case FsmState::s_HIT_MOLE:
    case FsmState::s_MISS_HIT:
    case FsmState::s_TIME_EXPIRED:
        setHeight(moleXy.x, moleXy.y, 0);
        ret.fsmState = FsmState::s_CLEAR_MOLE;
        break;

    // ------------------------------------
    // CLEAR_MOLE → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_CLEAR_MOLE:
        // Use the provided motor query again
        if (distanceToGoForMole == 0) {
            ret.currentRound = currentRound + 1;
            ret.fsmState = FsmState::s_CHOOSE_MOLE;
        }
        break;

    case FsmState::s_GAME_OVER:
        break;

    default:
        Serial.println("Invalid state");
    }

    return ret;
}
