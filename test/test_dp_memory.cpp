#include "test_main.h"


void test_dp_is_memory_clear() {
  TEST_ASSERT_EQUAL(0.0, dpcalc.get_memory());
  for(uint8_t i = 0; i < NUM_MEMORIES; i++) {
    TEST_ASSERT_EQUAL(0.0, dpcalc.get_memory(i));
  }
  TEST_ASSERT_EQUAL(0, dpcalc.get_memory_depth());
}


void test_dp_memory_recall() {
  dpcalc.clear_all_memory();
  dpcalc.set_memory(-123.456);
  dpcalc.set_memory(6, 3.1415976);
  dpcalc.push_memory(42);
  
  TEST_ASSERT_EQUAL(-123.456,   dpcalc.get_memory());
  TEST_ASSERT_EQUAL(3.1415976,  dpcalc.get_memory(6));
  TEST_ASSERT_EQUAL(1,          dpcalc.get_memory_depth());
  TEST_ASSERT_EQUAL(42,         dpcalc.pop_memory());
  TEST_ASSERT_EQUAL(0,          dpcalc.get_memory_depth());
}


void run_dp_memory_tests() {
  RUN_TEST(test_dp_is_memory_clear);  // danger: this test assumes its being run before any memory functions
  RUN_TEST(test_dp_memory_recall);
  dpcalc.clear_all_memory();
  RUN_TEST(test_dp_is_memory_clear);
  dpcalc.clear_all_memory();          // clean up
}
