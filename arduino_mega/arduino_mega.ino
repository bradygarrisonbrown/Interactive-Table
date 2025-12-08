#include "gpio_stepper_manager.h"
#include "fsm_util.h"
#include "button_util.h"

GpioStepperManager gpioStepperManager = GpioStepperManager::build();

void setup() {
  Serial.begin(9600);

#ifdef TESTING
  testAll();
  while (true)
    ;
#else
  gpioStepperManager.initialize();
  gpioStepperManager.printCalibrationWelcome();

  initializeButtons();
#endif
}

#ifdef NEED_CALIBRATION
bool inCalibration = true;
#else
bool inCalibration = false;
#endif

void loop() {
  if (inCalibration) {
    const auto calibrationDone = gpioStepperManager.calibrationStep();
    if (calibrationDone) {
      Serial.println("###################### CALIBRATION COMPLETE ######################");
      inCalibration = false;
    }
    return;
  }

  const auto buttons = readButtons();
  // for (int y = 0; y < Constants::HEIGHT; ++y) {
  //   for (int x = 0; x < Constants::WIDTH; ++x) {
  //     if (buttons.buttons[y][x]) {
  //       Serial.print("{x: ");
  //       Serial.print(x);
  //       Serial.print(", y: ");
  //       Serial.print(y);
  //       Serial.println("}");
  //     }
  //   }
  // }
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
        Serial.println("Game over!");
        Serial.print("Score: ");
        Serial.println(score);
        ret.fsmState = FsmState::s_GAME_OVER;
        break;
      }

      ret.moleXy = randomMole();
      ret.moleDurationMs = Constants::DEFAULT_MOLE_DURATION;

      // Set the mole's target height
      // STEPPER_MANAGER.setHeight(ret.moleXy, Constants::MOLE_RISE_HEIGHT);
      gpioStepperManager.setHeight(ret.moleXy, Constants::MOLE_RISE_HEIGHT);

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
      } else {
        // STEPPER_MANAGER.step();
        gpioStepperManager.step();
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
            if (buttons.buttons[y][x]) {
              anyPress = true;
              if (x == moleXy.x && y == moleXy.y) correct = true;
            }

        if (anyPress && correct) {
          ret.score = score + 1;
          ret.fsmState = FsmState::s_HIT_MOLE;
          Serial.println("HIT");
        } else if (anyPress && !correct) {
          ret.score = score - 1;
          ret.fsmState = FsmState::s_MISS_HIT;
          Serial.println("MISS");
        } else if (clock - moleStartMs > moleDurationMs) {
          ret.fsmState = FsmState::s_TIME_EXPIRED;
          Serial.println("EXPIRED");
        }
        break;
      }

    // ------------------------------------
    // HIT / MISS / TIME_EXPIRED → CLEAR_MOLE
    // ------------------------------------
    case FsmState::s_HIT_MOLE:
    case FsmState::s_MISS_HIT:
    case FsmState::s_TIME_EXPIRED:
      // STEPPER_MANAGER.setHeight(moleXy, 0);
      gpioStepperManager.setHeight(moleXy, 0);
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
      } else {
        // STEPPER_MANAGER.step();
        gpioStepperManager.step();
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
