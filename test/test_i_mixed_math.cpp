#include "test_main.h"


// 12 + 330 / -5 - 6 * 3 =
// = 12 + (330 / -5) - (6 * 3) =
// = 12 - 66 - 18 =
// = -72
void test_i_all_operators() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(12));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(330));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('/'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(-5));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('-'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(6));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('*'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(3));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.evaluate_all());
  TEST_ASSERT_EQUAL_DOUBLE(-72, icalc.pop_value());
  TEST_ASSERT_EQUAL(0, icalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


// (10 + 12) / (1 + 1) = 11
void test_i_parens() {
  icalc.push_operator('(');
  icalc.push_value(10.0);
  icalc.push_operator('+');
  icalc.push_value(12.0);
  icalc.push_operator(')');
  icalc.push_operator('/');
  icalc.push_operator('(');
  icalc.push_value(1.0);
  icalc.push_operator('+');
  icalc.push_value(1.0);
  icalc.push_operator(')');
  icalc.evaluate_all();
  TEST_ASSERT_EQUAL_DOUBLE(11.0, icalc.pop_value());
  TEST_ASSERT_EQUAL(0, icalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


// 1 + 1 = (testing the = operator)
void test_i_evaluate() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('+'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(1));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('='));
  TEST_ASSERT_EQUAL(2, icalc.peek_value());
  TEST_ASSERT_EQUAL(2, icalc.pop_value());
  TEST_ASSERT_EQUAL(0, icalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


void test_i_square() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(25));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('s'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('='));
  TEST_ASSERT_EQUAL(625, icalc.pop_value());
  TEST_ASSERT_EQUAL(0, icalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


void test_i_square_root() {
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_value(25));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('r'));
  TEST_ASSERT_EQUAL(NO_ERROR, icalc.push_operator('='));
  TEST_ASSERT_EQUAL(5, icalc.pop_value());
  TEST_ASSERT_EQUAL(0, icalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, icalc.value_stack.size());
}


void run_i_mixed_math_tests() {
  RUN_TEST(test_i_all_operators);
  RUN_TEST(test_i_parens);
  RUN_TEST(test_i_evaluate);
  RUN_TEST(test_i_square);
  RUN_TEST(test_i_square_root);
}
