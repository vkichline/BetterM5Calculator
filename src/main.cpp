#include <M5Stack.h>
#include <TextCalculator.h>

CoreCalculator<double>        fcalc;
CoreCalculator<int32_t>       icalc;
MemoryCalculator<double, 10>  memcalc;
TextCalculator                tcalc;


void setup() {
  M5.begin();
  fcalc.push_value(2);
  fcalc.push_operator('+');
  fcalc.push_value(3);
  fcalc.push_operator('/');
  fcalc.push_value(2);
  fcalc.push_operator('-');
  fcalc.push_value(1);
  fcalc.evaluate();
  Serial.printf("CoreCalculator<double>:  2 + 3 / 2 - 1 = %.1f\n", fcalc.get_value());

  icalc.push_value(2);
  icalc.push_operator('+');
  icalc.push_value(3);
  icalc.push_operator('/');
  icalc.push_value(2);
  icalc.push_operator('-');
  icalc.push_value(1);
  icalc.evaluate();
  Serial.printf("CoreCalculator<int32_t>: 2 + 3 / 2 - 1 = %d\n", icalc.get_value());

  memcalc.set_memory(100);
  memcalc.push_value(memcalc.get_memory());
  memcalc.push_operator('+');
  memcalc.push_value(memcalc.get_memory());
  memcalc.evaluate();
  Serial.printf("MemoryCalculator<double, 10>: M = 100, M + M = %.1f\n", memcalc.get_value());

  tcalc.enter("1");
  tcalc.enter('+');
  tcalc.enter("1");
  tcalc.total();
  Serial.printf("TextCalculator: \"1\" + \"1\" = \"%s\"\n", tcalc.value().c_str());
  
  tcalc.parse("1 + 5 / 3.2 * 7.3167 - 8 * 33.33 =");
  Serial.printf("\"1 + 5 / 3.2 * 7.3167 - 8 * 33.33 =\" = \"%s\"\n", tcalc.value().c_str());
}


void loop() {
  M5.update();
  delay(100);
}
