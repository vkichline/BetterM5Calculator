#include "test_main.h"


// Simply entering 9.0 would result in a display of 09.9
//
void test_number_format_bug_1() {
  TEST_ASSERT_TRUE(tcalc.enter("9.9"));
  TEST_ASSERT_EQUAL_STRING("9.9", tcalc.value().c_str());
}


// Corrections to the 9.9 bug resulted in crashes with small conversions
//
void test_number_format_bug_2() {
  TEST_ASSERT_TRUE(tcalc.enter("0.1"));
  TEST_ASSERT_EQUAL_STRING("0.1", tcalc.value().c_str());
}


// Change the operator from + to * (intentionally) by typing:
// 10 + * 2 =
// Expect 20.
//
void test_change_operator() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('*'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("20", k_display());
}


// Make sure the value and operand stack are empty after doing add percent operation
//
void test_add_percent_op_stack() {
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('5'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key('%'));
  TEST_ASSERT_EQUAL_STRING("4.5", k_display());
}


// Make sure PERCENT_OPERATOR, SQUARE_OPERATOR & SQUARE_ROOT_OPERATOR needs no =
//
void test_immediate_operators() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key('%'));
  TEST_ASSERT_EQUAL_STRING("0.1", k_display());
  TEST_ASSERT_TRUE(kcalc.key('5'));
  TEST_ASSERT_TRUE(kcalc.key(SQUARE_OPERATOR));
  TEST_ASSERT_EQUAL_STRING("25",k_display());
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key(SQUARE_ROOT_OPERATOR));
  TEST_ASSERT_EQUAL_STRING("10", k_display());
}


// Make sure stacked parens work, stacking left
//
void test_stacked_left_parens() {
  TEST_ASSERT_TRUE(kcalc.key('('));
  TEST_ASSERT_TRUE(kcalc.key('('));
  TEST_ASSERT_TRUE(kcalc.key('5'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key(')'));
  TEST_ASSERT_TRUE(kcalc.key('*'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key(')'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("18", k_display());
}


// Make sure stacked parens work, stacking right
//
void test_stacked_right_parens() {
  TEST_ASSERT_TRUE(kcalc.key('('));
  TEST_ASSERT_TRUE(kcalc.key('5'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('*'));
  TEST_ASSERT_TRUE(kcalc.key('('));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('8'));
  TEST_ASSERT_TRUE(kcalc.key(')'));
  TEST_ASSERT_TRUE(kcalc.key(')'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("38", k_display());
}


void run_regression_tests() {
  RUN_TEST(test_number_format_bug_1);
  RUN_TEST(test_number_format_bug_2);
  RUN_TEST(test_change_operator);
  RUN_TEST(test_add_percent_op_stack);
  RUN_TEST(test_immediate_operators);
  RUN_TEST(test_stacked_left_parens);
  RUN_TEST(test_stacked_right_parens);
}
