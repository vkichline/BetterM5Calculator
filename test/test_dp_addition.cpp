#include "test_main.h"


void test_dp_add_1_plus_1() {
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, dpcalc.evaluate());
  TEST_ASSERT_EQUAL(2, dpcalc.peek_value());
  TEST_ASSERT_EQUAL(2, dpcalc.pop_value());
}


void test_dp_add_1_plus_1_plus_1() {
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, dpcalc.evaluate());
  TEST_ASSERT_EQUAL(3, dpcalc.peek_value());
  TEST_ASSERT_EQUAL(3, dpcalc.pop_value());
}


void test_dp_add_4_fractional() {
  TEST_ASSERT_TRUE(dpcalc.push_value(12.719));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(331.04));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(-211.211));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(0));
  TEST_ASSERT_EQUAL(NO_ERROR, dpcalc.evaluate());
  TEST_ASSERT_EQUAL(132.548,  dpcalc.pop_value());
}


void run_dp_addition_tests() {
  RUN_TEST(test_dp_add_1_plus_1);
  RUN_TEST(test_dp_add_1_plus_1_plus_1);
  RUN_TEST(test_dp_add_4_fractional);
}
