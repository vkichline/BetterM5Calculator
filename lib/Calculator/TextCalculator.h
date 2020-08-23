#pragma once
#include <set>
#include "MemoryCalculator.h"

// There are two calculator modes now, but there may be more in the future: currency, huge, etc.
enum CalcMode { Calc_Mode_FP, Calc_Mode_Integer };
enum CalcBase { Calc_Base_Binary = 2, Calc_Base_Octal = 8, CalcBase_Decimal = 10, Calc_Base_Hexidecimal = 16 };
enum CalcTrigMode { Calc_Trig_Mode_Degrees, Calc_Trig_Mode_Radians, Calc_Trig_Mode_Grads };

#define NUM_CALC_MEMORIES   100
#define ERROR_UNKNOWN_MODE  -100

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
    void      copy_to_memory();
    bool      copy_to_memory(uint8_t index);
    void      push();
    bool      recall_memory();
    bool      recall_memory(uint8_t index);
    void      pop();
    void      clear_memory();
    void      clear_all_memory();
    Op_Err    get_error_state();
    void      clear_error_state();
  protected:
    MemoryCalculator<double, NUM_CALC_MEMORIES> _calc;
    std::set<uint8_t>                           _ops;
    std::set<char>                              _nums;
    std::set<char>                              _wspace;
    CalcMode                                    _mode;
    uint8_t                                     _precision;
    Op_Err                                      _error;
    String                                      _double_to_string(double val);
    double                                      _string_to_double(const char* val);
};
