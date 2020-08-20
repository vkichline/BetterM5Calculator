#include "test_main.h"


void test_key_add_1_plus_1() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('+'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("2.00", kcalc.value().c_str());
}


void test_key_memory() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('='));
  TEST_ASSERT_EQUAL_STRING("111.00", kcalc.value().c_str());
  kcalc.copy_to_memory();
  kcalc.copy_to_memory(3);
  kcalc.push();
  kcalc.push();
  kcalc.push();
  kcalc.recall_memory();
  TEST_ASSERT_EQUAL_STRING("111.00", kcalc.value().c_str());
  kcalc.recall_memory(3);
  TEST_ASSERT_EQUAL_STRING("111.00", kcalc.value().c_str());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("111.00", kcalc.value().c_str());
  TEST_ASSERT_TRUE(kcalc.key('A'));   // the AC key
  kcalc.recall_memory();
  TEST_ASSERT_EQUAL_STRING("0.00",   kcalc.value().c_str());
  kcalc.recall_memory(3);
  TEST_ASSERT_EQUAL_STRING("111.00", kcalc.value().c_str());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("111.00", kcalc.value().c_str());
  TEST_ASSERT_TRUE(kcalc.key('A'));   // the AC key a second time
  kcalc.recall_memory();
  TEST_ASSERT_EQUAL_STRING("0.00", kcalc.value().c_str());
  kcalc.recall_memory(3);
  TEST_ASSERT_EQUAL_STRING("0.00", kcalc.value().c_str());
  kcalc.pop();
  TEST_ASSERT_EQUAL_STRING("0.00", kcalc.value().c_str());
}


void test_change_sign() {
  TEST_ASSERT_TRUE(kcalc.key('1'));
  TEST_ASSERT_TRUE(kcalc.key('2'));
  TEST_ASSERT_TRUE(kcalc.key('3'));
  TEST_ASSERT_TRUE(kcalc.key('`'));
  TEST_ASSERT_EQUAL_STRING("-123.00", kcalc.value().c_str());
  TEST_ASSERT_TRUE(kcalc.key('`'));
  TEST_ASSERT_EQUAL_STRING("123.00", kcalc.value().c_str());
}


void run_key_calculator_tests() {
  RUN_TEST(test_key_add_1_plus_1);
  RUN_TEST(test_key_memory);
  RUN_TEST(test_change_sign);
}
