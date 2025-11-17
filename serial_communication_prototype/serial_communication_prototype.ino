const int kBaud = 9600;
String INPUT_STRING = "";      // a String to hold incoming data
bool STRING_COMPLETE = false;  // whether the string is complete

void setup() {
  // put your setup code here, to run once:
  Serial.begin(kBaud);

  INPUT_STRING.reserve(200);
}

void loop() {
  Serial.print("hi from the main loop: ");
  Serial.println(millis());

  if (STRING_COMPLETE) {
    Serial.println(INPUT_STRING);
    INPUT_STRING = "";
    STRING_COMPLETE = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
  
  james: i believe this should be async and we can use the bool as a flag in our main
  event loop
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    INPUT_STRING += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      STRING_COMPLETE = true;
    }
  }
}
