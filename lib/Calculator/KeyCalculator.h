#pragma once

#include "TextCalculator.h"

// This layer of the calculator interacts with a Calculator keyboard,
// where numbers are entered as individual digits. Some commands may
// by keys, and others may be entered by function call.

#define KEY_CALCULATOR_BUFFER_SIZE  64


class KeyCalculator : public TextCalculator {
  public:
    KeyCalculator();
    bool key(uint8_t code);
    bool change_sign();
    bool clear(bool all_clear);
    bool memory();
  protected:
    char    _buffer[KEY_CALCULATOR_BUFFER_SIZE] = {0};
    uint8_t _buffer_index                       =  0;
    uint8_t _last_key                           =  0;
    bool    _build_number(uint8_t code);
    String  _convert_buffer();
};
