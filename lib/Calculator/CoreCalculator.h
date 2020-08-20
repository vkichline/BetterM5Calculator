#pragma once

// This is a template for a no-frills, core calculator engine that handles any numeric type.
// It uses the shunting-yard algorithm to process infix notification, so operations can be pushed as
// one would enter them on an ordinary calculator: Push 1, Push +, Push 1, Eval, the stack contains 1.
// It's expected that one would derive more functional, text or key based calculators derived from this.
// This implementation features plug-in operators so that the set of operators can easily be extended.
//
// By Van Kichline
// In the year of the plague

#include <vector>
#include <map>
#include <Arduino.h>

#define OP_ID_NONE                 0                        // Result of popping an empty operator_stack
#define NO_ERROR                   0                        // Good result from a calculation
#define ERROR_TOO_FEW_OPERANDS    -1                        // Calculation error: not enough operands on the value_stack
#define ERROR_UNKNOWN_OPERATOR    -2                        // Calculation error: the operator is not recognized.

#define DEBUG_OPERATORS            0                        // 0 is quiet, 1 spews info for debugging operators
#define DEBUG_EVALUATION           0                        // 0 is quiet, 1 spews info for debugging evaluations

#define ADDITION_OPERATOR         (uint8_t('+'))
#define SUBTRACTION_OPERATOR      (uint8_t('-'))
#define MULTIPLICATION_OPERATOR   (uint8_t('*'))
#define DIVISION_OPERATOR         (uint8_t('/'))
#define PRECENT_OPERATOR          (uint8_t('%'))
#define OPEN_PAREN_OPERATOR       (uint8_t('('))
#define CLOSE_PAREN_OPERATOR      (uint8_t(')'))
#define EVALUATE_OPERATOR         (uint8_t('='))            // This is a special operator that is not implemented as a class or added to _operators


template<typename T> class        Operator;                 // Forward declaration to operator template
typedef int16_t                   Op_Err;                   // Signed error; 0 is no error, errors are normally negative. See #define ERROR_*
typedef uint16_t                  Op_ID;                    // Operators are normally identified by a single char like '+'. More may be needed.


template <typename T>
class CoreCalculator {
  public:
    CoreCalculator();
    bool                          push_value(T value);      // Enter a raw value value in base 10, regardless of num_mode, and push it onto the value_stack
    T                             pop_value();              // Return the top value of the value_stack after removing it from the stack
    T                             peek_value();             // Return the top value of the value_stack without changing the stack
    bool                          push_operator(Op_ID id);  // Push an operator onto the operator_stack (may cause evaluation of lower precedence items on stack)
    Op_ID                         pop_operator();           // Return the top value of the operator_stack after removing it from the stack (OP_ID_NONE if empty)
    Op_ID                         peek_operator();          // Return the top value of the operator_stack without changing the stack (OP_ID_NONE if empty)
    Op_Err                        evaluate_one();           // Evaluate the top operator on the operator_stack (if any)
    Op_Err                        evaluate();               // Evaluate the operator_stack until its empty and there's only one operand (=)
    T                             get_value();              // Top of the operand_stack, or 0.0 if stack is empty
    bool                          clear();                  // Change value to zero
    std::vector<T>                value_stack;              // Pushdown stack for values. Unfortunately, <stack> is broken
    std::vector<Op_ID>            operator_stack;           // pushdown stack for operators
  protected:
    void                          _initialize_operators();  // Put Operators in _operators map
    std::map<Op_ID, Operator<T>*> _operators;               // A map of all the operators (extensible!)
    void                          _spew_stacks();           // Writes operator_stack and value_stack to Serial
};


// Base class for all other operators. This approach makes operators extensible.
// Operator precedence: highest numbers are highest precedence. Values are unsigned 8 bits.
// Grouping   250   - pairs of (), [], etc.
// + -        200   - unary + and - prefixes
// exp root   150   - exponents and roots
// * / MOD    100   - multiplication, division and modulus
// + -         50   - addition and subtraction
// Evaluate     0   - always evaluate
//
// When operate() is called, the operator will already be popped off the operator_stack.
// The Operator must pop the operands, but not the operator.
//
template <typename T>
class Operator {
  public:
    Operator(CoreCalculator<T>* host) : _host(host) {}
    Op_ID             id                = OP_ID_NONE; // Is often a character, like '+' (43)
    uint8_t           precedence        = 0;          // Determines order of evaluation
    virtual bool      enough_values()   = 0;          // Override to determine if there are enough operands on the _operand_stack
    virtual Op_Err    operate()         = 0;          // Override to do what the operator does
  protected:
    CoreCalculator<T>*  _host;
};


////////////////////////////////////////////////////////////////////////////////
//
//  Operators Implementation
//
////////////////////////////////////////////////////////////////////////////////

// Convenience class to make binary operators (n OP n) trivial to implement.
// Usage:
// In your derived template's constructor:
//  BinaryOperator<T>::set_id_and_precedence('-', 100);
// In your derived template's operate():
//  BinaryOperator<T>::err = BinaryOperator<T>::prepare();
//  return BinaryOperator<T>::push_result(BinaryOperator<T>::op2 [BINARY_OPERATOR] BinaryOperator<T>::op1);
//
template <typename T>
class BinaryOperator: public Operator<T> {
  public:
    BinaryOperator(CoreCalculator<T>* host) : Operator<T>(host) {}
    void set_id_and_precedence(Op_ID id, uint8_t precedence) {
      this->id          = id;
      this->precedence  = precedence;
    }
    bool enough_values() {
      return (2 <= Operator<T>::_host->value_stack.size());
    }
    // Call this function to set up member variables before evaluation
    Op_Err prepare() {
      err = ERROR_TOO_FEW_OPERANDS;
      if(enough_values()) {
        op1 = Operator<T>::_host->pop_value();
        op2 = Operator<T>::_host->pop_value();
        err = NO_ERROR;
      }
      return err;
    }
    // This expects err to be set by the call to prepare. Pattern is:
    Op_Err push_result(double result) {
      if(NO_ERROR == err) {
        Operator<T>::_host->push_value(result);
        return NO_ERROR;
      }
      return err;
    }
  protected:
    double  op1     = 0.0;  // The top operand from the value_stack
    double  op2     = 0.0;  // The second opeand from the value_stack
    Op_Err  err;            // Shared error
};

template <typename T>
class AdditionOperator : public BinaryOperator<T> {
  public:
    AdditionOperator(CoreCalculator<T>* host) : BinaryOperator<T>(host) {
      BinaryOperator<T>::set_id_and_precedence(ADDITION_OPERATOR, 50);
    }
    Op_Err operate() {
      BinaryOperator<T>::err = BinaryOperator<T>::prepare();
      return BinaryOperator<T>::push_result(BinaryOperator<T>::op2 + BinaryOperator<T>::op1);
    }
};

template <typename T>
class SubtractionOperator : public BinaryOperator<T> {
  public:
    SubtractionOperator(CoreCalculator<T>* host) : BinaryOperator<T>(host) {
      BinaryOperator<T>::set_id_and_precedence(SUBTRACTION_OPERATOR, 50);
    }
    Op_Err operate() {
      BinaryOperator<T>::err = BinaryOperator<T>::prepare();
      return BinaryOperator<T>::push_result(BinaryOperator<T>::op2 - BinaryOperator<T>::op1);
    }
};

template <typename T>
class MultiplicationOperator : public BinaryOperator<T> {
  public:
    MultiplicationOperator(CoreCalculator<T>* host) : BinaryOperator<T>(host) {
      BinaryOperator<T>::set_id_and_precedence(MULTIPLICATION_OPERATOR, 100);
    }
    Op_Err operate() {
      BinaryOperator<T>::err = BinaryOperator<T>::prepare();
      return BinaryOperator<T>::push_result(BinaryOperator<T>::op2 * BinaryOperator<T>::op1);
    }
};

template <typename T>
class DivisionOperator : public BinaryOperator<T> {
  public:
    DivisionOperator(CoreCalculator<T>* host) : BinaryOperator<T>(host) {
      BinaryOperator<T>::set_id_and_precedence(DIVISION_OPERATOR, 100);
    }
    Op_Err operate() {
      BinaryOperator<T>::err = BinaryOperator<T>::prepare();
      return BinaryOperator<T>::push_result(BinaryOperator<T>::op2 / BinaryOperator<T>::op1);
    }
};

// Beginning of grouping does nothing, just puts a token on the stack
template <typename T>
class OpenParenOperator : public Operator<T> {
  public: 
    OpenParenOperator(CoreCalculator<T>* host) : Operator<T>(host) {
      Operator<T>::id          = OPEN_PAREN_OPERATOR;
      Operator<T>::precedence  = 250;
    }
    bool    enough_values() { return true;     }
    Op_Err  operate()       { return NO_ERROR; }
};

// End of grouping: keep evaluating until an open paren if found.
// Pop the open paren operator, the result is already on the value_stack.
//
template <typename T>
class CloseParenOperator : public Operator<T> {
  public:
    CloseParenOperator(CoreCalculator<T>* host) : Operator<T>(host) {
      Operator<T>::id          = CLOSE_PAREN_OPERATOR;
      Operator<T>::precedence  = 250;
    }
    bool    enough_values() { return true; }
    Op_Err  operate() {
      while(OPEN_PAREN_OPERATOR != Operator<T>::_host->peek_operator()) {
        Op_Err err = Operator<T>::_host->evaluate_one();
        if(err) return err;
      }
      Operator<T>::_host->pop_operator();
      return NO_ERROR;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
//  CoreCalculator Implementation
//
////////////////////////////////////////////////////////////////////////////////

template <typename T> CoreCalculator<T>::CoreCalculator() {
  _initialize_operators();
}

// Push a value onto the stack to be processed later.
//
template <typename T> bool CoreCalculator<T>::push_value(T value) {
  value_stack.push_back(value);
  return true;
}

// Return the top value from the stack and pop it off.
//
template <typename T> T CoreCalculator<T>::pop_value() {
  T value = value_stack.back();
  value_stack.pop_back();
  return value;
}

// Return the top value from the stack without modifying the stack.
//
template <typename T> T CoreCalculator<T>::peek_value() {
  return value_stack.back();
}

// Push an operator onto the operator_stack.
// The operator must exist in _operators.
// Shunting-yard algorithm for evaluating infix notation (ref: Dijkstra)
// If the operator on the operator_stack is the same or greater precedence
// than this operator, evaluate it first.
//
template <typename T> bool CoreCalculator<T>::push_operator(Op_ID id) {
  // The EVALUATE_OPERATOR is handled specially here. It's not in _operators
  if(EVALUATE_OPERATOR == id) {
    if(DEBUG_OPERATORS) Serial.println("\nOperator = : evaluating the entire stack\n");
    Op_Err result =  evaluate();
    if(DEBUG_OPERATORS) Serial.printf("evaluate() returned %d\n", result);
    return (NO_ERROR == result);
  }
  if(DEBUG_OPERATORS) Serial.printf("\nPushing operator %c\n", id);
  if(_operators.count(id)) {  // If it's a valid operator
    Operator<T>* incoming = _operators[id];
    if(DEBUG_OPERATORS) Serial.println("Operator valid");
    while(true) {
      Op_ID top = peek_operator();
      if(!_operators.count(top)) break;
      if(OPEN_PAREN_OPERATOR == top) break;   // Only the CloseParenOperator removes the OpenParenOperator
      Operator<T>* top_op = _operators[top];
      if(top_op->precedence < incoming->precedence) break;
      if(DEBUG_OPERATORS) Serial.printf("Forcing operator %c\n", top);
      if(evaluate_one()) return false;
      if(DEBUG_OPERATORS) Serial.println("Forced operation successful\n");
    }
    operator_stack.push_back(id);
    return true;
  }
  return false;
}

// Return the top value of the operator_stack after popping it.
// Return OP_ID_NONE if the stack is empty.
// Note: OP_ID_NONE is not in _operators, by design.
//
template <typename T> Op_ID CoreCalculator<T>::pop_operator() {
  if(0 == operator_stack.size()) return OP_ID_NONE;
  Op_ID id = operator_stack.back();
  operator_stack.pop_back();
  return id;
}

// Return the top value of the operator_stack without changing the stack (OP_ID_NONE if empty)
//
template <typename T> Op_ID CoreCalculator<T>::peek_operator() {
  if(0 == operator_stack.size()) return OP_ID_NONE;
  Op_ID id = operator_stack.back();
  return id;
}

// Evaluate the operator_stack until its empty and there's only one operand
// (Typical response to the = key)
//
template <typename T> Op_Err CoreCalculator<T>::evaluate() {
  while(1 <= operator_stack.size()) {
    Op_Err err = evaluate_one();
    if(err) {
      if(DEBUG_EVALUATION) Serial.printf("evaluate returning error: %d\n", err);
      return err;
    }
  }
  return NO_ERROR;
}

// Top of the value_stack, or 0 if stack is empty
// (What would display on a calculator screen)
//
template <typename T> T CoreCalculator<T>::get_value() {
  if(0 == value_stack.size()) return T(0);
  return peek_value();
}

// Set the value to zero. Do not change size of the stack unless it's empty,
//
template <typename T> bool CoreCalculator<T>::clear() {
  if(0 != value_stack.size()) value_stack.pop_back();
  value_stack.push_back(T(0));
  return true;
}

// Evaluate the top operator on the operator_stack (if any)
// BUGBUG: how is overflow/underflow detected?
//
template <typename T> Op_Err CoreCalculator<T>::evaluate_one() {
  if(DEBUG_EVALUATION) { Serial.print("\nevaluate_one: "); _spew_stacks(); }
  if(1 <= operator_stack.size()) {
    Op_ID id = pop_operator();
    if(DEBUG_EVALUATION) Serial.printf("popped operator %c\n", id);
    if(!_operators.count(id)) {
      if(DEBUG_EVALUATION) Serial.println("operator unknown; returning false");
      return ERROR_UNKNOWN_OPERATOR;
    }
    Operator<T>* op = _operators[id];
    if(!op->enough_values()) {
      if(DEBUG_EVALUATION) Serial.println("not enough operands; returning false");
      return ERROR_TOO_FEW_OPERANDS;
    }
    bool result = op->operate();
    if(DEBUG_EVALUATION) Serial.printf("Result of %c operation: %d\n", id, result);
    return result;
  }
  if(DEBUG_EVALUATION) Serial.println("operator stack empty");
  return NO_ERROR;  // Nothing to do; that's not an error
}

// Place all the operators we plan to use in the _operators vector.
// This design makes it trivial to add additional operators.
// Note: OP_ID_NONE is not put in _operators, by design.
//
template <typename T> void CoreCalculator<T>::_initialize_operators() {
  _operators.insert(std::pair<Op_ID, Operator<T>*>(ADDITION_OPERATOR,       new AdditionOperator<T>(this)));
  _operators.insert(std::pair<Op_ID, Operator<T>*>(SUBTRACTION_OPERATOR,    new SubtractionOperator<T>(this)));
  _operators.insert(std::pair<Op_ID, Operator<T>*>(MULTIPLICATION_OPERATOR, new MultiplicationOperator<T>(this)));
  _operators.insert(std::pair<Op_ID, Operator<T>*>(DIVISION_OPERATOR,       new DivisionOperator<T>(this)));
  _operators.insert(std::pair<Op_ID, Operator<T>*>(OPEN_PAREN_OPERATOR,     new OpenParenOperator<T>(this)));
  _operators.insert(std::pair<Op_ID, Operator<T>*>(CLOSE_PAREN_OPERATOR,    new CloseParenOperator<T>(this)));
}

// Writes operator_stack and value_stack to Serial for debugging
//
template <typename T> void CoreCalculator<T>::_spew_stacks() {
  String str = "Op Stack: [ ";
  if(0 == operator_stack.size()) {
    str += "EMPTY ";
  }
  else {
    for(int i = 0; i < operator_stack.size(); i++) { str += char(operator_stack[i]); str += ' '; }
  }
  str += "]  Val Stack: [ ";
  if(0 == value_stack.size()) {
    str += "EMPTY ";
  }
  else {
    for(int i = 0; i < value_stack.size(); i++) { str += value_stack[i]; str += ' '; }
  }
  str += "]";
  Serial.println(str.c_str());
}
