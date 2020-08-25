#pragma once

// This layer of the calculator interacts with a Calculator keyboard,
// where numbers are entered as individual digits. There are special
// modes for ongoing numeric entry and memory address entry.
//
// By Van Kichline
// In the year of the plague

#include "TextCalculator.h"

#define KEY_CALCULATOR_NUM_BUFFER_SIZE  64
#define KEY_CALCULATOR_MEM_BUFFER_SIZE   8


class KeyCalculator : public TextCalculator {
  public:
    KeyCalculator();
    bool    key(uint8_t code);                                    // Process a key from the keyboard
    bool    commit();                                             // If there is a value in the buffer, commit it to the stack
    String  get_display();                                        // Return the value being built, or the current value()
    void    set_display(String val);                              // Set the display value to val
    bool    is_building_memory(String* str);                      // Is a memory address being built? If so, return it in str (unless nullptr)
    bool    is_building_number();                                 // In numeric entry mode. BS can be used.
    String  get_status_display();                                 // Get display string for top of calc.
    String  get_operator_stack_display();                         // Get representation of op stack
    String  get_value_stack_display();                            // Get representation of value stack
  protected:
    char    _num_buffer[KEY_CALCULATOR_NUM_BUFFER_SIZE] = {0};    // Buffer for building a numeric entry
    char    _mem_buffer[KEY_CALCULATOR_MEM_BUFFER_SIZE] = {0};    // Buffer for building a memory address
    uint8_t _num_buffer_index                           =  0;     // Current position in _num_buffer
    uint8_t _mem_buffer_index                           =  0;     // Current position in mem_buffer
    uint8_t _previous_key                               =  0;     // The keypress before the current key
    bool    _entering_memory                            = false;  // True if M pressed and access incomplete
    bool    _handle_clear(bool all_clear = false);                // Handle the AC key, with 1st & 2nd press actions
    bool    _handle_change_sign();                                // Handle +/- key, which is an input action, not a command
    bool    _handle_memory_command(uint8_t code);                 // Handle the rather complicated memory commands
    bool    _build_number(uint8_t code);                          // Build the display value from keystrokes
    String  _convert_num_buffer(bool clear);                      // Convert the buffer to a String and clear it
    uint8_t _count_open_parens();                                 // Return the number of OPEN_PAREN operators on the operator_stack
};
