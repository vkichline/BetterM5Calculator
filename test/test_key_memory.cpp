#include "test_main.h"


void test_key_memory_assign_M() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key('0'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('='));   // assign 100 to M, leave 100 in value
  TEST_ASSERT_EQUAL_STRING("100", k_display());
  TEST_ASSERT_TRUE(kcalc.key('A'));   // clear
  TEST_ASSERT_EQUAL_STRING("0", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));   // recall
  TEST_ASSERT_EQUAL_STRING("100", k_display());
}


void test_key_memory_M() {
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('.'));
  TEST_ASSERT_TRUE(kcalc.key('5'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("3.5", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('='));   // assign 3.30 to M, leave 3.30 in value
  TEST_ASSERT_TRUE(kcalc.key('A'));
  TEST_ASSERT_EQUAL_STRING("0", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("3.5", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('A'));   // clear memory, not display
  TEST_ASSERT_EQUAL_STRING("3.5", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));   // recall memory
  TEST_ASSERT_EQUAL_STRING("0", k_display());
}


void test_key_memory_M34() {
  TEST_ASSERT_TRUE(kcalc.key('8'));
  TEST_ASSERT_TRUE(kcalc.key('.'));
  TEST_ASSERT_TRUE(kcalc.key('5'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("8.5", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('4'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_TRUE(kcalc.key('A'));
  TEST_ASSERT_EQUAL_STRING("0", k_display());
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('4'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("8.5", k_display());
}


void run_key_memory_tests() {
  RUN_TEST(test_key_memory_assign_M);
  RUN_TEST(test_key_memory_M);
  RUN_TEST(test_key_memory_M34);
}
