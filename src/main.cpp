#include <M5Stack.h>
#include <KeyCalculator.h>

#define KEYBOARD_I2C_ADDR     0X08          // I2C address of the Calculator FACE
#define KEYBOARD_INT          5             // Data ready pin for Calculator FACE (active low)

#define SCREEN_WIDTH          320           // Horizontal screen size
#define SCREEN_H_CENTER       160           // Horizontal center of screen
#define FG_COLOR              LIGHTGREY     // Arbitrary foreground color
#define BG_COLOR              BLUE          // Arbitrary background color

#define ANN_TOP               0             // Top of the Annunciator, which shows calculator status
#define ANN_HEIGHT            20            // Height of the Annunciator
#define ANN_V_MARGIN          2             // Offset from top to top text
#define ANN_H_MARGIN          16            // Left/right margin for the Annunciator
#define ANN_FONT              2             // Annunciator font
#define ANN_FG_COLOR          BLACK         // Annunciator foreground color
#define ANN_BG_COLOR          DARKGREY      // Annunciator background color

#define ACC_TOP               36            // Top of the Accumulator display, where the sum is shown
#define ACC_HEIGHT            92            // Height of the Accumulator
#define ACC_V_MARGIN          4             // Offset from top to top text
#define ACC_H_MARGIN          16            // Left/right marging of the Accumulator
#define ACC_FONT_1            6             // Preferred Accumulator font
#define ACC_FONT_2            4             // Smaller Accumulator font
#define ACC_FONT_3            2             // Smallest Accumulator font
#define ACC_FG_COLOR          FG_COLOR      // Accumulator foreground color
#define ACC_BG_COLOR          BG_COLOR      // Accumulator background color

#define INFO_TOP              128           // Top of the Info area
#define INFO_HEIGHT           92            // Height of the Info area
#define INFO_H_MARGIN         16            // Left/right margins of the Info area
#define INFO_V_MARGIN         2             // Offset from top to top text
#define INFO_FONT             2             // Info font
#define INFO_FG_COLOR         FG_COLOR      // Info foreground color
#define INFO_BG_COLOR         BG_COLOR      // Info background color

#define LABEL_TOP             220           // Top of the button labels
#define LABEL_HEIGHT          22            // Height of the button labels
#define LABEL_V_MARGIN        2             // Offset from top to top text
#define LABEL_FONT            2             // Font for the button labels
#define LABEL_FG_COLOR        ANN_FG_COLOR  // Button labels foreground color
#define LABEL_BG_COLOR        ANN_BG_COLOR  // Button labels background color
#define LABEL_BTN_A_CENTER    68            // Vertical center for Button A
#define LABEL_BTN_B_CENTER    160           // Vertical center for Button B
#define LABEL_BTN_C_CENTER    252           // Vertical center for Button C


KeyCalculator calc;


void display_value() {
  M5.Lcd.setTextFont(6);
  String disp_value = calc.get_display();           // Input in progress, or value
  M5.Lcd.fillRect(0, ACC_TOP, SCREEN_WIDTH, ACC_HEIGHT, ACC_BG_COLOR);
  M5.Lcd.setTextColor(ACC_FG_COLOR, ACC_BG_COLOR);  // Blank space erases background w/ background color set
  M5.Lcd.setTextDatum(TR_DATUM);                    // Print right-justified, relative to end of string
  M5.Lcd.drawString(disp_value, SCREEN_WIDTH - ACC_H_MARGIN, ACC_TOP + ACC_V_MARGIN, 6);
  M5.Lcd.setTextDatum(TL_DATUM);                    // Go back to normal text alignment
}


////////////////////////////////////////////////////////////////////////////////
//
//  Return true and change ref to input char if key is available, otherwise returns false.
//
bool read_key(char& input) {
  if(digitalRead(KEYBOARD_INT) == LOW) {      // If a character is ready
    Wire.requestFrom(KEYBOARD_I2C_ADDR, 1);   // request 1 byte from keyboard
    if(Wire.available()) {
      char key_val = Wire.read();             // receive a byte as character
      if(0 != key_val) {
        input = key_val;
        return true;
      }
    }
  }
  return false;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Read a key from the keyboard, or a button from the M5Stack.
//
bool process_input() {
  char input;
  M5.update();
  if(M5.BtnA.wasReleased()) { return true; }
  if(M5.BtnB.wasReleased()) { return true; }
  if(M5.BtnC.wasReleased()) { return true; }

  if(read_key(input)) {
    if(calc.key(input)) {
      display_value();
    }
  }
  return false;
}


void setup() {
  M5.begin();
  Wire.begin();
  pinMode(KEYBOARD_INT, INPUT_PULLUP);
  M5.Lcd.clearDisplay(BG_COLOR);
  display_value();
}


void loop() {
  process_input();
  delay(100);
}
