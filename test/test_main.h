#include <M5Stack.h>
#include <unity.h>
#include <MemoryCalculator.h>
#include <TextCalculator.h>
#include <KeyCalculator.h>

#define NUM_MEMORIES    100

extern MemoryCalculator<double,  NUM_MEMORIES>  dpcalc;
extern MemoryCalculator<int32_t, NUM_MEMORIES>  icalc;
extern TextCalculator                           tcalc;
extern KeyCalculator                            kcalc;



void run_i_push_value_tests();      // Tests from test_i_push_value.cpp
void run_i_addition_tests();        // Tests from test_i_addition.cpp
void run_i_mixed_math_tests();      // Tests from test_i_mixed_math.cpp
void run_i_memory_tests();          // Tests from test_i_memory.cpp
void run_dp_push_value_tests();     // Tests from test_dp_push_value.cpp
void run_dp_addition_tests();       // Tests from test_dp_addition.cpp
void run_dp_mixed_math_tests();     // Tests from test_dp_mixed_math.cpp
void run_dp_memory_tests();         // Tests from test_dp_memory.cpp
void run_parser_tests();            // Tests from test_parser.cpp
void run_key_calculator_tests();    // Tests from test_key_calculator.cpp
void run_key_memory_tests();        // Tests from test_key_memory.cpp
void run_key_number_tests();        // Tests from test_key_numbers.cpp
void run_key_chaining_tests();      // Tests from test_key_chaining.cpp

// Don't forget to add your new test to test_main.cpp's loop() function. Again.
