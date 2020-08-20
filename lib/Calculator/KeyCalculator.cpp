#include "KeyCalculator.h"

#define DEBUG_KEY_CALCULATOR  0


KeyCalculator::KeyCalculator() : TextCalculator() {
  _buffer_index = 0;
  _last_key     = 0;
}

bool KeyCalculator::key(uint8_t code) {
  // First, see if we just pressed the AC key two times
  if('A' == _last_key && 'A' == code) {
    return clear(true);
  }
  _last_key = code;

  // Next, see if a number is being entered
  if(('0' <= code && '9' >= code) || ('.' == code)) {
    return _build_number(code);
  }

  // Not a number anymore, so push any number that's been accumulated to the stack
  if(_buffer_index) {
    String str = _convert_buffer();
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing %s onto the stack\n", str.c_str());
    enter(str); // Do not return; push numver and continue processing
  }

  // See if its a simple operator
  if(ADDITION_OPERATOR == code || SUBTRACTION_OPERATOR == code || MULTIPLICATION_OPERATOR == code ||
    DIVISION_OPERATOR  == code || OPEN_PAREN_OPERATOR  == code || OPEN_PAREN_OPERATOR     == code ||
    EVALUATE_OPERATOR  == code || PRECENT_OPERATOR     == code) {
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing operator '%c'\n", code);
    return enter(code);
  }

  // These operations require more complex handling
  switch(code) {
    case '`':   return change_sign();
    case 'A':   return clear(false);
    case 'M':   return memory();
    default:    return false;
  }
  return false;
}


bool KeyCalculator::_build_number(uint8_t code) {
  _buffer[_buffer_index++] = code;
  _buffer[_buffer_index]   = '\0';
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_build_number: %s\n", _buffer);
  return true;
}


String KeyCalculator::_convert_buffer() {
  _buffer_index = 0;
  if(DEBUG_KEY_CALCULATOR) Serial.printf("_convert_buffer: %s\n", _buffer);
  return String(_buffer);
}


bool KeyCalculator::clear(bool all_clear) {
  if(all_clear) {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Clearing all memory");
    clear_all_memory();
    return true;
  }
  else {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Clearing memory");
    clear_memory();
    return true;
  }
  return false;
}


bool KeyCalculator::change_sign() {
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


bool KeyCalculator::memory() {
  return false;
}
