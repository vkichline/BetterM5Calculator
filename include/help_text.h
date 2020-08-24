// For the help textBox

#define HELP_TEXT "Welcome to the M5 Calculator\n\n" \
"This calculator works like you would expect, with two notable additions; the 3 M5 buttons and Memory.\n\n" \
"The buttons rotate their captions as you press Button C (right arrow), eventually starting over again.\nWhen entering " \
"a number, a special set of buttons is displayed, permitting you to backspace or cancel number input. If you want to " \
"get back to the ordinary buttons, just press Button C (right arrow).\nWhen inputting memory, a special set of buttons is " \
"shown with shortcuts for get, set, and clear.\nThe buttons will become easy to navigate once you use them a bit.\n\n" \
"Memory is very powerful on the M5 Calculator. You get one 'simple' memory, 100 numbered memories (0-99), and an " \
"unlimited push-down memory stack. Once you press the keyboard's 'M' key, you are in Memory Mode, and the next key is " \
"interprested specially:\n" \
"M     Recalls simple memory\n" \
"=      Stores simple memory\n" \
"AC   Clears simple memory\n" \
"+      Adds to simple memory\n" \
"-      Subtracts from simple memory\n" \
"*      Multiplies simple memory\n" \
"%    Takes percent of simple memory\n" \
".      Cancels memory mode\n" \
"3M   Recalls M[3]\n" \
"99+ Adds to M[99]\n\n" \
"The AC key clears the current value. Pressing AC twice in a row clears all memory as well.\n\n" \
"As with many calculators, you must use the +/- button to enter a negative number. Press +/- anytime that you are " \
"in number entry mode, and the leading '-' sign will appear or disappear.\n\n" \
"The display line just above the buttons shows the operator stack on the left, and the value stack on the right. You " \
"may find it interesting to see how the calculator calculates, but it's mainly there to reveal invisible bugs.\n\n" \
"When memory is in use, it will be displayed above the main value. First, if any numbered memories are in use, up to eight " \
"indexes will be shown, as in M[3,7,19]. If more than eight are in use, the indexes will be followed by '...'.\n" \
"Next, if the memory stack has any values on it, the status display will show S(n), where n is the depth of the stack.\n" \
"Finally, if simple memory is set, that status display will show M=nnn, where nnn is the value of memory.\n"
