#include "KeyCalculator.h"

#define DEBUG_KEY_CALCULATOR  0


KeyCalculator::KeyCalculator() : TextCalculator() {
  _buffer_index = 0;
  _last_key     = 0;
}

bool KeyCalculator::key(uint8_t code) {
  if('M' == _last_key && 'M' == code) {
    return clear(true);
  }
  if(('0' <= code && '9' >= code) || ('.' == code)) {
    return _build_number(code);
  }
  if(_buffer_index) {
    String str = _convert_buffer();
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing %s onto the stack\n", str.c_str());
    return enter(str);
  }
  if('+' == code || '-' == code || '*' == code || '/' == code || '+' == code || '=' == code || '%' == code) {
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing operator '%c'\n", code);
    return enter(code);
  }
Serial.println("I don't expect to get here yet...");
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
  // Need to implement clear in TextCalculator
  return false;
}

bool KeyCalculator::change_sign() { return false; }
bool KeyCalculator::memory()      { return false; }
