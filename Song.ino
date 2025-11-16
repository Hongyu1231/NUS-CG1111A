// how many *pairs* (note, duration) are in the melody
const int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// duration (ms) of a whole note at this tempo
const int wholeNote = (60000 * 4) / tempo;

const int sixtyFourthNote = wholeNote / 64;

void endSong() {
  int divider = 0, noteDuration = 0;
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholeNote) / divider;
    }
    else if (divider < 0) {
      noteDuration = (wholeNote) / abs(divider);
      noteDuration *= 1.5;
    }
    buzzer.tone(melody[thisNote], noteDuration);
    float r = 255 * cos(3.1415 * 0.0008 * millis()), g = abs(255 * sin(3.1415 * 0.0008 * millis())), b = 255 * -cos(3.1415 * 0.0008 * millis());
    r = r > 0 ? r : 0;
    b = b > 0 ? b : 0;
    led.setColor(r, g, b);
    led.show();
  }
}