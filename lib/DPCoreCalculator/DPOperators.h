#pragma once
#include "DPCoreCalculator.h"
// Double precision floating point operators for the DPCoreCalculator class.
// By Van Kichline
// In the year of the plague


// Operator precedence: highest numbers are highest precedence. Values are unsigned 8 bits.
// Grouping   250   - pairs of (), [], etc.
// + -        225   - unary + and - prefixes
// exp root   200   - exponents and roots
// * / MOD    150   - multiplication, division and modulus
// + -        100   - addition and subtraction
//
// When operate() is called, the operator will already be popped off the operator_stack.
// The Operator must pop the operands, but not the operator.


// Base class for all other operators. This approach makes operators extensible.
//
class DPOperator {
  public:
    DPOperator(DPCoreCalculator* host) : _host(host) {}
    Op_ID             id                = OP_ID_NONE; // Is often a character, like '+' (43)
    uint8_t           precedence        = 0;          // Determines order of evaluation
    virtual bool      enough_values()   = 0;          // Override to determine if there are enough operands on the value_stack
    virtual Op_Err    operate()         = 0;          // Override to do what the operator does
  protected:
    DPCoreCalculator* _host;
};


// Most operators are binary (n + n) so reduce duplication through subclassing
//
class DPBinaryOperator: public DPOperator {
  public:
    DPBinaryOperator(DPCoreCalculator* host) : DPOperator(host) {}
    void set_id_and_precedence(Op_ID id, uint8_t precedence) {
      this->id          = id;
      this->precedence  = precedence;
    }
    bool enough_values() {
      return (2 <=_host->value_stack.size());
    }
    // Call this function to set up member variables before evaluation
    Op_Err prepare() {
      err = ERROR_TOO_FEW_OPERANDS;
      if(enough_values()) {
        op1 = _host->pop_value();
        op2 = _host->pop_value();
        err = NO_ERROR;
      }
      return err;
    }
    // This expects err to be set by the call to prepare. Pattern is:
    // err = prepare();
    // return push_result(op2 OPERATION op1);
    Op_Err push_result(double result) {
      if(NO_ERROR == err) {
        _host->push_value(result);
        if(DEBUG_EVALUATIONS) Serial.printf("Evaluation: %.4f + %.4f = %.4f\n", op2, op1, result);
        return NO_ERROR;
      }
      return err;
    }
  protected:
    double  op1     = 0.0;  // The top operand from the value_stack
    double  op2     = 0.0;  // The second opeand from the value_stack
    Op_Err  err;            // Shared error
};


class DPAdditionOperator : public DPBinaryOperator {
  public:
    DPAdditionOperator(DPCoreCalculator* host) : DPBinaryOperator(host) {
      set_id_and_precedence('+', 100);
    }
    Op_Err operate() {
      err = prepare();
      return push_result(op2 + op1);
    }
};


class DPSubtractionOperator : public DPBinaryOperator {
  public:
    DPSubtractionOperator(DPCoreCalculator* host) : DPBinaryOperator(host) {
      set_id_and_precedence('-', 100);
    }
    Op_Err operate() {
      err = prepare();
      return push_result(op2 - op1);
    }
};


class DPMultiplicationOperator : public DPBinaryOperator {
  public:
    DPMultiplicationOperator(DPCoreCalculator* host) : DPBinaryOperator(host) {
      set_id_and_precedence('*', 150);
    }
    Op_Err operate() {
      err = prepare();
      return push_result(op2 * op1);
    }
};


class DPDivisionOperator : public DPBinaryOperator {
  public:
    DPDivisionOperator(DPCoreCalculator* host) : DPBinaryOperator(host) {
      set_id_and_precedence('/', 150);
    }
    Op_Err operate() {
      err = prepare();
      return push_result(op2 / op1);
    }
};
