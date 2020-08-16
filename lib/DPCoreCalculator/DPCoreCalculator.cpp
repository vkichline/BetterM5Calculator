#include <Arduino.h>
#include "DPCoreCalculator.h"
#include "DPOperators.h"


DPCoreCalculator::DPCoreCalculator() {
  _initialize_operators();
}


// Place all the operators we plan to use in the _operators vector.
// This design makes it trivial to add additional operators.
// Note: OP_ID_NONE is not put in _operators, by design.
//
void DPCoreCalculator::_initialize_operators() {
  _operators.insert(DPOpSpec('+', new DPAdditionOperator(this)));
  _operators.insert(DPOpSpec('-', new DPSubtractionOperator(this)));
  _operators.insert(DPOpSpec('*', new DPMultiplicationOperator(this)));
  _operators.insert(DPOpSpec('/', new DPDivisionOperator(this)));
}


// Evaluate the top operator on the operator_stack (if any)
// BUGBUG: how is overflow/underflow detected?
//
Op_Err DPCoreCalculator::_evaluate_one() {
  if(1 <= operator_stack.size()) {
    Op_ID id = pop_operator();
    if(!_operators.count(id)) return ERROR_UNKNOWN_OPERATOR;
    DPOperator* op = _operators[id];
    if(!op->enough_values()) return ERROR_TOO_FEW_OPERANDS;
    return op->operate();
  }
  return NO_ERROR;  // Nothing to do; that's not an error
}


// Evaluate the operator_stack until its empty and there's only one operand
// (Typical response to the = key)
//
Op_Err DPCoreCalculator::evaluate() {
  while(1 <= operator_stack.size()) {
    Op_Err err = _evaluate_one();
    if(err) return err;
  }
  return NO_ERROR;
}


// Push an operator onto the operator_stack.
// The operator must exist in _operators.
// Shunting-yard algorithm for evaluating infix notation (ref: Dijkstra)
// If the operator on the _operator stack is the same or greater precedence
// than this operator, evaluate it first.
// TODO: If the operator is an CLOSE_PAREN, evaluate all until OPEN_PAREN is popped.
//
bool DPCoreCalculator::push_operator(Op_ID id) {
  if(_operators.count(id)) {  // If it's a valid operator
    DPOperator* incoming = _operators[id];
    while(true) {
      Op_ID top = peek_operator();
      if(!_operators.count(top)) break;
      DPOperator* top_op = _operators[top];
      if(top_op->precedence < incoming->precedence) break;
      _evaluate_one();
    }
    operator_stack.push_back(id);
    return true;
  }
  return false;
}


// Top of the value_stack, or 0.0 if stack is empty
// (What would display on a calculator screen)
//
double DPCoreCalculator::value() {
  if(0 == value_stack.size()) return 0.0;
  return peek_value();
}


// Push a value onto the stack to be processed later.
//
bool DPCoreCalculator::push_value(double value) {
  value_stack.push_back(value);
  return true;
}


// Return the top value from the stack and pop it off.
//
double DPCoreCalculator::pop_value() {
  double value = value_stack.back();
  value_stack.pop_back();
  return value;
}


// Return the top value from the stack without modifying the stack.
//
double DPCoreCalculator::peek_value() {
  return value_stack.back();
}


// Return the top value of the operator_stack after popping it.
// Return OP_ID_NONE if the stack is empty.
// Note: OP_ID_NONE is not in _operators, by design.
//
Op_ID DPCoreCalculator::pop_operator() {
  if(0 == operator_stack.size()) return OP_ID_NONE;
  Op_ID id = operator_stack.back();
  operator_stack.pop_back();
  return id;
}


// Return the top value of the operator_stack without changing the stack (OP_ID_NONE if empty)
//
Op_ID DPCoreCalculator::peek_operator() {
  if(0 == operator_stack.size()) return OP_ID_NONE;
  Op_ID id = operator_stack.back();
  return id;
}
