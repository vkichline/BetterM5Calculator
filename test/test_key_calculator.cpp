#include "test_main.h"

void test_key_add_1_plus_1() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("2", k_display());
}


void test_key_memory() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('=')); // Save to M
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('=')); // Save to M[3]
  kcalc.push();
  kcalc.push();
  kcalc.push();
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M')); // recall M
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('M')); // recall M[3]
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  TEST_ASSERT_TRUE(kcalc.key('A'));   // the AC key
  TEST_ASSERT_EQUAL_STRING("0",   k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('M'));   // recall memory 3
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("111", k_display());
  TEST_ASSERT_TRUE(kcalc.key('A'));
  TEST_ASSERT_TRUE(kcalc.key('A')); // AA = clear all
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("0", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("0", k_display());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("0", k_display());
}


void test_key_change_sign() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('`'));
  TEST_ASSERT_EQUAL_STRING("-123", k_display());
  TEST_ASSERT_TRUE(kcalc.key('`'));
  TEST_ASSERT_EQUAL_STRING("123", k_display());
}


void test_key_backspace() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('3'));   // 123
  TEST_ASSERT_TRUE(kcalc.key('B'));   // 12
  TEST_ASSERT_TRUE(kcalc.key('B'));   // 1
  TEST_ASSERT_TRUE(kcalc.key('1'));   // 11
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('1'));   // M1
  TEST_ASSERT_TRUE(kcalc.key('1'));   // M11
  TEST_ASSERT_TRUE(kcalc.key('B'));   // M1
  TEST_ASSERT_TRUE(kcalc.key('0'));   // M10
  TEST_ASSERT_TRUE(kcalc.key('='));   // assign
  TEST_ASSERT_TRUE(kcalc.key('A'));   // clear
  TEST_ASSERT_EQUAL_STRING("0", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('0'));   // M10
  TEST_ASSERT_TRUE(kcalc.key('M'));   // recall
  TEST_ASSERT_EQUAL_STRING("11", k_display());
}


void run_key_calculator_tests() {
  RUN_TEST(test_key_add_1_plus_1);
  RUN_TEST(test_key_memory);
  RUN_TEST(test_key_change_sign);
  RUN_TEST(test_key_backspace);
}
