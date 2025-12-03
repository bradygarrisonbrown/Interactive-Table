#include "shared.h"
#include "output_util.h"

int NUM_ROUNDS = 10;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

#ifdef TESTING
  testAll();
  while (true)
    ;
#else
    Serial.println("Testing NOT enabled");
    initializeLED();

    if (Serial.available() > 0) {
      NUM_ROUNDS = Serial.parseInt();
      Serial.print("With number of rounds: ");
      Serial.println(NUM_ROUNDS);
    }
#endif
}

void loop() {
    static full_state_t fs = {
        /* moleStartMs=*/ 0,
        /* moleDurationMs=*/ 0,
        /* moleXy=*/ {-1, -1},
        /* currentRound=*/ 0,
        /* totalRounds=*/ 0,
        /* score=*/ 0,
        /* fsmState=*/ FsmState::s_INIT,
    };

//   long dist = stepper[m.x][m.y].distanceToGo();
    long dist = 0;

    fs = updateFSM(fs,
                NUM_ROUNDS,
                readButtons(),
                dist,
                millis());

    delay(10);
}

full_state_t updateFSM(full_state_t currState,
                       int numRounds,
                       ButtonGrid buttons,
                       long moleDistanceToGo,
                       unsigned long clock
                       ) {
  const auto moleStartMs = currState.moleStartMs;
  const auto moleDurationMs = currState.moleDurationMs;
  const auto moleXy = currState.moleXy;
  const auto currentRound = currState.currentRound;
  const auto totalRounds = currState.totalRounds;
  const auto score = currState.score;
  const auto fsm_state = currState.fsmState;

  full_state_t ret = currState;

  switch (fsm_state) {
    // ------------------------------------
    // INIT → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_INIT:
      if (numRounds != 0) {
        ret.currentRound = 1;
        ret.totalRounds = numRounds;
        ret.score = 0;
        ret.moleDurationMs = 0;
        ret.moleStartMs = 0;
        ret.moleXy = {-1, -1};
        ret.fsmState = FsmState::s_CHOOSE_MOLE;
      }
      break;

    // ------------------------------------
    // CHOOSE_MOLE → RAISE_MOLE or GAME_OVER
    // ------------------------------------
    case FsmState::s_CHOOSE_MOLE:
      if (currentRound > totalRounds) {
        displayLCD("Game Over!", ret.score);
        Serial.println("Game over!");
        Serial.print("Score: ");
        Serial.println(score);
        ret.fsmState = FsmState::s_GAME_OVER;
        break;
      }

      ret.moleXy = randomMole();
      ret.moleDurationMs = Constants::DEFAULT_MOLE_DURATION;

      // Set the mole's target height
      setHeight(ret.moleXy, Constants::MOLE_RISE_HEIGHT);

      ret.fsmState = FsmState::s_RAISE_MOLE;
      break;

    // ------------------------------------
    // RAISE_MOLE → WAIT
    // ------------------------------------
    case FsmState::s_RAISE_MOLE:
      // CHECK USING PROVIDED distanceToGoForMole
      if (moleDistanceToGo == 0) {
        ret.fsmState = FsmState::s_WAIT;
        ret.moleStartMs = clock;
      }
      break;

    // ------------------------------------
    // WAIT → HIT, MISS, TIME_EXPIRED
    // ------------------------------------
    case FsmState::s_WAIT:
      {
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
        } else if (anyPress && !correct) {
          ret.score = score - 1;
          ret.fsmState = FsmState::s_MISS_HIT;
        } else if (clock - moleStartMs > moleDurationMs) {
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
      setHeight(moleXy, 0);
      ret.fsmState = FsmState::s_CLEAR_MOLE;
      break;

    // ------------------------------------
    // CLEAR_MOLE → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_CLEAR_MOLE:
      // Use the provided motor query again
      if (moleDistanceToGo == 0) {
        ret.currentRound = currentRound + 1;
        ret.fsmState = FsmState::s_CHOOSE_MOLE;
      }
      break;

    case FsmState::s_GAME_OVER:
      break;

    default:
      Serial.println("Invalid state");
      break;
  }

  return ret;
}
