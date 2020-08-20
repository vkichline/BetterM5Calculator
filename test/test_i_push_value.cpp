#include "test_main.h"


void test_i_push_value() {
  int32_t result;
  TEST_ASSERT_TRUE(icalc.push_value(123));
  result = icalc.peek_value();
  TEST_ASSERT_EQUAL(123, result);
  TEST_ASSERT_EQUAL(123, icalc.get_value());
  result = icalc.pop_value();
  TEST_ASSERT_EQUAL(123, result);
  TEST_ASSERT_EQUAL(0, icalc.get_value());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


void test_i_push_3() {
  icalc.push_value(1);
  TEST_ASSERT_EQUAL(1, icalc.get_value());
  TEST_ASSERT_EQUAL(1, icalc.value_stack.size());
  icalc.push_value(2);
  TEST_ASSERT_EQUAL(2, icalc.get_value());
  TEST_ASSERT_EQUAL(2, icalc.value_stack.size());
  icalc.push_value(3);
  TEST_ASSERT_EQUAL(3, icalc.get_value());
  TEST_ASSERT_EQUAL(3, icalc.value_stack.size());

  TEST_ASSERT_EQUAL(3, icalc.pop_value());
  TEST_ASSERT_EQUAL(2, icalc.pop_value());
  TEST_ASSERT_EQUAL(1, icalc.pop_value());
  TEST_ASSERT_EQUAL(0, icalc.get_value());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


void run_i_push_value_tests() {
  RUN_TEST(test_i_push_value);
  RUN_TEST(test_i_push_3);
}
