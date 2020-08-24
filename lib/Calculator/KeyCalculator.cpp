#include "KeyCalculator.h"

#define DEBUG_KEY_CALCULATOR    0
#define CHANGESIGN_OPERATOR     (uint8_t('`'))
#define MEMORY_OPERATOR         (uint8_t('M'))
#define BACKSPACE_OPERATOR      (uint8_t('B'))
#define MAX_ARR_MEM_TO_SHOW     8
#define CALC_NUMERIC_PRECISION  8


KeyCalculator::KeyCalculator() : TextCalculator(CALC_NUMERIC_PRECISION) {
  _num_buffer_index = 0;
  _mem_buffer_index = 0;
  _last_key         = 0;
  _entering_memory  = false;
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


// Get the memory address that's being built up if _entering_memory,
// else return an empty string.
//
bool KeyCalculator::get_mem_entry(String* str) {
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
      *str += _double_to_string(_mem_buffer_index ? _calc.get_memory(atoi(_mem_buffer)) : _calc.get_memory());
      *str += ")";
    }
    return true;
  }
  if(str) *str = "";
  return false;
}


String KeyCalculator::get_operator_stack() {
  String result = "[ ";
  for(int i = 0; i < _calc.operator_stack.size(); i++) {
    result += char(_calc.operator_stack[i]);
    result += " ";
  }
  result += "]";
  return result;
}


String KeyCalculator::get_value_stack() {
  String result = "[ ";
  for(int i = 0; i < _calc.value_stack.size(); i++) {
    result += _double_to_string(_calc.value_stack[i]);
    result += " ";
  }
  result += "]";
  return result;
}


// Construct a string for a calculator to display showing status
//
String KeyCalculator::get_status_display() {
  String  str         = "";
  uint8_t arr_count   = 0;
  uint8_t stack_count = _calc.get_memory_depth();
  double  mem         = _calc.get_memory();

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
  if(stack_count) {
    str += "S() ";
    str += stack_count;
    str += "  ";
  }
  if(0.0 != mem) {
    str += "M=";
    str += _double_to_string(mem);
  }
  return str;
}


//  Clear (AC, the 'A' command) can be one or two keypresses
//  Memory operations can be multiple keypresses
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
  if('A' == _last_key && 'A' == code) {
    return _clear(true);
  }

  // Next, see if we are entering a multi-key memory command
  if(_entering_memory) {
    return _memory(code);
  }

  // Special case for chaining:
  // If you press 1 + =, most calculators will give you 2. The rule is:
  // When = is pressed, if _last_key was an operator (excluding =),
  // push the current value, then proceed to push = as normal.
  // Caution: % cannot be chained.
  //
  if(EVALUATE_OPERATOR == code) {
    if(ADDITION_OPERATOR       == _last_key || SUBTRACTION_OPERATOR == _last_key ||
       MULTIPLICATION_OPERATOR == _last_key || DIVISION_OPERATOR    == _last_key) {
      // can't use push_number() here
      enter(value());
    }
  }

  _last_key = code;

  // Next, see if a number is being entered (with B for Backspace)
  if(('0' <= code && '9' >= code) || ('.' == code) || 'B' == code) {
    return _build_number(code);
  }

  _push_number();

  // See if its a simple operator
  if(ADDITION_OPERATOR == code  || SUBTRACTION_OPERATOR == code || MULTIPLICATION_OPERATOR == code ||
     DIVISION_OPERATOR  == code || OPEN_PAREN_OPERATOR  == code || CLOSE_PAREN_OPERATOR    == code ||
     EVALUATE_OPERATOR  == code || PERCENT_OPERATOR     == code) {
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing operator '%c'\n", code);
    return enter(code);
  }

  // These operations require more complex handling
  switch(code) {
    case CHANGESIGN_OPERATOR: return _change_sign();
    case CLEAR_OPERATOR:      return _clear(false);
    case MEMORY_OPERATOR:     return _memory(code);
    default:                  return false;
  }
  return false;
}


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


String KeyCalculator::_convert_num_buffer(bool clear) {
  if(clear) _num_buffer_index = 0;
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_convert_num_buffer: %s\n", _num_buffer);
  return String(_num_buffer);
}


bool KeyCalculator::_clear(bool all_clear) {
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


bool KeyCalculator::_change_sign() {
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
bool KeyCalculator::_memory(uint8_t code) {
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_memory('%c')\n", code);
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
    if(ADDITION_OPERATOR  == code || SUBTRACTION_OPERATOR == code  || MULTIPLICATION_OPERATOR == code ||
       DIVISION_OPERATOR  == code || EVALUATE_OPERATOR    == code  || PERCENT_OPERATOR        == code ||
       CLEAR_OPERATOR     == code) {
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
