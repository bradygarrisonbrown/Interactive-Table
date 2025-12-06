#include "shared.h"
#include "output_util.h"
#include "button_util.h"

#include <Adafruit_MCP23X17.h>

int NUM_ROUNDS = -1;

bool E_STOP = false;

StepperManager STEPPER_MANAGER;
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;
Adafruit_MCP23X17 mcp3;

//Emergency Stop Pin
constexpr int EMERGENCY_STOP = 3;


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

    // Help I2C keep up (I got this from asking LLM, says it should be safe)
    Wire.setClock(400000);
    if (!mcp1.begin_I2C()) {
      Serial.println("Error starting mcp1");
    }

    if (!mcp2.begin_I2C(0x20 + 1)) {
      Serial.println("Error starting mcp2");
    }

    if (!mcp3.begin_I2C(0x20 + 2)) {
      Serial.println("Error starting mcp3");
    }
    STEPPER_MANAGER.initialize(&mcp1, &mcp2, &mcp3);
    initializeButtons();
    attachInterrupt(digitalPinToInterrupt(EMERGENCY_STOP), ISR, RISING);

    Serial.println("Enter number of rounds:");
#endif
}

  //Emergency stop interrupt function
void ISR(){
  //serial.println("EMERGENCY STOP!")
  E_STOP = false;
}

void loop() {
  // const auto btns = readButtons();
  // for (int y = 0; y < Constants::HEIGHT; ++y) {
  //   for (int x = 0; x < Constants::WIDTH; ++x) {
  //     if (btns[y][x]) {
  //       Serial.print(x);
  //       Serial.print(", ");
  //       Serial.println(y);
  //     }
  //   }
  // }

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
      return;
    }

    //If ISR in button_util sets emergency stop to false, enter infinite loop to stop running motors
    if (!E_STOP){
      Serial.println("EMERGENCY STOP!");
      while(true){}
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

//   long dist = stepper[m.x][m.y].distanceToGo();
    long dist = STEPPER_MANAGER.distanceToGo(fs.moleXy);

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
      STEPPER_MANAGER.setHeight(ret.moleXy, Constants::MOLE_RISE_HEIGHT);

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
        STEPPER_MANAGER.step();
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
      STEPPER_MANAGER.setHeight(moleXy, 0);
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
        STEPPER_MANAGER.step();
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
