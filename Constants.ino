#include <MeMCore.h>

// Define UltraSonic & IR thresholds
#define US_MIN 9 // Minimum distance of US to left wall
#define US_MAX 14 // Maximum distance of US to left wall
#define US_PATH 16 // Theoretical maximum distance of US to left if MBot was attached to right wall
#define US_UTURN 11 // Minimum distance to allow UTurning to the left
#define IR_MIN 23 // Limit for how close IR can be to right wall

// Define time delays in ms
#define COOLDOWN_DELAY 150 // Double turn intermediate cooldown movement delay
#define TURN_DELAY 275 // 90 degree turn angle delay
#define RIGHT_DOUBLE_DELAY 765 // Determines distance moved for double left turn
#define LEFT_DOUBLE_DELAY 740 // Determines distance moved for double left turn
#define RIGHT_ADJUST_DELAY 150 // Determines amount of time spent adjusting to the right
#define LEFT_ADJUST_DELAY 100 // Determines amount of time spent adjusting to the left
#define RGB_WAIT 50 // Length of time to on each RGB on for first layer reading
#define RGB_SATURATE 500 // Length of time to on RGB to saturate LDR
#define MEASURE_BASELINE 500 // Length of time to allow LDR to cooldown
#define WARM_UP 3000 // Length of time for warm up

// Define pins
#define DECODER_PIN_A A0  // Decoder control pin A
#define DECODER_PIN_B A1 // Decoder control pin B
#define IR_PIN A2 // IR input pin at A2
#define LDR_PIN A3 // LDR sensor pin at A3

// Define LED indices
#define R 0
#define G 1
#define B 2
#define OFF 3
#define IR 3 // Equivalent to OFF as IR is on when LED is off from 2-4 converter; defined for readability

// Define Colour indices
#define RED 0
#define GREEN 1
#define ORANGE 2
#define PINK 3
#define BLUE 4
#define WHITE 5

// Define global variables
int IRValue, ambientIR; // Integers for instantaneous ambient and emitted IR values
float USDistance; // Float for instanaeous UltraSonic distance

// Define motor speeds for movement
uint8_t FW_SPEED = 240; // Forward
uint8_t TRN_SPEED = 255; // Turning
uint8_t ADJUST_SPEED = 180; // Adjusting

// Default mBot pin declaration
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
MeRGBLed led(0, 30); // Set up LED pin
MeLineFollower lineFinder(PORT_1); // Assigning Line Follower to port 1
MeUltrasonicSensor ultraSensor(PORT_2); // Assigning UR to port 2
MeBuzzer buzzer;

// Detected colour values during run
double colourArray[3];

// Pre-run calibrated colour values
int calibratedColours[6][3] = { 
  {710, 495, 430}, // Red
  {549, 511, 412}, // Green
  {700, 500, 435}, // Orange
  {735, 650, 650}, // Pink
  {567, 526, 561}, // Blue
  {740, 700, 700}, // White
};

// Display colours for RGB LED for debugging purposes
int displayLED[6][3] = {
  {255, 0, 0}, // Red
  {0, 255, 0}, // Green
  {255, 50, 0}, // Orange
  {230, 95, 215}, // Pink
  {0, 0, 255}, // Blue 
  {110, 165, 220}, // White
};

// For song
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

int tempo = 114;
int melody[] = {

  NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
  NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4,
  NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
  NOTE_E5, -8, NOTE_E5, -8, NOTE_D5, -4,
  NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
  NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, 8, NOTE_CS5, -32, NOTE_B4, -32, NOTE_A4, 4,
  NOTE_A4, 8, NOTE_E5, 4, NOTE_D5, 2,

  NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
  NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4,
  NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16, NOTE_A5, 4,
  NOTE_CS5, 8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, 8,

  NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
  NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 4,
  NOTE_A4, 8, NOTE_E5, 4, NOTE_D5, 4, REST, 4
};