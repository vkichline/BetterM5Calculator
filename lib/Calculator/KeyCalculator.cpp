#include "KeyCalculator.h"

#define DEBUG_KEY_CALCULATOR  0
#define MEMORY_OPERATOR       (uint8_t('M'))


KeyCalculator::KeyCalculator() : TextCalculator() {
  _num_buffer_index = 0;
  _mem_buffer_index = 0;
  _last_key         = 0;
  _entering_memory  = false;
}

//  Clear (AC, the 'A' command) can be one or two keypresses
//  Memory operations can be multiple keypresses
//
bool KeyCalculator::key(uint8_t code) {
  // First, see if we just pressed the AC key two times
  if('A' == _last_key && 'A' == code) {
    return clear(true);
  }

  // Next, see if we are entering a multi-key memory command
  if(_entering_memory) {
    return memory(code);
  }

  _last_key = code;

  // Next, see if a number is being entered
  if(('0' <= code && '9' >= code) || ('.' == code)) {
    return _build_number(code);
  }

  _push_number();

  // See if its a simple operator
  if(ADDITION_OPERATOR == code  || SUBTRACTION_OPERATOR == code || MULTIPLICATION_OPERATOR == code ||
     DIVISION_OPERATOR  == code || OPEN_PAREN_OPERATOR  == code || OPEN_PAREN_OPERATOR     == code ||
     EVALUATE_OPERATOR  == code || PRECENT_OPERATOR     == code) {
    if(DEBUG_KEY_CALCULATOR) Serial.printf("pushing operator '%c'\n", code);
    return enter(code);
  }

  // These operations require more complex handling
  switch(code) {
    case '`':   return change_sign();
    case 'A':   return clear(false);
    case 'M':   return memory(code);
    default:    return false;
  }
  return false;
}


bool KeyCalculator::_build_number(uint8_t code) {
  if(KEY_CALCULATOR_NUM_BUFFER_SIZE <= _num_buffer_index - 1) return false;
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


bool KeyCalculator::clear(bool all_clear) {
  if(all_clear) {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Clearing all memory");
    clear_all_memory();
    return true;
  }
  else {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Clearing value");
    return _calc.clear();
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


// Memory commands are complicated. There is a simple memory, a memory array, and a memory stack.
// The memory array size is established with the MemoryCalculator template parameter M and can be
// ascertained with _calc.get_mem_array_size().
// Once M is pressed, it can be followed with another M for recall, a memory operator (+-*/%=)
// to store the value in the simple memory, or (0-9) to build a memory address for the operator:
// MM   Recall simple memory      (M -> Value)
// M=   Store simple memory       (Value -> M)
// M+   Simple memory opperation  (M + Value -> M)
// M0=  Store into M[0]           (Value -> M[0])
// M9=  Store into M[9]           (Value -> M[9])
// Note that there may be 10 or thousands of memories.
//
bool KeyCalculator::memory(uint8_t code) {
  if(DEBUG_KEY_CALCULATOR) Serial.printf("memory('%c')\n", code);
  if(!_entering_memory) {
    if(DEBUG_KEY_CALCULATOR) Serial.println("Setting _entering_memory");
    assert(MEMORY_OPERATOR == code);
    _entering_memory = true;
    return true;
  }
  else {
    // Build memory name if it's a number
    if(('0' <= code && '9' >= code) || ('.' == code)) {
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
    if(ADDITION_OPERATOR  == code || SUBTRACTION_OPERATOR == code || MULTIPLICATION_OPERATOR == code ||
       DIVISION_OPERATOR  == code || EVALUATE_OPERATOR    == code   || PRECENT_OPERATOR      == code) {
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
  return false;
}
