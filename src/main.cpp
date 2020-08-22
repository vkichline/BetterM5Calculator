#include <M5Stack.h>
#include <M5ez.h>
#include <KeyCalculator.h>

#define KEYBOARD_I2C_ADDR     0X08          // I2C address of the Calculator FACE
#define KEYBOARD_INT          5             // Data ready pin for Calculator FACE (active low)

#define SCREEN_WIDTH          320           // Horizontal screen size
#define SCREEN_H_CENTER       160           // Horizontal center of screen
#define FG_COLOR              BLACK         // Arbitrary foreground color
#define BG_COLOR              0xEF7D        // Arbitrary background color

#define NUM_TOP              36             // Top of the number display, where the sum is shown
#define NUM_HEIGHT           52             // Height of the number display
#define NUM_V_MARGIN         4              // Offset from top to top text
#define NUM_H_MARGIN         16             // Left/right margin of the number
#define NUM_FONT             6              // Preferred number font
#define NUM_FG_COLOR         FG_COLOR       // Number display foreground color
#define NUM_BG_COLOR         BG_COLOR       // Number display background color

#define MEM_TOP               88            // Top of the memory storage display
#define MEM_HEIGHT            32            // Height of the memory storage display
#define MEM_V_MARGIN          4             // Offset from top to top text
#define MEM_FONT              4
#define MEM_FG_COLOR          BLUE
#define MEM_BG_COLOR          BG_COLOR

#define BUTTONS_NORMAL        "< # ? # ^"
#define BUTTONS_MEM_MODE      "get # set # clear"


KeyCalculator calc;


// Main dispaly; show the number being entered or the current evaluation value
//
void display_value() {
  M5.Lcd.setTextFont(NUM_FONT);
  String disp_value = calc.get_display();           // Input in progress, or value
  M5.Lcd.fillRect(0, NUM_TOP, SCREEN_WIDTH, NUM_HEIGHT, NUM_BG_COLOR);
  M5.Lcd.setTextColor(NUM_FG_COLOR, NUM_BG_COLOR);  // Blank space erases background w/ background color set
  M5.Lcd.setTextDatum(TR_DATUM);                    // Print right-justified, relative to end of string
  M5.Lcd.drawString(disp_value, SCREEN_WIDTH - NUM_H_MARGIN, NUM_TOP + NUM_V_MARGIN, 6);
  M5.Lcd.setTextDatum(TL_DATUM);                    // Go back to normal text alignment
}


// Show the memory location we're building up (M, Mn, Mnn)
//
void display_memory_storage() {
  String disp_value;
  M5.Lcd.setTextFont(MEM_FONT);
  M5.Lcd.fillRect(0, MEM_TOP, SCREEN_WIDTH, MEM_HEIGHT, MEM_BG_COLOR);
  if(calc.get_mem_entry(&disp_value)) {
    M5.Lcd.setTextColor(MEM_FG_COLOR, MEM_BG_COLOR);  // Blank space erases background w/ background color set
    M5.Lcd.drawCentreString(disp_value.c_str(), SCREEN_H_CENTER, MEM_TOP + MEM_V_MARGIN, MEM_FONT);
  }
}


// Set the buttons at the bottom of the screen appropriately, depending on the mode
//
void set_buttons() {
  if(calc.get_mem_entry(nullptr)) {
    ez.buttons.show(BUTTONS_MEM_MODE);
  }
  else {
    ez.buttons.show(BUTTONS_NORMAL);
  }
}


// This is expected to get more complicated and smarter
//
void display_all() {
  display_value();
  display_memory_storage();
  set_buttons();
  ez.header.show("Calculator");                // restore the header after its been reused
  ez.yield();
}


// Someday this will be a real settings menu
//
void more_menu() {
  ezMenu menu("More Functions");
  menu.txtSmall();
  menu.addItem("Square");
  menu.addItem("Square Root");
  menu.addItem("Inverse");
  menu.addItem("PI");
  menu.addItem("e");
  menu.addItem("Show Memory");
  menu.addItem("Settings");
  menu.runOnce();
}


// Respond to the "?" button with some instructions
//
void info_screen() {
  ez.textBox("Info", "Someday this will tell you how to use this calculator.", true);
}


//  Return true and change ref to input char if key is available, otherwise return false.
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


//  Read a key from the keyboard, or a button from the M5Stack.
//
bool process_input() {
  char input;

  if(read_key(input)) {
    if(calc.key(input)) {
      display_all();
      return true;
    }
  }
  String result = ez.buttons.poll();
  if(result.length()) {
    if     (result == "get")    calc.key('M');  // In memory mode: retrieve
    else if(result == "set")    calc.key('=');  // In memory mode: st
    else if(result == "clear")  calc.key('A');  // In memory mode: clear
    else if(result == "<")      calc.key('B');  // KeyCalculator command for backspace
    else if(result == "?")      info_screen();
    else if(result == "^")      more_menu();
    display_all();
    return true;
  }
  return false;
}


void setup() {
  ez.begin();
  Wire.begin();
  pinMode(KEYBOARD_INT, INPUT_PULLUP);
  M5.Lcd.setTextSize(1);
  display_all();
}


void loop() {
  process_input();
  delay(100);
}
