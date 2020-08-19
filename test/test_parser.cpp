#include "test_main.h"

void test_parser_1() {
  TEST_ASSERT_TRUE(tcalc.parse("1 + 5 / 3.2 * 7.3167 - 8 * 33.33 ="));
  TEST_ASSERT_EQUAL_STRING("-254.21", tcalc.value().c_str());
}

void run_parser_tests() {
  RUN_TEST(test_parser_1);  // danger: this test assumes its being run before any memory functions
}
