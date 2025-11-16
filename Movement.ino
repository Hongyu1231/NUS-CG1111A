void forward() {
  leftMotor.run(-FW_SPEED); // Left wheel goes anti-clockwise
  rightMotor.run(FW_SPEED); // Right wheel goes clockwise
}

void reverse(int time) {
  leftMotor.run(FW_SPEED); // Left wheel goes clockwise
  rightMotor.run(-FW_SPEED); // Right wheel goes anti-clockwise
  delay(time); // Adjustable reverse length
}

void rightTurn(double degree) {
  leftMotor.run(-TRN_SPEED); // Left wheel goes anti-clockwise
  rightMotor.run(-TRN_SPEED); // Right wheel goes anti-clockwise
  delay((degree * TURN_DELAY) / 90.0); // Adjustable turn angle
}

void leftTurn(double degree) {
  leftMotor.run(TRN_SPEED); // Left wheel goes clockwise
  rightMotor.run(TRN_SPEED); // Right wheel goes clockwise
  delay((degree * TURN_DELAY) / 90.0); // Adjustable turn angle
}

void stop(int time) {
  leftMotor.stop();
  rightMotor.stop();
  delay(time); // Adjustable stop time
}

void adjustRight() { // Avoidance when too close to left wall
  leftMotor.run(-FW_SPEED);
  rightMotor.run(ADJUST_SPEED);
  delay(RIGHT_ADJUST_DELAY);
}

void adjustLeft() { // Avoidance when too close to right wall
  leftMotor.run(-ADJUST_SPEED);
  rightMotor.run(FW_SPEED);
  delay(LEFT_ADJUST_DELAY);
}

void uTurn() {
  if (USDistance > US_UTURN || IRValue > IR_MIN) leftTurn(180); // Determines which side has more clearance
  else rightTurn(180);
  stop(COOLDOWN_DELAY);
}

void doubleTurnForward(int time) { // Adjustable distance moved during double turn
  stop(COOLDOWN_DELAY);
  leftMotor.run(-FW_SPEED);
  rightMotor.run(FW_SPEED);
  delay(time);
  stop(COOLDOWN_DELAY);
}

void doubleLeftTurn() {
  leftTurn(100);
  doubleTurnForward(LEFT_DOUBLE_DELAY);
  leftTurn(95);
}

void doubleRightTurn() {
  rightTurn(94);
  doubleTurnForward(RIGHT_DOUBLE_DELAY);
  rightTurn(97);
}

void permStop() { // Stop permanently
  leftMotor.stop();
  rightMotor.stop();
  while(true) delay(10); // Trapping loop
}