#include "test_main.h"


// 12.5 + 331.5 / -5 - 3.5 * 4.8 =
// = 12.5 + (331.5 / -5) - (3.5 * 4.8) =
// = 12.5 - 66.3 - 16.8 =
// = -70.6
void test_dp_all_operators() {
  TEST_ASSERT_TRUE(dpcalc.push_value(12.5));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(331.5));
  TEST_ASSERT_TRUE(dpcalc.push_operator('/'));
  TEST_ASSERT_TRUE(dpcalc.push_value(-5));
  TEST_ASSERT_TRUE(dpcalc.push_operator('-'));
  TEST_ASSERT_TRUE(dpcalc.push_value(3.5));
  TEST_ASSERT_TRUE(dpcalc.push_operator('*'));
  TEST_ASSERT_TRUE(dpcalc.push_value(4.8));
  TEST_ASSERT_EQUAL(NO_ERROR, dpcalc.evaluate());
  TEST_ASSERT_EQUAL_DOUBLE(-70.6, dpcalc.pop_value());
  TEST_ASSERT_EQUAL(0, dpcalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, dpcalc.value_stack.size());
}


// (10 + 12) / (1 + 1) = 11
void test_dp_parens() {
  dpcalc.push_operator('(');
  dpcalc.push_value(10.0);
  dpcalc.push_operator('+');
  dpcalc.push_value(12.0);
  dpcalc.push_operator(')');
  dpcalc.push_operator('/');
  dpcalc.push_operator('(');
  dpcalc.push_value(1.0);
  dpcalc.push_operator('+');
  dpcalc.push_value(1.0);
  dpcalc.push_operator(')');
  dpcalc.evaluate();
  TEST_ASSERT_EQUAL_DOUBLE(11.0, dpcalc.pop_value());
  TEST_ASSERT_EQUAL(0, dpcalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, dpcalc.value_stack.size());
}


// 1 + 1 = (testing the = operator)
void test_dp_evaluate() {
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_TRUE(dpcalc.push_operator('+'));
  TEST_ASSERT_TRUE(dpcalc.push_value(1));
  TEST_ASSERT_TRUE(dpcalc.push_operator('='));
  TEST_ASSERT_EQUAL(2, dpcalc.peek_value());
  TEST_ASSERT_EQUAL(2, dpcalc.pop_value());
  TEST_ASSERT_EQUAL(0, dpcalc.operator_stack.size());
  TEST_ASSERT_EQUAL(0, dpcalc.value_stack.size());
}


void run_dp_mixed_math_tests() {
  RUN_TEST(test_dp_all_operators);
  RUN_TEST(test_dp_parens);
  RUN_TEST(test_dp_evaluate);
}
