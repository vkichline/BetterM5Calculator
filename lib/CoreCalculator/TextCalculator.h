#pragma once
#include <set>
#include "MemoryCalculator.h"

// There are two calculator modes now, but there may be more in the future: currency, huge, etc.
enum CalcMode { Calc_Mode_FP, Calc_Mode_Integer };
enum CalcBase { Calc_Base_Binary = 2, Calc_Base_Octal = 8, CalcBase_Decimal = 10, Calc_Base_Hexidecimal = 16 };
enum CalcTrigMode { Calc_Trig_Mode_Degrees, Calc_Trig_Mode_Radians, Calc_Trig_Mode_Grads };

#define ERROR_UNKNOWN_MODE  -100
#define DEBUG_PARSING       1

// This class contains the MemoryCalculator and provides an implementation that uses
// strings to represent numeric values. It can be used for a command-line
// calculator, or further augmented to respond to individual keystrokes.
// Note that operations generally affect and concern the value.
// Rather than getting a value from memory, you retrieve a value, and it
// replaces the value on the top of the value stack.

class TextCalculator {
  public:
    TextCalculator(uint8_t precision = 8);
    bool      parse(const char* statement);
    String    parse(String statement);
    bool      enter(const char* value);
    bool      enter(String value);
    bool      enter(Op_ID id);
    Op_Err    total();
    String    value();
    void      move_to_memory();
    bool      move_to_memory(uint8_t index);
    void      push();
    void      retrieve_memory();
    void      retrieve_memory(uint8_t index);
    void      pop();
  protected:
    MemoryCalculator<double, 10>  _calc;
    std::set<uint8_t>             _ops;
    std::set<char>                _nums;
    std::set<char>                _wspace;
    CalcMode                      _mode;
    uint8_t                       _precision;
    Op_Err                        _error;
    String                        _double_to_string(double val);
    double                        _string_to_double(const char* val);
};


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

void TextCalculator::move_to_memory() {
  _calc.set_memory(_calc.get_value());
}

bool TextCalculator::move_to_memory(uint8_t index) {
  return _calc.set_memory(index, _calc.get_value());
}

void TextCalculator::push() {
  return _calc.push_memory(_calc.get_value());
}

void TextCalculator::retrieve_memory() {
  if(0 == _calc.value_stack.size())
    _calc.push_memory(_calc.get_memory());
  else
    _calc.value_stack.back() = _calc.get_memory();
}

void TextCalculator::retrieve_memory(uint8_t index) {
  if(0 == _calc.value_stack.size())
    _calc.push_memory(_calc.get_memory(index));
  else
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
