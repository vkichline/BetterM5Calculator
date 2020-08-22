#include "test_main.h"

// Chaining is an obscure feature, but one really misses it if it's broken.
// Pressing 1 + = + = + = re-enters the value each time (in special-case code in KeyCalculator::key()).

void test_key_chain_plus() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("8", kcalc.get_display().c_str());
}


void test_key_chain_mixed() {
  TEST_ASSERT_TRUE(kcalc.key('8'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_TRUE(kcalc.key('*'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_TRUE(kcalc.key('/'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_TRUE(kcalc.key('-'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("0", kcalc.get_display().c_str());
}


void run_key_chaining_tests() {
  RUN_TEST(test_key_chain_plus);
  RUN_TEST(test_key_chain_mixed);
}
