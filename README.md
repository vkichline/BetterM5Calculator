# BetterM5Calculator

An object oriented calculator for the M5Stack with Calculator Keyboard.  
Under Construction, not complete but may be usable. Let your curiosity be your guide.

## Goal

I purchased the [M5STACK Faces Kit Pocket Computer with Keyboard/Game/Calculator](https://m5stack.com/collections/m5-core/products/face) and really liked the Calculator Keyboard.
I loaded the [Arduino sample program](https://github.com/m5stack/M5-ProductExampleCodes/blob/master/Module/CALCULATOR/CALCULATOR.ino) and saw that it demonstrated how to use the
keyboard, without doing much to implement an actual calculator.  
A look through the forums revealed that others had looked for a calculator program and had failed to achieve much satisfaction. So I wrote a very simple calculator program in a day:
[M5Calc-Arduino](https://github.com/vkichline/M5Calc-Arduino). This looks like a calculator, and acts sort of like one, but as the readme says, it's simplified.

I decided to write a real calculator program for the M5Stack; one which would be actually usable, expose some advantages of having a powerful processor to work with, and be
extensible. The foundation layer I'm working on here does only double-precision floating-point math in base 10 and has no trigonometric functions. However, it's designed to be capable
of growing into an excellent calculator, even with the simple keyboard, using UI enhancements. A programable layer with a web interface would be interesting, too.

## Architecture

### CoreCalculator<T>

### MemoryCalculator<T, M>

### TextCalculator

### KeyCalculator

### M5Calculator