#include "KeyCalculator.h"

#define CHANGE_SIGN_OPERATOR    (uint8_t('`'))
#define BACKSPACE_OPERATOR      (uint8_t('B'))
#define MAX_ARR_MEM_TO_SHOW     8
#define CALC_NUMERIC_PRECISION  8
#define DEBUG_KEY_CALCULATOR    0


KeyCalculator::KeyCalculator() : TextCalculator(CALC_NUMERIC_PRECISION) {
  _num_buffer_index = 0;
  _mem_buffer_index = 0;
  _previous_key     = 0;
  _entering_memory  = false;
}


// This is the workhorse routine for handling incoming keys from the keyboard.
// Clear (AC, the 'A' command) can be one or two keypresses
// Memory operations can be multiple keypresses
//
bool KeyCalculator::key(uint8_t code) {
  // First, see if we are in an error state. If so, only AC works!
  if(_calc.get_error_state()) {
    if(CLEAR_OPERATOR == code) {
      _calc.clear_error_state();
      return true;
    }
    return false;
  }

  // See if we just pressed the AC key two times
  if('A' == _previous_key && 'A' == code) {
    return _handle_clear(true);
  }

  // Next, see if we are entering a multi-key memory command
  if(_entering_memory) {
    return _handle_memory_command(code);
  }

  // Special case for chaining:
  // If you press 1 + =, most calculators will give you 2. The rule is:
  // When = is pressed, if _previous_key was an operator (excluding =),
  // push the current value, then proceed to push = as normal.
  // Caution: % cannot be chained.
  //
  if(EVALUATE_OPERATOR == code) {
    if(ADDITION_OPERATOR       == _previous_key || SUBTRACTION_OPERATOR == _previous_key ||
       MULTIPLICATION_OPERATOR == _previous_key || DIVISION_OPERATOR    == _previous_key) {
      // can't use push_number() here
      if(DEBUG_KEY_CALCULATOR) Serial.printf("Pushing %.4f in chaining mode\n", _calc.get_value());
      enter(value());
    }
  }

  _previous_key = code;   // Save the keystroke for processing in next call

  // Next, see if a number is being entered (with B for Backspace)
  if(('0' <= code && '9' >= code) || ('.' == code) || 'B' == code) {
    return _build_number(code);
  }

  _push_number();   // Converts the input buffer (if there's anything to convert)

  // See if its a simple operator
  if(is_operator(code)) {
    // Do not allow a CLOSE_PAREN_OPERATOR on the stack unless there's a matching OPEN_PAREN_OPERATOR
    if(CLOSE_PAREN_OPERATOR == code && 0 == _count_open_parens()) {
      if(DEBUG_KEY_CALCULATOR) Serial.println("Rejecting close paren because there is no matching open paren.");
      return false;
    }
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing operator '%c'\n", code);
    return enter(code);
  }

  // These operations require more complex handling
  switch(code) {
    case CHANGE_SIGN_OPERATOR:  return _handle_change_sign();
    case CLEAR_OPERATOR:        return _handle_clear(false);
    case MEMORY_OPERATOR:       return _handle_memory_command(code);
    default:                    return false;
  }
  return false;
}


// If we're in number input mode, return the current _num_buffer
// else, display the value
//
String KeyCalculator::get_display() {
  if(_num_buffer_index) {
    return _convert_num_buffer(false);
  }
  return value();
}


// Push val onto the value stack and clear _num_buffer_index so value() is diplayed.
// Set _previous_key to the last character supplied, to that key() processing doesn't
// add chaining or other special handling that's not appropriate.
//
void KeyCalculator::set_display(String val) {
  _num_buffer_index = 0;                // so get_display goes to value() and not to buffer
  _num_buffer[0]    = 0;                // so it's not scanned even though index is zero
  enter(val);                           // push the value onto the stack
  _previous_key = val[val.length()-1];  // Important: as if we had entered it key by key
}


// Get the memory address that's being built up if _entering_memory,
// else return an empty string.
//
bool KeyCalculator::is_building_memory(String* str) {
  if(_entering_memory) {
    if(str) {
      *str = "M[";
      switch(_mem_buffer_index) {
        case 0: *str += "__"; break;
        case 1: *str += "_"; *str += _mem_buffer; break;
        case 2: *str += _mem_buffer; break;
        default: return false;
      }
      *str += "]  (";
      *str += double_to_string(_mem_buffer_index ? _calc.get_memory(atoi(_mem_buffer)) : _calc.get_memory());
      *str += ")";
    }
    return true;
  }
  if(str) *str = "";
  return false;
}


// In numeric entry mode. Backspace can be used.
//
bool KeyCalculator::is_building_number() {
  return 0 != _num_buffer_index;
}


// Construct a string for a calculator to display showing status.
// It may be entirely empty, or as complex as:  (( M[1,2,4,6,7,18.37,81,...]  S(5)  M=3.14159265
//
String KeyCalculator::get_status_display() {
  String  str         = "";
  uint8_t paren_count = _count_open_parens();
  uint8_t arr_count   = 0;
  uint8_t stack_count = _calc.get_memory_depth();
  double  mem         = _calc.get_memory();

  // Begin by showing how many open parens there are on the stack (if any).
  // This part of the status string leads and looks like: (((
  if(paren_count) {
    for(int i = 0; i < paren_count; i++) str += "(";
    str += " ";
  }

  // Next, display info about indexed memories. Show the indexes of up to
  // eight; if there are more add ...
  // This part of the status string looks like: M[0,1,2,3]
  for(uint8_t i = 0; i < NUM_CALC_MEMORIES; i++) {
    if(0.0 != _calc.get_memory(i)) {
      if(0 == arr_count++) str += "M[";
      if(MAX_ARR_MEM_TO_SHOW > arr_count) {
        str += i;
        str += ',';
      }
      else {
        str += "...,";
        break;
      }
    }
  }
  if(arr_count) {
    str[str.length() - 1] = ']';
    str += "  ";
  }

  // Next, display info about the memory stack. If it is non-empty, add
  // S(n), where n is the number of items on the stack.
  // This part of the status string looks like: S(2)
  if(stack_count) {
    str += "S(";
    str += stack_count;
    str += ")  ";
  }

  // Finally, if simple memory is set, display its value.
  // This part of the status string looks like: M=3.14159265
  if(0.0 != mem) {
    str += "M=";
    str += double_to_string(mem);
  }
  return str;
}


// Get representation of operator stack for UI display.
// Looks like:  [ + * ]
//
String KeyCalculator::get_operator_stack_display() {
  String result = "[ ";
  for(int i = 0; i < _calc.operator_stack.size(); i++) {
    result += char(_calc.operator_stack[i]);
    result += " ";
  }
  result += "]";
  return result;
}


// Get representation of value stack for UI display.
// Looks like: [ 5 3.175 -99 ]
//
String KeyCalculator::get_value_stack_display() {
  String result = "[ ";
  for(int i = 0; i < _calc.value_stack.size(); i++) {
    result += double_to_string(_calc.value_stack[i]);
    result += " ";
  }
  result += "]";
  return result;
}


// Handle the AC key, with 1st & 2nd press actions
//
bool KeyCalculator::_handle_clear(bool all_clear) {
  if(all_clear) {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Clearing all memory");
    clear_all_memory();
    return true;
  }
  else {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Clearing value");
    return (NO_ERROR == _calc.clear());
  }
  return false;
}


// Handle +/- key (Change Sign), which is an input action, not a command.
//
bool KeyCalculator::_handle_change_sign() {
  if(DEBUG_KEY_CALCULATOR) Serial.println("change_sign");
  if(0 < _calc.value_stack.size()) {
    if(0.0 != _calc.get_value()) {
      double val = _calc.pop_value();
      if(DEBUG_KEY_CALCULATOR) Serial.printf("Changing value from %.2f to %.2f\n", val, -val);
      _calc.push_value(-val);
      return true;
    }
  }
  return false;
}


// Memory commands are complicated. There is a simple memory, a memory array, and a memory stack.
// The memory array size is established with the MemoryCalculator template parameter M and can be
// ascertained with _calc.get_mem_array_size().
// Once M is pressed, it can be followed with another M for recall, a memory operator (+-*/%=)
// to store the value in the simple memory, or (0-9) to build a memory address for the operator:
// MM   Recall simple memory      (M -> Value)
// M=   Store simple memory       (Value -> M)
// MA   Clear simple memory       (0 -> M)
// M+   Simple memory opperation  (M + Value -> M)
// M0=  Store into M[0]           (Value -> M[0])
// M9=  Store into M[9]           (Value -> M[9])
// M.   Cancel out of memory mode (N/C)
// Note that there may be 10 or thousands of memories.
//
bool KeyCalculator::_handle_memory_command(uint8_t code) {
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_handle_memory_command('%c')\n", code);
  if(!_entering_memory) {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Setting _entering_memory");
    assert(MEMORY_OPERATOR == code);
    _entering_memory = true;
    return true;
  }
  else {
    // If it's a '.', cancel _entering_memory and return true
    if('.' == code) {
      _mem_buffer_index = 0;
      _mem_buffer[0]    = '\0';
      _entering_memory  = false;
      return true;
    }
    // If it's a backspace, reduce the memory name buffer by one
    if('B' == code) {
      if(_mem_buffer_index) {
        _mem_buffer_index--;
        _mem_buffer[_mem_buffer_index] = '\0';
        return true;
      }
      return false;
    }
    // Build memory name if it's a number
    if('0' <= code && '9' >= code) {
      // Make sure the number doesn't exceed the number of memories:
      _mem_buffer[_mem_buffer_index++] = code;
      _mem_buffer[_mem_buffer_index]   = '\0';
      if(NUM_CALC_MEMORIES - 1 < atoi(_mem_buffer)) {
        // roll back
        if(DEBUG_KEY_CALCULATOR) Serial.printf("Error setting memory address: '%s' would be too large.\n", _mem_buffer);
        _mem_buffer[--_mem_buffer_index] = '\0';
        return false;
      }
      if(DEBUG_KEY_CALCULATOR) Serial.printf("Building memory address: '%s'\n", _mem_buffer);
      return true;
    }

    // If it's a second 'M', we want to recall memory from the given location
    if(MEMORY_OPERATOR == code) {
      bool result = false;
      if(0 == _mem_buffer_index) {
        if(DEBUG_KEY_CALCULATOR) Serial.println("Recalling simple memory");
        result = recall_memory();
      }
      else {
        uint8_t index = atoi(_mem_buffer);
        if(DEBUG_KEY_CALCULATOR) Serial.printf("Recalling memory M[%d]\n", index);
        result = recall_memory(index);
      }
      _mem_buffer_index               = 0;
      _mem_buffer[_mem_buffer_index]  = '\0';
      _entering_memory                = false;
      return result;
    }

    // If it's a memory operation, call the memory_operation and terminate _entering_memory
    if(is_mem_operator(code)) {
      if(0 == _mem_buffer_index) {
        if(DEBUG_KEY_CALCULATOR) Serial.printf("call memory_operation(Op_ID = '%c')\n\n", code);
        _calc.memory_operation(code);
      }
      else {
        if(DEBUG_KEY_CALCULATOR) Serial.printf("call memory_operation: Op_ID = '%c', index = '%s'\n\n", code, _mem_buffer);
        _calc.memory_operation(code, atoi(_mem_buffer));
      }
      _mem_buffer_index               = 0;
      _mem_buffer[_mem_buffer_index]  = '\0';
      _entering_memory                = false;
      return true;
    }
  }
  // Some random command? Bail out of memory mode.
  _mem_buffer_index = 0;
  _mem_buffer[0]    = '\0';
  _entering_memory  = false;
  return false;
}


// Build the display value from keystrokes
//
bool KeyCalculator::_build_number(uint8_t code) {
  if('B' == code) {
    // backspace one space
    if(_num_buffer_index) {
      _num_buffer_index--;
      _num_buffer[_num_buffer_index] = '\0';
      return true;
    }
    return false;
  }
  if(KEY_CALCULATOR_NUM_BUFFER_SIZE <= _num_buffer_index - 1) return false;
  if('.' == code) { if(nullptr != strchr(_num_buffer, '.')) { return false; } } // One dp per number
  _num_buffer[_num_buffer_index++] = code;
  _num_buffer[_num_buffer_index]   = '\0';
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_build_number: %s\n", _num_buffer);
  return true;
}


// Not inputting a number anymore, so push any number that's been accumulated to the stack
//
bool KeyCalculator::_push_number() {
  if(_num_buffer_index) {
    String str = _convert_num_buffer(true);
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing %s onto the stack\n", str.c_str());
    enter(str); // Do not return; push numver and continue processing
    return true;
  }
  return false;
}


// Convert the buffer to a String and optionally clear it
//
String KeyCalculator::_convert_num_buffer(bool clear) {
  String str(_num_buffer);
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_convert_num_buffer: %s\n", _num_buffer);
  if(clear) {
    _num_buffer_index = 0;
    _num_buffer[0]    = '\0';
  }
  return str;
}


// Return the number of OPEN_PAREN operators on the operator_stack
//
uint8_t KeyCalculator::_count_open_parens() {
  uint8_t count = 0;
  for(int i = 0; i < _calc.operator_stack.size(); i++) {
    if(OPEN_PAREN_OPERATOR == _calc.operator_stack[i]) count++;
  }
  return count;
}
