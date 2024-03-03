// Include libraries
#include <IRremote.h>

// Define pins
const int IR_PIN = 10;
const int ENABLE_PIN = 9;
const int STEP_PIN = 1;
const int DIR_PIN = 0;

// Define blind parameters
const unsigned long BLIND_LENGTH = 94000;
const int BLIND_SPEED = 500;
unsigned long blindPosition = 0;
const int BLIND_DIR = 0;

// IR receiver object
IRrecv irrecv(IR_PIN);
decode_results results;

// Function prototypes
void setup();
void loop();
void driveMotor(unsigned long value);
void moveMotor(int moveDir, unsigned int noSteps);

void setup() {
  // Start the IR receiver
  irrecv.enableIRIn();
  
  // Set pin modes
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH); // Turn off motors
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
}

void loop() {
  if (irrecv.decode()) {
    auto value = irrecv.decodedIRData.decodedRawData;
    driveMotor(value);
    irrecv.resume();
    delay(200);
  }
}

void driveMotor(unsigned long value) {
  switch (value) {
    case 3108437760: // Button + Pressed - Open Slightly
      if (blindPosition >= 6000) {
        moveMotor(0, 6000);
        blindPosition -= 6000;
      }
      break;
    case 3927310080: // Button - Pressed - Close Slightly
      if (blindPosition <= BLIND_LENGTH - 6000) {
        moveMotor(1, 6000);
        blindPosition += 6000;
      } else {
        Serial.print("Blind position to closed at: ");
        Serial.print(blindPosition);
      }
      break;
    case 2907897600: // Button 0 Pressed - Open Fully
      moveMotor(0, blindPosition);
      blindPosition = 0;
      break;
    case 3910598400: // Button 1 Pressed - Close Fully
      moveMotor(1, BLIND_LENGTH - blindPosition);
      blindPosition = BLIND_LENGTH;
      break;
    case 3860463360: // Button 2 Pressed - Blind Preset 2 (1/4 Closed)
      if (blindPosition > BLIND_LENGTH / 4) {
        moveMotor(0, blindPosition - (BLIND_LENGTH / 4));
      } else if (blindPosition < BLIND_LENGTH / 4) {
        moveMotor(1, (BLIND_LENGTH / 4) - blindPosition);
      }
      blindPosition = BLIND_LENGTH / 4;
      break;
    case 4061003520: // Button 3 Pressed - Blind Preset 3 (1/2 Closed)
      if (blindPosition > BLIND_LENGTH / 2) {
        moveMotor(0, blindPosition - (BLIND_LENGTH / 2));
      } else if (blindPosition < BLIND_LENGTH / 2) {
        moveMotor(1, (BLIND_LENGTH / 2) - blindPosition);
      }
      blindPosition = BLIND_LENGTH / 2;
      break;
    case 3175284480: // Press # to reset blind position to 0 (open)
      blindPosition = 0;
      break;
    case 3041591040: // Press * to reset blind position to max blind length (closed)
      blindPosition = BLIND_LENGTH;
      break;

    default: // Handle unrecognized IR signals
      Serial.println("Unknown IR signal");
      break;
  }
}

void moveMotor(int moveDir, unsigned int noSteps) {
  digitalWrite(ENABLE_PIN, LOW); // Turn on motors
  delay(200);
  digitalWrite(DIR_PIN, moveDir == BLIND_DIR ? HIGH : LOW);
  for (unsigned int i = 0; i < noSteps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(BLIND_SPEED);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(BLIND_SPEED);
  }
  digitalWrite(ENABLE_PIN, HIGH); // Turn off motors
}
