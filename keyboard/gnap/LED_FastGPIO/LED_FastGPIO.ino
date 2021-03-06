#include <FastGPIO.h>
#include <TimerOne.h>

int iByte;
byte col = 0;
byte leds[12][4];
byte pass = 1;
int fadecount = 1;
const int fadelimit = 3000;
byte mode = 0;
byte brightness = 9;

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[17] = {
  -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20, 21
};

// col[xx] of leds = pin yy on led matrix
int cols[12] = {
  pins[8], pins[7], pins[6], pins[5], pins[9], pins[10], pins[11], pins[12], pins[13], pins[14], pins[15], pins[16]
};

// row[xx] of leds = pin yy on led matrix
int rows[4] = {
  pins[1], pins[2], pins[3], pins[4]
};


#define DELAY 0
extern byte leds[12][4];

void setup() {
  Serial1.begin(9600);
  setupLeds();

  fadeIn();
  fadeOut();
  fadeIn();
}

void fadeIn() {
  for ( int r = 1; r  < 9; r++)  {
    delayMicroseconds(65000);
    delayMicroseconds(65000);
    for (int j = 0; j < 4; j++) {
      for (int i = 0; i < 12; i++) {
        leds[i][j] = 1;
        for (int p = 0; p < 25; p++) {
        }
        leds[i][j] = r;
      }
    }
  }
}

void fadeOut() {
  for ( int r = 9; r  > 0; r--)  {
    delayMicroseconds(65000);
    delayMicroseconds(65000);
    delayMicroseconds(65000);
    for (int j = 0; j < 4; j++) {
      for (int i = 0; i < 12; i++) {
        leds[i][j] = 1;
        for (int p = 0; p < 25; p++) {
        }
        leds[i][j] = r;
      }
    }
  }
}

void loop() {
  switch (mode) {
    case 0:
      // Backlight
      for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++) {
          leds[i][j] = brightness;
        }
      }
      break;
    case 1:
      // Reactive
      fadecount++;
      if (fadecount > fadelimit) {
        fadecount = 1;
        for (int i = 0; i < 12; i++) {
          for (int j = 0; j < 4; j++) {
            if (leds[i][j] > 0) {
              leds[i][j] = leds[i][j] - 1;
            }
          }
        }
      }
      break;
    case 2:
      // Fade
      fadecount++;
      if (fadecount > fadelimit) {
        fadecount = 1;
        for (int i = 0; i < 12; i++) {
          for (int j = 0; j < 4; j++) {
            if (leds[i][j] < 9) {
              leds[i][j] = leds[i][j] + 1;
            }
          }
        }
      }
      break;
  }

  checkserial();
}

void checkserial() {
  if (Serial1.available() > 0) {
    iByte = Serial1.read();
    if (iByte == 100) {
      switch (brightness) {
        case 1:
          brightness = 2;
          break;
        case 2:
          brightness = 5;
          break;
        case 5:
          brightness = 9;
          break;
        case 9:
          brightness = 1;
          break;
      }
    }
 
    if (iByte == 101) {
      switch (mode) {
        case 0: // backlight
          fadeOut();
          mode = 1; 
          break;
        case 1: // reactive
          fadeIn();
          brightness = 9;
          mode = 2;
          break;
        case 2: // fade
          fadeOut();
          fadeIn();
          brightness = 9;
          mode = 0;
          break;
      }
    }
    
    if (iByte < 100) {
      byte row = iByte / 16;
      byte col = iByte % 16;
      switch (mode) {
        case 1: // reactive
          leds[col][row] = 18;
          break;
        case 2:
          clearLeds();
          break;
      }
    }
  }
}

void setupLeds() {
  // sets the pins as output
  FastGPIO::Pin<2>::setOutputLow();
  FastGPIO::Pin<3>::setOutputLow();
  FastGPIO::Pin<4>::setOutputLow();
  FastGPIO::Pin<5>::setOutputLow();
  FastGPIO::Pin<6>::setOutputLow();
  FastGPIO::Pin<7>::setOutputLow();
  FastGPIO::Pin<8>::setOutputLow();
  FastGPIO::Pin<9>::setOutputLow();
  FastGPIO::Pin<10>::setOutputLow();
  FastGPIO::Pin<16>::setOutputLow();
  FastGPIO::Pin<14>::setOutputLow();
  FastGPIO::Pin<15>::setOutputLow();
  FastGPIO::Pin<18>::setOutputLow();
  FastGPIO::Pin<19>::setOutputLow();
  FastGPIO::Pin<20>::setOutputLow();
  FastGPIO::Pin<21>::setOutputLow();

  // set up Cols
  FastGPIO::Pin<6>::setOutputValueLow();
  FastGPIO::Pin<7>::setOutputValueLow();
  FastGPIO::Pin<8>::setOutputValueLow();
  FastGPIO::Pin<9>::setOutputValueLow();
  FastGPIO::Pin<10>::setOutputValueLow();
  FastGPIO::Pin<16>::setOutputValueLow();
  FastGPIO::Pin<14>::setOutputValueLow();
  FastGPIO::Pin<15>::setOutputValueLow();
  FastGPIO::Pin<18>::setOutputValueLow();
  FastGPIO::Pin<19>::setOutputValueLow();
  FastGPIO::Pin<20>::setOutputValueLow();
  FastGPIO::Pin<21>::setOutputValueLow();

  // set up Rows
  FastGPIO::Pin<2>::setOutputValueLow();
  FastGPIO::Pin<3>::setOutputValueLow();
  FastGPIO::Pin<4>::setOutputValueLow();
  FastGPIO::Pin<5>::setOutputValueLow();

  clearLeds();
  Timer1.initialize(25);
  Timer1.attachInterrupt(display);
}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 4; j++) {
      leds[i][j] = 0;
    }
  }
}

// Interrupt routine
void display() {
  switch (col) { // Turn whole previous column off
    case 0:
      FastGPIO::Pin<6>::setOutputValueLow();
      break;
    case 1:
      FastGPIO::Pin<7>::setOutputValueLow();
      break;
    case 2:
      FastGPIO::Pin<8>::setOutputValueLow();
      break;
    case 3:
      FastGPIO::Pin<9>::setOutputValueLow();
      break;
    case 4:
      FastGPIO::Pin<10>::setOutputValueLow();
      break;
    case 5:
      FastGPIO::Pin<16>::setOutputValueLow();
      break;
    case 6:
      FastGPIO::Pin<14>::setOutputValueLow();
      break;
    case 7:
      FastGPIO::Pin<15>::setOutputValueLow();
      break;
    case 8:
      FastGPIO::Pin<18>::setOutputValueLow();
      break;
    case 9:
      FastGPIO::Pin<19>::setOutputValueLow();
      break;
    case 10:
      FastGPIO::Pin<20>::setOutputValueLow();
      break;
    case 11:
      FastGPIO::Pin<21>::setOutputValueLow();
      break;
  }

  col++;
  if (col == 12) {
    col = 0;
    pass++;
    if (pass > 8) {
      pass = 1;
    }
  }
  
  for (int row = 0; row < 4; row++) {
    if (leds[col][row] > pass) {
      switch (row) { // Turn on this led
        case 0:
          FastGPIO::Pin<2>::setOutputValueLow();
          break;
        case 1:
          FastGPIO::Pin<3>::setOutputValueLow();
          break;
        case 2:
          FastGPIO::Pin<4>::setOutputValueLow();
          break;
        case 3:
          FastGPIO::Pin<5>::setOutputValueLow();
          break;
      }
    }
    else {
      switch (row) { // Turn off this led
        case 0:
          FastGPIO::Pin<2>::setOutputValueHigh();
          break;
        case 1:
          FastGPIO::Pin<3>::setOutputValueHigh();
          break;
        case 2:
          FastGPIO::Pin<4>::setOutputValueHigh();
          break;
        case 3:
          FastGPIO::Pin<5>::setOutputValueHigh();
          break;
      }
    }
  }
  
  switch (col) { // Turn column on
    case 0:
      FastGPIO::Pin<6>::setOutputValueHigh();
      break;
    case 1:
      FastGPIO::Pin<7>::setOutputValueHigh();
      break;
    case 2:
      FastGPIO::Pin<8>::setOutputValueHigh();
      break;
    case 3:
      FastGPIO::Pin<9>::setOutputValueHigh();
      break;
    case 4:
      FastGPIO::Pin<10>::setOutputValueHigh();
      break;
    case 5:
      FastGPIO::Pin<16>::setOutputValueHigh();
      break;
    case 6:
      FastGPIO::Pin<14>::setOutputValueHigh();
      break;
    case 7:
      FastGPIO::Pin<15>::setOutputValueHigh();
      break;
    case 8:
      FastGPIO::Pin<18>::setOutputValueHigh();
      break;
    case 9:
      FastGPIO::Pin<19>::setOutputValueHigh();
      break;
    case 10:
      FastGPIO::Pin<20>::setOutputValueHigh();
      break;
    case 11:
      FastGPIO::Pin<21>::setOutputValueHigh();
      break;
  }
}

