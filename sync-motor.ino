// Simple 4-motor balancing example using encoder feedback

const int motorPins[4] = {3, 5, 6, 9};      // PWM pins for motors
const int encoderPins[4] = {2, 4, 7, 8};    // Encoder input pins

volatile long encoderCounts[4] = {0, 0, 0, 0};

int baseSpeed = 150;   // Base PWM value (0–255)
float Kp = 0.5;        // Proportional gain for correction

// Interrupt service routines for encoders
void count0() { encoderCounts[0]++; }
void count1() { encoderCounts[1]++; }
void count2() { encoderCounts[2]++; }
void count3() { encoderCounts[3]++; }

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
    pinMode(encoderPins[i], INPUT_PULLUP);
  }

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(encoderPins[0]), count0, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPins[1]), count1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPins[2]), count2, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPins[3]), count3, RISING);
}

void loop() {
  static long lastCounts[4] = {0, 0, 0, 0};
  long speeds[4];

  // Calculate speed as change in encoder counts
  for (int i = 0; i < 4; i++) {
    speeds[i] = encoderCounts[i] - lastCounts[i];
    lastCounts[i] = encoderCounts[i];
  }

  // Compute average speed
  long avgSpeed = 0;
  for (int i = 0; i < 4; i++) {
    avgSpeed += speeds[i];
  }
  avgSpeed /= 4;

  // Adjust each motor
  for (int i = 0; i < 4; i++) {
    long error = avgSpeed - speeds[i];
    int correction = Kp * error;

    int pwm = baseSpeed + correction;
    pwm = constrain(pwm, 0, 255);

    analogWrite(motorPins[i], pwm);
  }

  // Debug output
  Serial.print("Speeds: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(speeds[i]);
    Serial.print(" ");
  }
  Serial.println();

  delay(100); // Control loop delay
}
