//for calibration of LDR at the start(Disabled for Final Run)
// void setBalance() {
//     //int noise;
//     Serial.println("Place White Sample for Calibration...");
//     delay(5000); // Wait for white sample

//     // Scan white sample
//     for (int i = 0; i <= 2; i++) {
//         //noise = getNoise();
//         selectLED(i);
//         delay(RGB_WAIT);
//         whiteArray[i] = getReading();
//         Serial.println(whiteArray[i]);
//         offLED();
//     }

//     Serial.println("Place Black Sample for Calibration...");
//     delay(5000); // Wait for black sample

//     // Scan black sample
//     for (int i = 0; i <= 2; i++) {
//         //noise = getNoise();
//         selectLED(i);
//         delay(RGB_WAIT);
//         blackArray[i] = getReading();
//         Serial.println(blackArray[i]);
//         greyDiff[i] = whiteArray[i] - blackArray[i];
//         offLED();
//     }
//     Serial.println("Calibration Completed. Ready for Color Detection.");
//     Serial.println("{" + String(whiteArray[0]) + ", " + String(whiteArray[1]) + ", " + String(whiteArray[2]) + "}");
//     Serial.println("{" + String(blackArray[0]) + ", " + String(blackArray[1]) + ", " + String(blackArray[2]) + "}");
//     Serial.println("{" + String(greyDiff[0]) + ", " + String(greyDiff[1]) + ", " + String(greyDiff[2]) + "}");
//     delay(5000);
// }

/*int getNoise() {
    offLED();
    return getAvgReading(5);
}*/

//LDR gets analog reading of the amount of light reflected
// int getReading() {
//     int reading = analogRead(LDR);
//     return reading;
// }

//function that turns on selected LED color
void selectLED(int color) {
    switch (color) {
        case 0: // Red
            digitalWrite(DECODER_PIN_A, HIGH);
            digitalWrite(DECODER_PIN_B, HIGH);
            break;
        case 1: // Green
            digitalWrite(DECODER_PIN_A, HIGH);
            digitalWrite(DECODER_PIN_B, LOW);
            break;
        case 2: // Blue
            digitalWrite(DECODER_PIN_A, LOW);
            digitalWrite(DECODER_PIN_B, HIGH);
            break;
    }
}

//turn off LED and on IR
void offLED() {
  digitalWrite(DECODER_PIN_A, LOW);
  digitalWrite(DECODER_PIN_B, LOW);
}

//detect color of paper and return index of color detected
int detectColor() {
    offLED();
    delay(100);
    led.setColor(255, 255, 0); // yellow for thinking
    led.show();
    analogRead(LDR);
    for (int c = 0; c <= 2; c++) {
        int previous = 0;
        selectLED(c); // on the selected LED
        delay(RGB_WAIT); // Stabilize LDR
        // Average reading for current LED color
        while (abs(analogRead(LDR) - previous) > 0) {
            previous = analogRead(LDR);
            Serial.print(LED[c]);
            Serial.print(" : ");
            Serial.println(previous);
            delay(100);
        }
        colorArray[c] = previous;
        //colorArray[c] = (colorArray[c] - blackArray[c]) / greyDiff[c] * 255;
        offLED();
        //debugging purposes
        Serial.print(blackArray[c]);
        Serial.print(whiteArray[c]);
        Serial.print(greyDiff[c]);
        Serial.print("Intensity for ");
        Serial.print(LED[c]);
        Serial.print(": ");
        Serial.println(int(colorArray[c]));
        delay(RGB_WAIT);
    }

    int minDifference = 5000;  // Arbitrary large number
    int colorIndex = -1;
   // Euclidean Distance: double squaredDetectColor = sqrt(colorArray[0]*colorArray[0] + colorArray[1]*colorArray[1] + colorArray[2]*colorArray[2]);
    
    // Find closest match using Manhattan Distance
    for (int i = 0; i < 6; i++) {
        //Euclidean Distance: double diff = fabs(squaredDetectColor - squaredColors[i]);
        int diff = abs(colorArray[0] - calibratedColors[i][0]) +
                   abs(colorArray[1] - calibratedColors[i][1]) +
                   abs(colorArray[2] - calibratedColors[i][2]);
        if (diff < minDifference) {
            minDifference = diff;
            colorIndex = i;
        }
    }
    //Index of the closest matching color
    if (colorIndex != -1) {
        Serial.print("Detected Color: ");//Debug on Serial Monitor
        Serial.println(int(colorIndex));
        Serial.println(colorNames[colorIndex]);
        led.setColor(displayLED[colorIndex][0], displayLED[colorIndex][1], displayLED[colorIndex][2]);//Visual debug
        led.show();
    }
    return colorIndex;
    //Return the index of the color detected
}

int colorVote() {
    int votes[6] = {0};
    // unsigned long start_time = millis();
    // while(millis() - start_time < 10000) {
    //     for (int i = 0; i < 3; i++) {
    //         selectLED(i);
    //         int reading = getReading();
    //     }
    // }
    detectColor();
    for (int i = 0; i < ROUNDS; i++) {
        int vote = detectColor();
        if (vote != -1) votes[vote]++;
    }
    int choice = -1;
    for (int i = 0; i < 6; i++) {
        Serial.print(votes[i]);
        if (votes[i] > choice) choice = i;
    }
    return choice;
}