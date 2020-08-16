#include "test_main.h"



void test_dp_push_value(void) {
  double result;
  dpcalc.push_value(123.456);
  result = dpcalc.peek_value();
  TEST_ASSERT_EQUAL(123.456, result);
  TEST_ASSERT_EQUAL(123.456, dpcalc.get_value());
  result = dpcalc.pop_value();
  TEST_ASSERT_EQUAL(123.456, result);
  TEST_ASSERT_EQUAL(0, dpcalc.get_value());
  TEST_ASSERT_EQUAL(0, dpcalc.value_stack.size());
}


void test_dp_push_3(void) {
  dpcalc.push_value(1);
  TEST_ASSERT_EQUAL(1, dpcalc.get_value());
  TEST_ASSERT_EQUAL(1, dpcalc.value_stack.size());
  dpcalc.push_value(2);
  TEST_ASSERT_EQUAL(2, dpcalc.get_value());
  TEST_ASSERT_EQUAL(2, dpcalc.value_stack.size());
  dpcalc.push_value(3);
  TEST_ASSERT_EQUAL(3, dpcalc.get_value());
  TEST_ASSERT_EQUAL(3, dpcalc.value_stack.size());

  TEST_ASSERT_EQUAL(3, dpcalc.pop_value());
  TEST_ASSERT_EQUAL(2, dpcalc.pop_value());
  TEST_ASSERT_EQUAL(1, dpcalc.pop_value());
  TEST_ASSERT_EQUAL(0, dpcalc.get_value());
  TEST_ASSERT_EQUAL(0, dpcalc.value_stack.size());
}



void run_dp_push_value_tests() {
  RUN_TEST(test_dp_push_value);
  RUN_TEST(test_dp_push_3);
}
