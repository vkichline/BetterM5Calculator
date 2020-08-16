#include "test_main.h"

void test_i_is_memory_clear() {
  TEST_ASSERT_EQUAL(0.0, icalc.get_memory());
  for(uint8_t i = 0; i < NUM_MEMORIES; i++) {
    TEST_ASSERT_EQUAL(0.0, icalc.get_memory(i));
  }
  TEST_ASSERT_EQUAL(0, icalc.get_memory_depth());
}

void test_i_memory_recall() {
  icalc.clear_all_memory();
  icalc.set_memory(-123.456);
  icalc.set_memory(6, 3.1415976);
  icalc.push_memory(42);

  TEST_ASSERT_EQUAL(-123.456,   icalc.get_memory());
  TEST_ASSERT_EQUAL(3.1415976,  icalc.get_memory(6));
  TEST_ASSERT_EQUAL(1,          icalc.get_memory_depth());
  TEST_ASSERT_EQUAL(42,         icalc.pop_memory());
  TEST_ASSERT_EQUAL(0,          icalc.get_memory_depth());
}


void run_i_memory_tests() {
  RUN_TEST(test_i_is_memory_clear);  // danger: this test assumes its being run before any memory functions
  RUN_TEST(test_i_memory_recall);
  icalc.clear_all_memory();
  RUN_TEST(test_i_is_memory_clear);
  icalc.clear_all_memory();          // clean up
}
