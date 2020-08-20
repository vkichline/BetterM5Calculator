#include "test_main.h"


void test_key_memory_M() {
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('.'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("3.30", kcalc.value().c_str());
  kcalc.copy_to_memory(); // BUGBUG: use M keys:
  TEST_ASSERT_TRUE(kcalc.key('A'));
  TEST_ASSERT_EQUAL_STRING("0.00", kcalc.value().c_str());

  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("3.30", kcalc.value().c_str());
}


void test_key_memory_M10() {
  TEST_ASSERT_TRUE(kcalc.key('8'));
  TEST_ASSERT_TRUE(kcalc.key('.'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("8.10", kcalc.value().c_str());
  kcalc.copy_to_memory(34); // BUGBUG: use M keys:
  TEST_ASSERT_TRUE(kcalc.key('A'));
  TEST_ASSERT_EQUAL_STRING("0.00", kcalc.value().c_str());

  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('4'));
  TEST_ASSERT_TRUE(kcalc.key('M'));
  TEST_ASSERT_EQUAL_STRING("8.10", kcalc.value().c_str());
}


void run_key_memory_tests() {
  RUN_TEST(test_key_memory_M);
  RUN_TEST(test_key_memory_M10);
}
