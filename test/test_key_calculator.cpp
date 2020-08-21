#include "test_main.h"


void test_key_add_1_plus_1() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("2", kcalc.get_display().c_str());
}


void test_key_memory() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
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
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('M')); // recall M[3]
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
  TEST_ASSERT_TRUE(kcalc.key('A'));   // the AC key
  TEST_ASSERT_EQUAL_STRING("0",   kcalc.get_display().c_str());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
  kcalc.recall_memory(3);
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("111", kcalc.get_display().c_str());
  TEST_ASSERT_TRUE(kcalc.key('A'));
  TEST_ASSERT_TRUE(kcalc.key('A')); // AA = clear all
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("0", kcalc.get_display().c_str());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("0", kcalc.get_display().c_str());   // Point of failure
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("0", kcalc.get_display().c_str());
}


void test_change_sign() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('`'));
  TEST_ASSERT_EQUAL_STRING("-123", kcalc.get_display().c_str());
  TEST_ASSERT_TRUE(kcalc.key('`'));
  TEST_ASSERT_EQUAL_STRING("123", kcalc.get_display().c_str());
}


void run_key_calculator_tests() {
  RUN_TEST(test_key_add_1_plus_1);
  RUN_TEST(test_key_memory);
  RUN_TEST(test_change_sign);
}
