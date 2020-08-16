#include "test_main.h"


MemoryCalculator<double,  NUM_MEMORIES>  dpcalc;
MemoryCalculator<int32_t, NUM_MEMORIES>  icalc;


void setUp(void) {}   // set stuff up here
void tearDown() {}    // clean stuff up here


void setup() {
  // Note: Wait for > 2 secs if board doesn't support software reset via Serial.DTR/RTS
  // delay(2000);
  UNITY_BEGIN();  // start unit testing
}

void loop() {
  run_i_push_value_tests();
  run_i_addition_tests();
  run_i_mixed_math_tests();
  run_i_memory_tests();

  run_dp_push_value_tests();
  run_dp_addition_tests();
  run_dp_mixed_math_tests();
  run_dp_memory_tests();

  UNITY_END();  // stop unit testing
}
