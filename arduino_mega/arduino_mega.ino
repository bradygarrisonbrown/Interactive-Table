#include "gpio_stepper_manager.h"
#include "fsm_util.h"
#include "button_util.h"
#include "led_util.h"
#include "wdt.h"

GpioStepperManager gpioStepperManager = GpioStepperManager::build();

/**
 * Set up Serial, hardware, and run tests.
 * Inputs: None
 * Outputs: None
 * Side Effects: Initialize hardware
 */
void setup() {
  Serial.begin(9600);

#ifdef TESTING
  testAll();
  while (true)
    ;
#else
  gpioStepperManager.initialize();
  initializeButtons();
  initializeLED();
    /**
  for(int i = 1; i<=3; i++) {
    for(int j = 1; j<=3; j++) {
      setStripColor(i, j, NamedColors::COLOR_BLACK);
      setGridColor(i-1, j-1, NamedColors::COLOR_BLACK);
    }
  }
  */
  fillStrip(COLOR_BLACK);
  fillGrid(COLOR_BLACK);

#ifdef ENABLE_WDT
  initWDT();
#endif



  randomSeed(analogRead(A0));

#ifdef NEED_CALIBRATION
  gpioStepperManager.printCalibrationWelcome();
#endif
#endif
}

#ifdef NEED_CALIBRATION
bool inCalibration = true;
#else
bool inCalibration = false;
#endif

bool welcomeMessagePrinted = false;
bool emergencyStop = false;

int NUM_ROUNDS = -1;

/**
 * Run main FSM loop.
 * Inputs: None
 * Outputs: Motor heights, LEDs, Serial
 * Side Effects: Modify hardware fields
 */
void loop() {
  // Use this to prove that the WDT is working
  // delay(10000);

  if (emergencyStop) {
    Serial.println("EMERGENCY STOP!");
    fillGrid(COLOR_RED);
    fillStrip(COLOR_RED);
    while (true);
  }

  const auto buttons = readButtons();
  // Buttons test
  // for (int y = 0; y < Constants::HEIGHT; ++y) {
  //   for (int x = 0; x < Constants::WIDTH; ++x) {
  //     if (buttons.buttons[y][x]) {
  //       Serial.print("{x: ");
  //       Serial.print(x);
  //       Serial.print(" y: ");
  //       Serial.print(y);
  //       Serial.println("}");
  //     }
  //   }
  // }
  // return;

  // LED test
  // for(int i = 1; i<=3; i++){
  //   for(int j = 1; j<=3; j++){
  //     setStripColor(i, j, NamedColors::COLOR_MAGENTA);
  //     setGridColor(i-1, j-1, NamedColors::COLOR_MAGENTA);
  //     delay(500);
  //   }
  // }
  // for(int i = 1; i<=3; i++){
  //   for(int j = 1; j<=3; j++){
  //     setStripColor(i, j, NamedColors::COLOR_BLACK);
  //     setGridColor(i-1, j-1, NamedColors::COLOR_BLACK);
  //   }
  //   delay(500);
  // }
  // return;
  

  if (inCalibration) {
    const auto calibrationDone = gpioStepperManager.calibrationStep();
    if (calibrationDone) {
      Serial.println("###################### CALIBRATION COMPLETE ######################");
      inCalibration = false;
    }
    return;
  }

  if (!welcomeMessagePrinted) {
    Serial.println("Enter a number of rounds:");
    welcomeMessagePrinted = true;
  }

  if (Serial.available() > 0 && NUM_ROUNDS == -1) {
    NUM_ROUNDS = Serial.parseInt();
    if (NUM_ROUNDS <= 0) {
      Serial.println("Please enter non-zero rounds.");
      NUM_ROUNDS = -1;

      Serial.read(); // expect this to be a \n
    } else {
      Serial.print("Starting with number of rounds: ");
      Serial.println(NUM_ROUNDS);
    }
  }

  if (NUM_ROUNDS == -1) {
    // We don't want to do anything until the user says how many rounds
    rainbowFade(1, 1);
    return;
  }

  static full_state_t fs = {
      /* moleStartMs=*/ 0,
      /* moleDurationMs=*/ 0,
      /* moleXy=*/ {-1, -1},
      /* currentRound=*/ 0,
      /* totalRounds=*/ 0,
      /* score=*/ 0,
      /* fsmState=*/ FsmState::s_INIT,
  };

  // const auto buttons = readButtons();

  long dist = gpioStepperManager.distanceToGo(fs.moleXy);
  // long dist = 0;

  fs = updateFSM(fs,
              NUM_ROUNDS,
              buttons,
              dist,
              millis());
}

/**
 * Run one iteration of the time-triggered FSM.
 * Inputs: See function signature
 * Outputs: The next state
 * Side Effects: May affect motor height, LED array, or Serial outpu, or Serial output.
 */
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
      if (numRounds != 0) { // t 1-2
        ret.currentRound = 1;
        ret.totalRounds = numRounds;
        ret.score = 0;
        ret.moleDurationMs = 0;
        ret.moleStartMs = 0;
        ret.moleXy = {-1, -1};
        ret.fsmState = FsmState::s_CHOOSE_MOLE;
      }
      break; // s_INIT

    // ------------------------------------
    // CHOOSE_MOLE → RAISE_MOLE or GAME_OVER
    // ------------------------------------
    case FsmState::s_CHOOSE_MOLE:
      if (currentRound > totalRounds) { // t 2-9
        Serial.println("Game over!");
        Serial.print("Score: ");
        Serial.println(score);
        ret.fsmState = FsmState::s_GAME_OVER;
        break;
      }

      // t 2-3
      setPWM(0, totalRounds, currentRound);
      ret.moleXy = randomMole();
      ret.moleDurationMs = Constants::DEFAULT_MOLE_DURATION;

      // Set the mole's target height
      gpioStepperManager.setHeight(ret.moleXy, Constants::MOLE_RISE_HEIGHT);

      ret.fsmState = FsmState::s_RAISE_MOLE;
      break; // s_CHOOSE_MOLE

    // ------------------------------------
    // RAISE_MOLE → WAIT
    // ------------------------------------
    case FsmState::s_RAISE_MOLE: {
      bool anyPress = false;
      for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
          if (buttons.buttons[y][x]) {
            anyPress = true;
          }


      if (moleDistanceToGo == 0) { // t 3-4
        ret.fsmState = FsmState::s_WAIT;
        ret.moleStartMs = clock;
        setGridColor(ret.moleXy.y, ret.moleXy.x, MoleColors::RAISED);
        setStripColor(ret.moleXy.y + 1, ret.moleXy.x + 1, MoleColors::RAISED);
      } else if (anyPress) { 
        setGridColor(ret.moleXy.y, ret.moleXy.x, MoleColors::MISSED);
        setStripColor(ret.moleXy.y + 1, ret.moleXy.x + 1, MoleColors::MISSED);
        gpioStepperManager.setHeight(ret.moleXy, 0);
        ret.fsmState = FsmState::s_CLEAR_MOLE;
      } else { // t 3-3
        gpioStepperManager.step();
      }
    } break; // s_RAISE_MOLE

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

        if (anyPress && correct) { // t 4-5
          ret.score = score + 1;
          ret.fsmState = FsmState::s_HIT_MOLE;
          Serial.println("HIT");
          setGridColor(ret.moleXy.y, ret.moleXy.x, MoleColors::HIT);
          setStripColor(ret.moleXy.y + 1, ret.moleXy.x + 1, MoleColors::HIT);
        } else if (anyPress && !correct) { // t 4-6
          ret.score = score - 1;
          ret.fsmState = FsmState::s_MISS_HIT;
          Serial.println("MISS");
          setGridColor(ret.moleXy.y, ret.moleXy.x, MoleColors::MISSED);
          setStripColor(ret.moleXy.y + 1, ret.moleXy.x + 1, MoleColors::MISSED);
        } else if (clock - moleStartMs > moleDurationMs) { // t 4-7
          ret.fsmState = FsmState::s_TIME_EXPIRED;
          Serial.println("EXPIRED");
          setGridColor(ret.moleXy.y, ret.moleXy.x, MoleColors::EXPIRED);
          setStripColor(ret.moleXy.y + 1, ret.moleXy.x + 1, MoleColors::EXPIRED);
        }
        break; // s_WAIT
      }

    // ------------------------------------
    // HIT / MISS / TIME_EXPIRED → CLEAR_MOLE
    // ------------------------------------
    case FsmState::s_HIT_MOLE:
    case FsmState::s_MISS_HIT:
    case FsmState::s_TIME_EXPIRED: {
      // t {5, 6, 7}-8
      gpioStepperManager.setHeight(moleXy, 0);
      ret.fsmState = FsmState::s_CLEAR_MOLE;
      break;
    }

    // ------------------------------------
    // CLEAR_MOLE → CHOOSE_MOLE
    // ------------------------------------
    case FsmState::s_CLEAR_MOLE: {
      if (moleDistanceToGo == 0) { // t 8-2
        ret.currentRound = currentRound + 1;
        ret.fsmState = FsmState::s_CHOOSE_MOLE;
        setGridColor(ret.moleXy.y, ret.moleXy.x, MoleColors::CLEAR);
        setStripColor(ret.moleXy.y + 1, ret.moleXy.x + 1, MoleColors::CLEAR);
      } else { // t 8-8
        gpioStepperManager.step();
      }
      break; // s_CLEAR_MOLE
    }

    case FsmState::s_GAME_OVER: {
      break; // s_GAME_OVER
    }

    default:
      Serial.println("Invalid state");
      break; // default
  }


#ifdef ENABLE_WDT
  petWDT();
#endif

  return ret;
}
