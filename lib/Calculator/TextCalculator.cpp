#include "TextCalculator.h"

#define DEBUG_PARSING   0


TextCalculator::TextCalculator(uint8_t precision) {
  // set_mode(mode);
  _precision  = precision;
  _error      = NO_ERROR;
  _ops        = { ADDITION_OPERATOR, SUBTRACTION_OPERATOR, MULTIPLICATION_OPERATOR, DIVISION_OPERATOR, OPEN_PAREN_OPERATOR, CLOSE_PAREN_OPERATOR, EVALUATE_OPERATOR };
  _nums       = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', /*'+', '-'*/ };
  _wspace     = { ' ', '\t', '\n', '\r' };
  enter("0");   // Start with an empty value on the stack.
}

// It's not obvious, but if the operator_stack is empty, the value_stack should be cleared.
// Otherwise, 1+1= 1+1= 1+1= results in a useless value_stack containing [2 2 2]
//
bool TextCalculator::enter(const char* value) {
  if(0 == _calc.operator_stack.size()) _calc.value_stack.clear();
  double val = _string_to_double(value);
  return NO_ERROR == _calc.push_value(val);
}

bool TextCalculator::enter(String value) {
  return enter(value.c_str());
}

// Like entering a value, clear the value stack if the Operatr_stack is empty, and this is a '('
//
bool TextCalculator::enter(Op_ID id) {
  if((OPEN_PAREN_OPERATOR == id) && (0 == _calc.operator_stack.size())) _calc.value_stack.clear();
  return (NO_ERROR == _calc.push_operator(id));
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
  return NO_ERROR == _calc.set_memory(index, _calc.get_value());
}

void TextCalculator::push() {
  return _calc.push_memory(_calc.get_value());
}

bool TextCalculator::recall_memory() {
  _calc.value_stack.push_back(_calc.get_memory());
  return true;
}

bool TextCalculator::recall_memory(uint8_t index) {
  if(NUM_CALC_MEMORIES - 1 <= index) return false;
  _calc.value_stack.push_back(_calc.get_memory(index));
  return true;
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

Op_Err TextCalculator::get_error_state() {
  return _calc.get_error_state();
}

void TextCalculator::clear_error_state() {
  _calc.clear_error_state();
}


// Convert the value to a string, with no trailing decimal point.
// This algorithm only works for positive numbers, so for negative numbers
// insert a - in the buffer and invert val
//
String TextCalculator::_double_to_string(double val) {
  if(0.0 == val) return String("0");
  double  threshold   = 1.0 / pow(10.0, _precision);  // Precision is a private member variable
  int     m           = log10(ceil(abs(val)));
  char    buffer[64]  = {0};
  int     digit       = 0;
  char*   p           = buffer;

  if(0.0 > val) {
    val    = -val;
    *(p++) = '-';
  }
  while((0 <= m) || (val > threshold)) {
    double weight = pow(10.0, m);
    digit = floor(val / weight);
    val  -= (digit * weight);
    *(p++)= '0' + digit;
    if(m == 0) *(p++) = '.';
    *p    = '\0';
    m--;
  }
  // if the last character is a '.', delete it
  if('.' == *(p-1)) *(p-1) = '\0';
  return String(buffer);
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
