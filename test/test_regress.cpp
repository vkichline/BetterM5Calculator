#include "test_main.h"


// Simply entering 9.0 would result in a display of 09.9
//
void test_number_format_bug_1() {
  TEST_ASSERT_TRUE(tcalc.enter("9.9"));
  TEST_ASSERT_EQUAL_STRING("9.9", tcalc.value().c_str());
}


void run_regression_tests() {
  RUN_TEST(test_number_format_bug_1);
}
