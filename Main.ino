// Action executed after LDR decodes the colour of the paper
void action() {
  int colour = detectColour(); // LDR number corresponds to specific colour detected
  if (colour == RED) leftTurn(90);
  else if (colour == GREEN) rightTurn(90);
  else if (colour == ORANGE) uTurn();
  else if (colour == PINK) doubleLeftTurn();
  else if (colour == BLUE) doubleRightTurn();
  else { // White
    endSong();
    permStop();
  }
}

void warmUp() { // Ensure LDR reaches saturation before starting run
  while(millis() < WARM_UP) {
    int LED = millis() % 3; // Loops through LEDs at high frequency
    selectLED(LED);

    // Output flashing lights to onboard RGB LEDs to prevent mistaking for bot failure during warmup
    float r = 255 * cos(3.1415 * 0.0008 * millis()), g = abs(255 * sin(3.1415 * 0.0008 * millis())), b = 255 * -cos(3.1415 * 0.0008 * millis());
    r = r > 0 ? r : 0;
    b = b > 0 ? b : 0;
    led.setColor(r, g, b);
    led.show();
  }
  selectLED(OFF);
  led.setColor(0, 0, 0); // LED Reset to prevent confusion during run
  led.show();
}

void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(DECODER_PIN_A, OUTPUT);
  pinMode(DECODER_PIN_B, OUTPUT);
  selectLED(OFF);
  led.setpin(13); // Set the onboard RGB LED pin
  Serial.begin(9600); // Begin serial communication
  warmUp();
}

void loop() {
  int lineDetect = lineFinder.readSensors();
  if (lineDetect == S1_IN_S2_IN) { // Senses black line
    reverse(10); // Pullback to prevent bumping with forward wall
    stop(10); // Ensure bot does not continue reversing
    action();
  }

  for (int i = 0; i < 4; i++) { // Loop through LEDs and IR Emitter
    if (i != IR) {
      selectLED(i); // Offs LEDs before getting ambient IR to prevent LEDs from interfering with readings
      ambientIR = analogRead(IR_PIN); // Get ambient IR value when emitter is off
    }
    else {
      selectLED(IR); // Turns IR on
      IRValue = ambientIR - analogRead(IR_PIN); // Get IR value without ambient IR, flipped to get positive values
    }
  }

  USDistance = ultraSensor.distanceCm(); // Get UltraSonic sensor reading

  if (USDistance < US_MIN) { // too close to the left wall
    adjustRight();
  }
  if (IRValue > IR_MIN || (USDistance < US_PATH && USDistance > US_MAX)) { // too close to the right wall
    adjustLeft();
  }
  else {
    forward();
  }
}

