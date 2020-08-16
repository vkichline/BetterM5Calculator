#pragma once
#include <vector>
#include <map>
#include <Arduino.h>

// This is a no-frills, core calculator engine that handles double-precision floating point numbers.
// It uses the shunting-yard algorithm to process infix notification, so operations can be pushed as
// one would enter them on an ordinary calculator: Push 1, Push +, Push 1, Eval, the stack contains 1.
// It's expected that one would derive more functional, text or key based calculators derived from this.
// This implementation features plug-in operators (see Operators.h) so the set of operators can easily
// be extended.
// By Van Kichline
// In the year of the plague

#define OP_ID_NONE                  0   // Result of popping an empty _operator_stack
#define NO_ERROR                    0   // Good result from a calculation
#define ERROR_TOO_FEW_OPERANDS      -1  // Calculation error: not enough operands on the _operand_stack
#define ERROR_UNKNOWN_OPERATOR      -2  // Calculation error: the operator is not recognized.
#define DEBUG_EVALUATIONS           0   // Set to 1 for serial output on evaluation, 0 for silent evaluation

class   DPOperator;

typedef int16_t                       Op_Err;
typedef uint16_t                      Op_ID;
typedef std::map<Op_ID, DPOperator*>  DPOpMap;
typedef std::vector<double>           DPValues;
typedef std::vector<Op_ID>            DPOperators;
typedef std::pair<Op_ID, DPOperator*> DPOpSpec;


class DPCoreCalculator {
  public:
    DPCoreCalculator();

    bool        push_value(double value); // Enter a raw value value in base 10, regardless of num_mode, and push it onto the opperand stack
    double      pop_value();              // Return the top value of the _opperand_stack after removing it from the stack
    double      peek_value();             // Return the top value of the _opperand_stack without changing the stack

    bool        push_operator(Op_ID id);  // Push an operator onto the _operator_stack (may cause evaluation of lower precedence items on stack)
    Op_ID       pop_operator();           // Return the top value of the _operator_stack after removing it from the stack (OP_ID_NONE if empty)
    Op_ID       peek_operator();          // Return the top value of the _operator_stack without changing the stack (OP_ID_NONE if empty)

    Op_Err      evaluate();               // Evaluate the _operator_stack until its empty and there's only one operand (=)
    double      value();                  // Top of the _operand_stack, or 0.0 if stack is empty

    DPValues    value_stack;              // Pushdown stack for values. Unfortunately, <stack> is broken
    DPOperators operator_stack;           // pushdown stack for operators

  protected:
    Op_Err      _evaluate_one();          // Evaluate the top operator on the _operator_stack (if any)
    DPOpMap     _operators;               // A map of all the operators (extensible!)
    void        _initialize_operators();  // Put Operators in _operators map
};
