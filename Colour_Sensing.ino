//function that turns on selected LED colour
void selectLED(int colour) {
    switch (colour) {
        case R: // Red
            digitalWrite(DECODER_PIN_A, HIGH);
            digitalWrite(DECODER_PIN_B, HIGH);
            break;
        case G: // Green
            digitalWrite(DECODER_PIN_A, HIGH);
            digitalWrite(DECODER_PIN_B, LOW);
            break;
        case B: // Blue
            digitalWrite(DECODER_PIN_A, LOW);
            digitalWrite(DECODER_PIN_B, HIGH);
            break;
        case IR: // IR/off LED
            digitalWrite(DECODER_PIN_A, LOW);
            digitalWrite(DECODER_PIN_B, LOW);
            break;
    }
}

int euclidean() { // First layer separator via euclidean distance
    int minDiff = 1000000;  // Arbitrarily large number
    int colour = -1; // Placeholder
    for (int i = 0; i < 6; i++) { // Find closest match using Euclidean Distance
        int diff = sqrt((colourArray[R] - calibratedColours[i][R])*(colourArray[R] - calibratedColours[i][R]) +
                   (colourArray[G] - calibratedColours[i][G])*(colourArray[G] - calibratedColours[i][G]) +
                   (colourArray[B] - calibratedColours[i][B])*(colourArray[B] - calibratedColours[i][B]));
        if (diff < minDiff) { // Choose the color that is the closest to the reading values
            minDiff = diff;
            colour = i;
        }
    }
    return colour;
}

int tieBreak(int colour) { // Second Layer separator via threshold
    // Colour pairs Orange & Red, and Pink & White
    if (colour == RED || colour == ORANGE || colour == PINK || colour == WHITE) {
        while (analogRead(LDR_PIN) > MEASURE_BASELINE) selectLED(OFF); // Discharge back to measurement baseline
        selectLED(1); // Selection of Green as most significant separating colour between the pairs
        delay(RGB_SATURATE); // Significant time for LDR to saturate to differentiating value
        int green = analogRead(LDR_PIN); // Variable green storing differentiating and testing against experimentally tested thresholds
        if (green < 515) colour = RED;
        else if (green < 660) colour = ORANGE;
        else if (green < 740) colour = PINK;
        else colour = WHITE;
    }

    // Colour pair Blue & Green
    if (colour == BLUE || colour == GREEN) {
        while (analogRead(LDR_PIN) > MEASURE_BASELINE) selectLED(OFF); // Discharge back to measurement baseline
        selectLED(2); // Selection of Blue as most significant separating colour between the pairs
        delay(RGB_SATURATE); // Significant time for LDR to saturate to differentiating value
        int blue = analogRead(LDR_PIN);// Variable blue storing differentiating and testing against experimentally tested thresholds
        if (blue < 610) colour = GREEN;
        else colour = BLUE;
    }
    return colour;
}

// Detect colour of paper and return index of colour detected
int detectColour() {
    led.setColor(255, 255, 0); // Output yellow to onboard RGB LEDs to prevent mistaking for bot failure during colour detection
    led.show();

    for (int c = 0; c < 3; c++) { // loop through LEDs and insert values into colourArray
        while (analogRead(LDR_PIN) > MEASURE_BASELINE) selectLED(OFF); // Discharge back to measurement baseline
        selectLED(c);
        delay(RGB_WAIT);
        double reading = analogRead(LDR_PIN);
        selectLED(OFF);
        colourArray[c] = reading;
    }

    int colour = euclidean(); // First Layer separation
    colour = tieBreak(colour); // Second Layer separation

    if (colour != -1) { // Output detected colour to onboard RGB LEDs for visual debug
        led.setColor(displayLED[colour][0], displayLED[colour][1], displayLED[colour][2]);
        led.show();
    }
    return colour;
}