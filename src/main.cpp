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
#define NUM_BUTTON_SETS       5


KeyCalculator calc;
TFT_eSprite   sprite          = TFT_eSprite(&M5.Lcd);
String        button_sets[]   = { BUTTONS_NORMAL_0, BUTTONS_NORMAL_1, BUTTONS_NORMAL_2, BUTTONS_NORMAL_3, BUTTONS_NORMAL_4 };
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


// Display a list of ten indexed memory values, starting at index
//
void show_indexed_memory_group(uint8_t index) {
  ezMenu menu("Indexed Memory");
  menu.txtSmall();
  menu.buttons("up # back # down");
  for(int i = 0; i < 10; i++) {
    menu.addItem(String("M[") + String(index) + "]\t" + String(calc._calc.get_memory(index)));
    index++;
  }
  menu.run();
}


// Display a menu for selecting a sub-group of indexed memories
//
void show_indexed_memory() {
  int index = 0;
  ezMenu menu("Indexed Memory");
  menu.txtSmall();
  menu.buttons("up # back # select ## down#");
  // Add ten menus, reading like: M[50] - M[59]
  for(int i = 0; i < 10; i++) {
    menu.addItem(String("M[") + index + "] - M[" + (index+9) + "]");
    index += 10;
  }
  menu.addItem("back | Back to Calculator Settings");
  while(menu.runOnce()) {
    show_indexed_memory_group((menu.pick() - 1) * 10);
  }
}


// If the memory stack is empty, show a notice. If not, show the entire stack.
//
void show_memory_stack() {
  if(0 == calc._calc.get_memory_depth()) {
    ez.msgBox("Memory Stack Empty", "There are no values in the memory stack.");
  }
  else {
    ezMenu menu("Memory Stack");
    menu.txtSmall();
    menu.buttons("up # back #  down");
    for(int i = calc._calc.get_memory_depth() - 1; i >= 0; i--) {
      menu.addItem(calc.double_to_string(calc._calc.memory_stack[i]));
    }
    menu.run();
  }
}


// Perform a few simple operations on the memory stack
//
void memory_stack_operations() {
  ezMenu menu("Memory Stack Operations");
  menu.txtSmall();
  menu.buttons("up # back # select ## down #");
  menu.addItem("Clear");
  menu.addItem("Sum");
  menu.addItem("Average");
  menu.addItem("Count");
  menu.addItem("back | Back to Calculator Settings");
  while(menu.runOnce()) {
    if(menu.pickName() == "Clear") calc._calc.clear_memory_stack();
    else if(menu.pickName() == "Sum") {
      double total = 0.0;
      for(int i = 0; i < calc._calc.memory_stack.size(); i++)
        total += calc._calc.memory_stack[i];
      calc.set_display(calc.double_to_string(total));
    }
    else if(menu.pickName() == "Average") {
      double total = 0.0;
      int    depth = calc._calc.memory_stack.size();
      for(int i = 0; i < depth; i++)
        total += calc._calc.memory_stack[i];
      total /= depth;
      calc.set_display(calc.double_to_string(total));
    }
    else if(menu.pickName() == "Count") {
      calc.set_display(calc.double_to_string(calc._calc.memory_stack.size()));
    }
  }
}


// Display a menu of miscellaneous functions
//
void menu_menu() {
  ezMenu menu("Calculator Settings");
  menu.txtSmall();
  menu.buttons("up # back # select ## down #");
  menu.addItem(String("Stacks | Display Calc Stacks\t") + (stacks_visible ? "On" : "Off"));
  menu.addItem("View Indexed Memory");
  menu.addItem("View Memory Stack");
  menu.addItem("Memory Stack Operations");
  menu.addItem("Exit | Back to Calculator");
  while(menu.runOnce()) {
    if(menu.pickName() == "Exit") return;
    else if(menu.pickName() == "Stacks") {
      if(menu.pickCaption().endsWith("On")) {
        menu.setCaption("Stacks", "Display Calc Stacks\tOff");
        stacks_visible = false;
      }
      else {
        menu.setCaption("Stacks", "Display Calc Stacks\tOn");
        stacks_visible = true;
      }
    }
    else if(menu.pickName() == "View Indexed Memory") {
      show_indexed_memory();
    }
    else if(menu.pickName() == "View Memory Stack") {
      show_memory_stack();
    }
    else if(menu.pickName() == "Memory Stack Operations") {
      memory_stack_operations();
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
      if(calc.is_building_number() && !cancel_bs) {
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
    else if(result == "M")      calc.key('M');  // In memory mode: retrieve
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
    else if(result == "push")   { calc.key('='); calc.push(); }
    else if(result == "pop")    { calc.key('=');  calc.pop(); }
    // normal 4
    else if(result == "square") calc.key('s');
    else if(result == "sqroot") calc.key('r');

    if (result != "right") {
      cancel_bs = false; // get out of cancel_bs as soon as any non-right button pressed.
    }

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
