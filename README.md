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

## How to Calculate

This calculator works like you would expect, with two notable additions; the 3 M5 buttons and Memory.  
The buttons rotate their captions as you press Button C (right arrow), eventually starting over again.\nWhen entering
a number, a special set of buttons is displayed, permitting you to backspace or cancel number input. If you want to
get back to the ordinary buttons, just press Button C (right arrow).\nWhen inputting memory, a special set of buttons is
shown with shortcuts for get, set, and clear.\nThe buttons will become easy to navigate once you use them a bit.  
Memory is very powerful on the M5 Calculator. You get one 'simple' memory, 100 numbered memories (0-99), and an
unlimited push-down memory stack. Once you press the keyboard's 'M' key, you are in Memory Mode, and the next key is
interpreted specially:

* `MM` :   Recalls simple memory
* `M=` :  Stores simple memory
* `MA` :  Clears simple memory
* `M+` :   Adds to simple memory
* `M-` :   Subtracts from simple memory
* `M*` :   Multiplies simple memory
* `M%` :   Takes percent of simple memory
* `M.` :   Cancels memory mode
* `M3M` :  Recalls M[3]
* `M99+` : Adds to M[99]

The AC key clears the current value. Pressing AC twice in a row clears all memory as well.  
As with many calculators, you must use the +/- button to enter a negative number. Press +/- anytime that you are
in number entry mode, and the leading '-' sign will appear or disappear.  
The display line just above the buttons shows the operator stack on the left, and the value stack on the right. You
may find it interesting to see how the calculator calculates, but it's mainly there to reveal invisible bugs  
The Status is displayed above the main value. First, the state value of the KeyCalculator is displayed:

* `A` :  Ready for Any input
* `N` :  Ready for Numeric input
* `O` :  Ready for Operator input
* `>` : Currently building a number in the display
* `M` :  In "Memory Mode", building a memory address
* `X` :  In global error mode

If any numbered memories are in use, up to eight
indexes will be shown, as in M[3,7,19]. If more than eight are in use, the indexes will be followed by '...'.  
Next, if the memory stack has any values on it, the status display will show S(n), where n is the depth of the stack.  
Finally, if simple memory is set, that status display will show M=nnn, where nnn is the value of memory.

## Design

The calculator is built with strict separation of concerns.

* The lowest level, the CoreCalculator, is a state-free stack evaluator, designed as a class template so that any numeric type can be used for computation.
* The next level, the MemoryCalculator, is another class template which add memories and memory operations.
* The TextCalculator is a concrete class, currently aggregating a single double-precision CoreCalculator, adding textual conversion needed for UI.
* The KeyCalculator wraps the TextCalculator in an object that handles input one character at a time from a keyboard.
* And finally, the files in src create an M5Stack-aware KeyCalculator and provide display and extra input options.

### `CoreCalculator<T>`

This template takes a single typename parameter and creates a basic, no-frills calculator engine with a value stack, an operand stack, and and evaluator. It includes functions for manipulating the two stacks, evaluating
the stacks, and processing the Global Error State.  Currently, only divide by zero errors are handled; overflow and underflow are planned.  
Operators are implemented as objects which are added to an _operator array. You can remove, replace, or add additional operators derived from `Operator<T>`, whose tye must match the calculator's type.  
Type-specific operators (for example, operators that work only on integers or on floating-point numbers) can be added in type-specific calculators derived from this template.

### `MemoryCalculator<T, M>`

MemoryCalculator adds a "simple" memory, and array of M indexed memories (ste to 100 in this example), and a memory stack limited only by RAM. Memories must match the data type of the CoreCalculator.  
By keeping memory operations out of the CoreCalculator, and calculations out of the MemoryCalculator implementations, they're much simpler and more cohesive.

### `TextCalculator`

Ultimately the calculator must use human-readable data. This layer converts numbers to text and back.  Concepts such as number base (binary, octal, decimal, hexadecimal) belong in this layer,
as do trigonometric modes (degree, radian, rads) but are not yet implemented at this point.  
This layer includes an extremely simple parser. This can be leveraged for simplifying test creation, or for use in other programs. It's not used in the calculator.

### `KeyCalculator`

The KeyCalculator is a state-driven processor for keystrokes. While operators are generally one keystroke, numbers and memory addresses must be composed. Special keys like AC may have special semantics.
This layer of the engine actively rejects keys it sees as inappropriate (like a close parentheses when there's no open parentheses) in order to keep errors from occurring. Ideally, only valid key combinations would
be accepted. Utility routines are provided so that hosts can easily provide complete and accurate state information.

### `M5Calculator`

Finally, a small program is wrapped around the KeyCalculator which interacts with the M5Stack computer, using its buttons and screen as well as the calculator keyboard extension.  
A status display is supplied at the top of the screen, followed by a value display, an area used to display memory selections of error message, and a view of the operator and value stacks.  Under the screen display are button labels for the A, B and C buttons, whose labels change based on state and user selection.  The menu selection, in particular, allows access to settings and additional functions.

---

Everything up through the memory calculator should be pretty portable. One could make a command line calculator, for example, or add expression parsing to an existing program.  
More layers can easily be inserted in this design or added on top.  A fully programmable calculator is left as an exercise for the reader.
