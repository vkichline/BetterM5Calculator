#include "test_main.h"

// there were several problems with double_to_string(). Make sure they don't regress.


void enter_number_and_verify(const char* entry) {
  for(int i = 0; i < strlen(entry); i++) {
    TEST_ASSERT_TRUE(kcalc.key(entry[i]));  // enter whatever was passed in
  }
  TEST_ASSERT_TRUE(kcalc.key('='));         // enter into memory
  TEST_ASSERT_EQUAL_STRING(entry, k_display());
  TEST_ASSERT_TRUE(kcalc.key('A'));         // clear memory
}


// the KeyCalculator doesn't take a negative prefix, it uses the +/- operator (`)
void enter_negative_number_and_verify(const char* entry) {
  assert('-' == entry[0]);
  for(int i = 1; i < strlen(entry); i++) {
    TEST_ASSERT_TRUE(kcalc.key(entry[i]));  // enter whatever was passed in
  }
  TEST_ASSERT_TRUE(kcalc.key('='));         // enter into memory
  TEST_ASSERT_TRUE(kcalc.key('`'));         // change sign
  TEST_ASSERT_EQUAL_STRING(entry, k_display());
  TEST_ASSERT_TRUE(kcalc.key('A'));         // clear memory
}


void test_key_numbers() {
  enter_number_and_verify("0");
  enter_number_and_verify("10000");
  enter_number_and_verify("0.1");
  enter_number_and_verify("0.0000001");
  enter_number_and_verify("1000.1");
  enter_number_and_verify("0.9999999");
}


void test_key_negative_numbers() {
  enter_negative_number_and_verify("-1");
  enter_negative_number_and_verify("-100");
  enter_negative_number_and_verify("-0.1");
  enter_negative_number_and_verify("-0.0000001");
  enter_negative_number_and_verify("-999999");
  enter_negative_number_and_verify("-0.9999999");
}


void run_key_number_tests() {
  RUN_TEST(test_key_numbers);
  RUN_TEST(test_key_negative_numbers);
}
