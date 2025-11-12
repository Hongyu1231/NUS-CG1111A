#include <MeMCore.h> 

// Define number of readings or rounds
//#define READINGS 3
#define ROUNDS 3

// Define time delays for LDR, values are used in LDR.ino
#define RGB_WAIT 1000 // in ms

// Define IR and USS turn threshold
#define IR_MIN 90
#define US_MIN 8
#define US_MAX 10
#define US_PATH 16

// Define time delays for Motor, values are used in motor.ino
#define FORWARD_DELAY 250 // in ms
#define TURN_DELAY_right 250 // in ms
#define TURN_DELAY_left 250 // in ms
#define ONEBOX_DELAY 700 // in ms
#define ADJUST_DELAY 150 // in ms

// Define pins
#define DECODER_PIN_A A0  // Decoder control pin A
#define DECODER_PIN_B A1 // Decoder control pin B
#define IR A2 // IR input PIN at A2
#define LDR A3 // LDR sensor pin at A3

// Define some global variables, variables are used inside the main loop
int ir_value;
float ultra_distance;
int threshold_ir;

// Define motor speeds for moving forward and adjusting
uint8_t SPEED = 255;
uint8_t ADJUST_SPEED = 200;

// Default mBot pin declaration
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
MeRGBLed led(0, 30); // set up LED pin
MeLineFollower lineFinder(PORT_1); // assigning Line Follower to port 1
MeUltrasonicSensor ultraSensor(PORT_2); // assigning UR to port 2
MeBuzzer buzzer;

// Placeholders for color detection (Final Test Run values)
float colorArray[] = {0, 0, 0}; // Detected color intensities
float whiteArray[] = {115, 115, 115}; 
float blackArray[] = {30, 30, 35}; 
float greyDiff[] = {85, 85, 80}; 

// check calibration on the test day (Final Test Run values)
// Manhattan Distance
int calibratedColors[6][3] = { 
  {105, 55, 55}, // Red
  {45, 75, 58}, // Green
  {105, 64, 50}, // Orange
  {115, 100, 100}, // Pink
  {45, 75, 100}, // Blue
  {100, 115, 115}, // White
};

// Display colors for RGB LED, for debugging purposes
int displayLED[6][3] = {
  {255, 0, 0}, // Red
  {0, 255, 0}, // Green
  {255, 50, 0}, // Orange
  {230, 95, 215}, // Pink
  {0, 0, 255}, // Blue 
  {255, 255, 255}, // White
};

// Color names for serial output for debug
char* colorNames[] = {"Red", "Green", "Orange", "Pink", "Blue", "White"};
char* LED[] = {"Red", "Green", "Blue"};

// Ambient IR value
/*void ambientIR() {
  ir_value = 0;
  digitalWrite(DECODER_PIN_A, HIGH);
  ir_value -= analogRead(IR);
  digitalWrite(DECODER_PIN_A, LOW);
}*/

// Action executed after LDR decodes the color of the paper, function inside motor.ino
void action() {
  int color;
  color = detectColor();
  delay(20); // Each LDR number corresponds to a specific color detected.
  if (color == 0) { // Red
    turnLeft(150);
  } else if (color == 1) { // Green
    turnRight(150);
  } else if (color == 2) { // Orange 
    uTurn();
  } else if (color == 3) { // Pink 
    twoLeftTurn();
  } else if (color == 4) { // Blue
    twoRightTurn();
  }
   else { // White
    celebrate();
    stopIndef();
  } 
}

void celebrate() {
 // Star Wars Main Theme (simplified)
  led.setColor(displayLED[0][0], displayLED[0][1], displayLED[0][2]);
  led.show();
  // Note: G
  buzzer.tone(392, 500);  // G4 for 500 ms
  delay(100);
  led.setColor(displayLED[0][0], displayLED[0][1], displayLED[0][2]);
  led.show();
  
  // Note: G
  buzzer.tone(392, 500);  // G4 for 500 ms
  delay(100);
  led.setColor(displayLED[0][0], displayLED[0][1], displayLED[0][2]);
  led.show();  
  
  // Note: G
  buzzer.tone(392, 500);  // G4 for 500 ms
  delay(100);
  led.setColor(displayLED[0][0], displayLED[0][1], displayLED[0][2]);
  led.show();  
 
  // Note: Eb
  buzzer.tone(311, 350);  // Eb4 for 350 ms
  led.setColor(displayLED[1][0], displayLED[1][1], displayLED[1][2]);
  led.show(); 
  
  // Note: Bb
  buzzer.tone(466, 150);  // Bb4 for 150 ms
  led.setColor(displayLED[2][0], displayLED[2][1], displayLED[2][2]);
  led.show();
  
  // Note: G
  buzzer.tone(392, 500);  // G4 for 500 ms
  led.setColor(displayLED[0][0], displayLED[0][1], displayLED[0][2]);
  led.show();
  
  // Note: Eb
  buzzer.tone(311, 350);  // Eb4 for 350 ms
  led.setColor(displayLED[1][0], displayLED[1][1], displayLED[1][2]);
  led.show();
  
  // Note: Bb
  buzzer.tone(466, 150);  // Bb4 for 150 ms
  led.setColor(displayLED[2][0], displayLED[2][1], displayLED[2][2]);
  led.show();
  
  // Note: G
  buzzer.tone(392, 1000); // G4 for 1000 ms
  led.setColor(displayLED[0][0], displayLED[0][1], displayLED[0][2]);
  led.show();
  delay(1100);
}

void setup() {
  pinMode(DECODER_PIN_A, OUTPUT);
  pinMode(DECODER_PIN_B, OUTPUT);
  led.setpin(13); // Set the LED pin
  pinMode(IR, INPUT);
  Serial.begin(9600); // Begin serial communication
  //setBalance(); // Calibrate LDR with black and white (Disabled after calibration)
  while(millis() < 10000) {
    selectLED(millis() % 3);
    float r = 255 * cos(3.1415 * 0.0008 * millis()), g = abs(255 * sin(3.1415 * 0.0008 * millis())), b = 255 * -cos(3.1415 * 0.0008 * millis());
    r = r > 0 ? r : 0;
    b = b > 0 ? b : 0;
    led.setColor(r, g, b);
    led.show();
    Serial.println(analogRead(LDR));
  }
  led.setColor(0, 0, 0);
  led.show();
  threshold_ir = analogRead(IR);
}

void loop() {
  selectLED(millis() % 3);
  Serial.println(analogRead(LDR));
  ir_value = analogRead(IR);
  ultra_distance = ultraSensor.distanceCm();
  forward();
  Serial.print("ir value: " + String(ir_value / 10));
  Serial.println(" ultradistance: " + String(ultra_distance));
  if (ultra_distance < US_MIN) { // too close to the left wall
    adjustRight();
  }
  if ((threshold_ir - 6 < ir_value && ir_value < threshold_ir - 3) || (US_MAX < ultra_distance && ultra_distance < US_PATH)) { // too close to the right wall
    adjustLeft();
  }
  else {
    forward();
  }
  int sensorState = lineFinder.readSensors();
  if (sensorState == S1_IN_S2_IN) { // inside black line, LDR operates and starts action
    stop(10);
    detectColor();
  }
}

