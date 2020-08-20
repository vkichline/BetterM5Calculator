#pragma once

#include "TextCalculator.h"

// This layer of the calculator interacts with a Calculator keyboard,
// where numbers are entered as individual digits. Some commands may
// by keys, and others may be entered by function call.

#define KEY_CALCULATOR_NUM_BUFFER_SIZE  64
#define KEY_CALCULATOR_MEM_BUFFER_SIZE   8


class KeyCalculator : public TextCalculator {
  public:
    KeyCalculator();
    bool key(uint8_t code);
    bool change_sign();
    bool clear(bool all_clear = false);
    bool memory(uint8_t code);
  protected:
    char    _num_buffer[KEY_CALCULATOR_NUM_BUFFER_SIZE] = {0};
    char    _mem_buffer[KEY_CALCULATOR_MEM_BUFFER_SIZE] = {0};
    uint8_t _num_buffer_index                           =  0;
    uint8_t _mem_buffer_index                           =  0;
    uint8_t _last_key                                   =  0;
    bool    _entering_memory                            = false;
    bool    _build_number(uint8_t code);
    bool    _push_number();
    String  _convert_num_buffer(bool clear);
};
