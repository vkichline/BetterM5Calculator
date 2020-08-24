#include <M5Stack.h>
#include <M5ez.h>
#include <KeyCalculator.h>
#include "screen_layout.h"
#include "help_text.h"

#define BUTTONS_NUM_MODE      "BS # cancel # right"
#define BUTTONS_MEM_MODE      "get # M # set # = # clear # AC"
#define BUTTONS_NORMAL_0      "help # menu # right"
#define BUTTONS_NORMAL_1      "( # ) # right"
#define BUTTONS_NORMAL_2      "pi # e # right"
#define BUTTONS_NORMAL_3      "push # pop # right"
#define BUTTONS_NORMAL_4      "square # sqroot # right"
#define BUTTONS_NORMAL_5      "inv # fact # right"
#define NUM_BUTTON_SETS       6


KeyCalculator calc;
TFT_eSprite   sprite          = TFT_eSprite(&M5.Lcd);
String        button_sets[]   = { BUTTONS_NORMAL_0, BUTTONS_NORMAL_1, BUTTONS_NORMAL_2, BUTTONS_NORMAL_3, BUTTONS_NORMAL_4, BUTTONS_NORMAL_5 };
uint8_t       button_set      = 0;
bool          cancel_bs       = false;  // If true, override displaying the BS buttons
bool          stacks_visible  = true;   // Can be turned off in settings menu


// Display the status of memory etc(?) in small text above value
//
void display_status() {
  M5.Lcd.setTextFont(STAT_FONT);
  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.setTextColor(STAT_FG_COLOR, STAT_BG_COLOR);  // Blank space erases background w/ background color set
  M5.Lcd.fillRect(0, STAT_TOP, SCREEN_WIDTH, STAT_HEIGHT, STAT_BG_COLOR);
  M5.Lcd.drawString(calc.get_status_display(), STAT_LEFT_MARGIN, STAT_TOP, STAT_FONT);
}

// Main display; show the number being entered or the current evaluation value.
// Since the number can be wider than the display, and text wrapping always wraps to zero,
// and I want a left margin, I must use a sprite to render then number.
//
void display_value() {
  bool is_err = calc.get_error_state();
  sprite.fillSprite(NUM_BG_COLOR);
  sprite.setTextFont(NUM_FONT);
  sprite.setTextColor(is_err ? RED :NUM_FG_COLOR, NUM_BG_COLOR);  // Blank space erases background w/ background color set
  sprite.setTextWrap(true);

  String   disp_value = calc.get_display();
  uint16_t margin     = 0;
  uint16_t wid        = sprite.textWidth(disp_value);
  if(sprite.width() > wid) margin = sprite.width() - wid;
  sprite.setCursor(margin, 0);
  sprite.print(disp_value);
  sprite.pushSprite(LEFT_MARGIN, NUM_TOP);
}


// Show the memory location we're building up (M, Mn, Mnn)
// If we're in error mode, show the error instead
//
void display_memory_storage() {
  String disp_value;
  M5.Lcd.fillRect(0, MEM_TOP, SCREEN_WIDTH, MEM_HEIGHT, MEM_BG_COLOR);
  Op_Err err = calc.get_error_state();
  if(err) {
    M5.Lcd.setTextFont(MEM_FONT);
    M5.Lcd.setTextColor(RED, MEM_BG_COLOR);
    switch (err) {
      case ERROR_TOO_FEW_OPERANDS:  disp_value = "Too Few Operands";      break;
      case ERROR_UNKNOWN_OPERATOR:  disp_value = "Unknown Operator";      break;
      case ERROR_DIVIDE_BY_ZERO:    disp_value = "Divide by Zero";        break;
      case ERROR_OVERFLOW:          disp_value = "Overflow";              break;
      default:                      disp_value = "Unknown Error: " + err; break;
    }
    M5.Lcd.drawCentreString(disp_value.c_str(), SCREEN_H_CENTER, MEM_TOP + MEM_V_MARGIN, MEM_FONT);
  }
  else {
    M5.Lcd.setTextFont(MEM_FONT);
    if(calc.is_building_memory(&disp_value)) {
      M5.Lcd.setTextColor(MEM_FG_COLOR, MEM_BG_COLOR);  // Blank space erases background w/ background color set
      M5.Lcd.drawCentreString(disp_value.c_str(), SCREEN_H_CENTER, MEM_TOP + MEM_V_MARGIN, MEM_FONT);
    }
  }
}


// Show the memory location we're building up (M, Mn, Mnn)
//
void display_stacks() {
  bool   is_err    = calc.get_error_state();
  String op_stack  = calc.get_operator_stack_display();
  String val_stack = calc.get_value_stack_display();
  M5.Lcd.fillRect(0, STACK_TOP, SCREEN_WIDTH, STACK_HEIGHT,STACK_BG_COLOR);
  if(stacks_visible && (3 < op_stack.length() || 3 < val_stack.length())) {
    M5.Lcd.setTextDatum(TL_DATUM);
    M5.Lcd.setTextFont(STACK_FONT);
    M5.Lcd.setTextColor(is_err ? RED : STACK_FG_COLOR, STACK_BG_COLOR);  // Blank space erases background w/ background color set
    M5.Lcd.drawString(op_stack.c_str(), LEFT_MARGIN, STACK_TOP + STACK_V_MARGIN, STACK_FONT);
    M5.Lcd.setTextDatum(TR_DATUM);
    M5.Lcd.drawString(val_stack.c_str(), SCREEN_WIDTH - RIGHT_MARGIN, STACK_TOP + STACK_V_MARGIN, STACK_FONT);
    M5.Lcd.setTextDatum(TL_DATUM);
  }
}


// Set the buttons at the bottom of the screen appropriately, depending on the mode
//
void set_buttons() {
  bool num_mode = calc.is_building_number();
  if(!num_mode) cancel_bs = false;  // make sure this special mode gets cleared asap

  if(calc.is_building_memory(nullptr)) {
    ez.buttons.show(BUTTONS_MEM_MODE);
  }
  else if(num_mode && !cancel_bs) {
    ez.buttons.show(BUTTONS_NUM_MODE);
  }
  else {
    ez.buttons.show(button_sets[button_set]);
  }
}


// This is expected to get more complicated and smarter
//
void display_all() {
  display_value();
  display_status();
  display_memory_storage();
  set_buttons();
  display_stacks();
  ez.header.show("Calculator");   // restore the header after its been reused
  ez.yield();
}


// Available functions
//
void menu_menu() {
  ezMenu menu("Calculator Settings");
  menu.txtSmall();
  menu.buttons("up # back # select ## down #");
  menu.addItem(String("Stacks | Display Stacks\t") + (stacks_visible ? "On" : "Off"));
  menu.addItem("Show All Memory");
  menu.addItem("Exit | Back to Calculator");
  while(menu.runOnce()) {
    if(menu.pickName() == "Exit") return;
    if(menu.pickName() == "Stacks") {
      if(menu.pickCaption().endsWith("On")) {
        menu.setCaption("Stacks", "Display Stacks\tOff");
        stacks_visible = false;
      }
      else {
        menu.setCaption("Stacks", "Display Stacks\tOn");
        stacks_visible = true;
      }
    }
  }
}


// Respond to the "?" button with some instructions
//
void help_screen() {
  ez.textBox("Info", HELP_TEXT, true);
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
    if(calc.key(input) || calc.get_error_state()) {
      display_all();
      return true;
    }
  }
  String result = ez.buttons.poll();
  if(result.length()) {
    if (result == "right") {
      if(calc.is_building_number()) {
        // Special case: we're displaying the BUTTONS_NUM_MODE menu, and want to get out of it.
        cancel_bs = true;
      }
      else {
        button_set++;
        if(NUM_BUTTON_SETS <= button_set)
        button_set = 0;
      }
    }
    // memory mode
    else if(result == "get")    calc.key('M');  // In memory mode: retrieve
    else if(result == "set")    calc.key('=');  // In memory mode: st
    else if(result == "clear")  calc.key('A');  // In memory mode: clear
    if     (result == "M")      calc.key('M');  // In memory mode: retrieve
    else if(result == "=")      calc.key('=');  // In memory mode: st
    else if(result == "AC")     calc.key('A');  // In memory mode: clear
    // number entry mode
    else if(result == "BS")     calc.key('B');  // KeyCalculator command for backspace
    else if(result == "cancel") while(calc.is_building_number()) calc.key('B');  // to cancel input, we backspace until we can't anymore.
    // normal 0
    else if(result == "help")   help_screen();
    else if(result == "menu")   menu_menu();
    // normal 1
    else if(result == "(")      calc.key('(');
    else if(result == ")")      calc.key(')');
    // normal 2
    else if(result == "pi")     calc.set_display("3.14159265");
    else if(result == "e")      calc.set_display("2.71828182");
    // normal 3
    else if(result == "push")   calc.push();
    else if(result == "pop")    calc.pop();
    // normal 4
    else if(result == "square") return false;   // BUGBUG
    else if(result == "sqroot") return false;   // BUGBUG
    // normal 5
    else if(result == "inv") return false;     // BUGBUG
    else if(result == "fact") return false;    // BUGBUG

    display_all();
    return true;
  }
  return false;
}


void setup() {
  ez.begin();
  Wire.begin();
  pinMode(KEYBOARD_INT, INPUT_PULLUP);
  sprite.createSprite(SCREEN_WIDTH - LEFT_MARGIN - RIGHT_MARGIN, NUM_HEIGHT);
  M5.Lcd.setTextSize(1);
  display_all();
}


void loop() {
  process_input();
  delay(100);
}
