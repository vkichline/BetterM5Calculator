#include "test_main.h"


void test_i_add_1_plus_1() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.evaluate_all());
  TEST_ASSERT_EQUAL(2, icalc.peek_value());
  TEST_ASSERT_EQUAL(2, icalc.pop_value());
}


void test_i_add_1_plus_1_plus_1() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.evaluate_all());
  TEST_ASSERT_EQUAL(3, icalc.peek_value());
  TEST_ASSERT_EQUAL(3, icalc.pop_value());
}


void test_i_add_4() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(12));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(331));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(0));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(-211));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.evaluate_all());
  TEST_ASSERT_EQUAL(132,  icalc.pop_value());
}


void run_i_addition_tests() {
  RUN_TEST(test_i_add_1_plus_1);
  RUN_TEST(test_i_add_1_plus_1_plus_1);
  RUN_TEST(test_i_add_4);
}
