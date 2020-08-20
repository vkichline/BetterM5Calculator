#include "TextCalculator.h"

#define DEBUG_PARSING   0


TextCalculator::TextCalculator(uint8_t precision) {
  // set_mode(mode);
  _precision  = precision;
  _error      = NO_ERROR;
  _ops        = { ADDITION_OPERATOR, SUBTRACTION_OPERATOR, MULTIPLICATION_OPERATOR, DIVISION_OPERATOR, OPEN_PAREN_OPERATOR, CLOSE_PAREN_OPERATOR, EVALUATE_OPERATOR };
  _nums       = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', /*'+', '-'*/ };
  _wspace     = { ' ', '\t', '\n', '\r' };
}

bool TextCalculator::enter(const char* value) {
  double val = _string_to_double(value);
  return _calc.push_value(val);
}

bool TextCalculator::enter(String value) {
  return enter(value.c_str());
}

bool TextCalculator::enter(Op_ID id) {
  return _calc.push_operator(id);
}

Op_Err TextCalculator::total() {
  return _calc.evaluate();
}

 String TextCalculator::value() {
  return _double_to_string(_calc.get_value());
}

void TextCalculator::copy_to_memory() {
  _calc.set_memory(_calc.get_value());
}

bool TextCalculator::copy_to_memory(uint8_t index) {
  return _calc.set_memory(index, _calc.get_value());
}

void TextCalculator::push() {
  return _calc.push_memory(_calc.get_value());
}

void TextCalculator::recall_memory() {
  _calc.value_stack.back() = _calc.get_memory();
}

void TextCalculator::recall_memory(uint8_t index) {
  _calc.value_stack.back() = _calc.get_memory(index);
}

void TextCalculator::pop() {
  if(0 == _calc.value_stack.size()) {
    _calc.push_memory(_calc.pop_memory());
  }
  else {
    _calc.value_stack.back() = _calc.pop_memory();
  }
}

void TextCalculator::clear_memory() {
  _calc.set_memory(0.0);
}

void TextCalculator::clear_all_memory() {
  _calc.clear_all_memory();
}


String TextCalculator::_double_to_string(double val) {
  return String(val);   // trivial implementation; will use precision and base later
}

double TextCalculator::_string_to_double(const char* val) {
  return atof(val);     // trivial implementation; will use precision and base later
}

String TextCalculator::parse(String statement) {
  if(parse(statement.c_str())) {
    return value();
  }
  else {
    return String("Error");
  }
}


// BUGBUG: This approach doesn't handle unary +/-
//
bool TextCalculator::parse(const char* statement) {
  int   index = 0;
  char  buffer[64];
  if(DEBUG_PARSING) Serial.printf("\nDebugging parse(%s)\n", statement);
  while(index < strlen(statement)) {
    char c = statement[index++];
    // Skip over whitespace
    if(_wspace.find(c) == _wspace.end()) {
      if(_ops.find(uint8_t(c)) != _ops.end()) {
        if(DEBUG_PARSING) Serial.printf("Pushing operator %c\n", c);
        if(!enter(c)) {
          if(DEBUG_PARSING) Serial.println("Error pushing operator! Returning false\n");
          return false;
        }
      }
      if(_nums.find(c) != _nums.end()) {
        int bi = 1;
        buffer[0] = c;
        if(DEBUG_PARSING) Serial.println("Parsing number");
        while(_nums.count( statement[index])) {
          buffer[bi++] = statement[index++];
        }
        buffer[bi] = '\0';
        if(DEBUG_PARSING) Serial.printf("Pushing value %s\n", buffer);
        if(!enter(buffer)) {
          if(DEBUG_PARSING) Serial.println("Error pushing value! Returning false\n");
          return false;
        }
      }
    }
  }
  if(DEBUG_PARSING) Serial.println("Parse complete");
  return true;
}
