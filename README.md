# waiw

*waiw* (**W**hen **A**m **I** **W**here, pronounced like Owen Wilson saying
"wow") is an esoteric programming language designed to function as obtusely and
read as ridiculously as possible.  It source code appears like a demented hybrid
between obfuscated English, bad ASCII art, and something a hacker might type
into a terminal in a bad 90s action movie that was released straight to home
video.  

I designed this language and wrote the interpreter for it in about 4 to 5 hours
total so don't expect a high degree of quality in this language.

This document details to how to install, run, and write this programming
language. I have also included several example programs in the `examples`
directory.

## Table of Contents

- [Installation](#install)
- [Running Waiw](#running)
- [Program Execution](#exec)
- [Operator Reference](#operators)

## <a name="install"> Install

### Option 1: Use Prebuilt Release

If you are on Windows, you can just download a prebuilt version of *waiw* from
the Releases tab -- just use the latest release.

### Option 2: Build from Source

In order to build *waiw*, you will first need to clone or download this
repository locally.

Then, set up and install `cmake` as well as the appropriate C++ build tool for
your platform (Visual C++, `make`, etc.)  Note that this program was built with
C++ 17 but should work as far back as C++ 11.

Before you begin building the program, note that because this program was built
with Visual C++, `interpreter.h` takes advantage of `#pragma once` which may not
be available in all versions of C++.

To build on all platforms, start by running the command below inside the `waiw`
directory:

    cmake -S . -B out

Then, if you are on Windows, run 

    MSBuild out/waiw.sln

If you are on a Unix-based platform, run

    make -f out/Makefile

## <a name="running"> Running Waiw

To run a *waiw* program, call the `waiw` executable with the path to the file
you want to run.

    path/to/waiw path/to/file.waiw

Conventionally, *waiw* program files end with the `.waiw` extension, but they do
not have to.

*Note: On Windows, the executable has the `.exe` extension.*

## <a name="exec"> Program Execution

### The Instruction Pointer

*waiw* programs are executed on a 2D grid of cells which are the characters of
the program (where each newline denotes moving down 1 on the y-axis).  The
**instruction pointer** is used to determine which cell is executed as any given
point in time.  The instruction pointer starts at `(0, 0)` (line 1, col 1 -- top
left corner) and moves right -- thereby reading the program left to right.

At each cell it arrives on (including the cell it starts on), the instruction
pointer evaluates that cell.  If the instruction pointer moves out of bounds,
the program exits as soon as the out of bounds cell is evaluated (no exception
is created in this case).  Note that newlines the separate the different y
coordinates of the cells are trimmed off when the program is loaded and so won't
be counted as in bounds cells.

Whenever the instruction pointer encounters the `!` operator (unescaped and
unignored), it exits the program.

### The Value Accumulator

Each cell can either contain text or an operator.  If the cell contains text,
the value of that cell is stored in the **value accumulator** which is a
temporary register used as an operand to many operators.  Whenever the value
accumulator is updated, the value that is placed into it is printed to the
console.  The value accumulator stores the text value of the cell as a string of
length 1.  By default, the value accumulator is *overwritten* with the new value
(it is not a stack).

Using this logic alone, when can write a simple *Hello world* program.  Below
is the program's source code. 

    Hello world

As you can see this program is trivial to write in *waiw*.  Because none of the
characters of `Hello world` (including the space) are considered operators, the
instruction pointer simply moves left to right starting at the `H` and ending
when it exits the bounds of the program.  At each character, it stores that
characters value in the value accumulator and prints that character. 

You can "ignore" blocks of text using `[]` (which are considered operators). Any
text inside those brackets will not be treated as if it does not exist.  So the
program below:

    Hello [cool ]world

prints the same message as the first *Hello world* program.

You can escape characters using the `\` operator which causes the operator that
follows it to interpreted as plain text (and will work inside `[]`).

    Hello [\[very\] cool ]world

The program once again runs the same as the initial *Hello world* program.

You can also store aggregate values into the value accumulator.  This is done
by enclosing those values in parentheses.

    (Hello) world

This program does not appear to run any differently than the previous programs;
however, the value `Hello` is pushed all at once into the value accumulator and
stored as a full value instead of printed.  This is called **grouping** and
subgrouping is completely illegal in *waiw*.

If you would like to not print a value when it is stored into the value
accumulator, you can use the `~` operator which causes the next value stored in
the value accumulator not to be printed.

    ~(Hello )world

The above program just prints `world`.

### The Stack

To understand why this ability accumulate as opposed to overwrite is important,
let's take a look at *waiw*'s stack.  The **stack** is LIFO data structure that,
in *waiw*, stores strings.  We can push strings onto the stack using the `&` and
`$` operators.  The former pushes the value in front of it onto the stack and
the latter pushes the value behind it onto the stack.  For example, the two
operations below are equivalent.

    &a
    a$

To retrieve values from the stack, we can use the `_` and `:` operators.  The
former "acts as" the top value on the stack -- whenever it is encountered, the
instruction pointer will store the value on top of the stack into the value
accumulator.  The latter operator has the same behavior as the former except it
also pops the top value off the stack.  So the program,

    &a__

prints `a` three times.  We can use the `~` operator to avoid the first print.

    &~a__

This is where those parentheses come in handy.  For example, if we wanted to
store and print the value `abc`, without the parentheses, we would have to
write:

    &~a&~b&~c:::

However, using the parentheses, we can write:

    &~(abc):

which has the exact some functionality while being considerably shorter.

Note that both the `_` and `:` operators will cause errors if you use them when
the stack is empty.

The next stack operator is `@` which pushes a user-inputted value onto the
stack.  Note that `@` reads a whole line in (trimming off the newline) not just
a single character.  So, a *cat* program (which simply prints the value the user
inputs) in *waiw* is super simple.

    @:

The final stack operator is `|` which reverses the order of values on the stack.

    &a&b|::

The above program prints `abba`.

### Binary Operators

There are five basic binary operators: `+`, `-`, `*`, `/`, and `%`.  They are
all arithmetic operators that take the value before them and the value after
them, apply their respective operation to those values, and push the result onto
the stack as a string.  

The wrinkle with these operators is that they will *reinterpret* their operands
as integers.  They do this by using string-to-integer conversion to read the
string values as numbers.  Note that this is *NOT* ASCII conversion -- the
string `5` is interpreted as the *number* `5` (just as `6783` is interpreted as,
well, `6783`).

A simple program that adds two user-inputted numbers can be written like so:

    @~:+@~::

Running this program will prompt the user to enter two numbers on separate lines
and will print the result.

Note that *waiw* only operates on integers and so `/` performs integer division
not floating-point division.

    ~(50)/~(22):

The above program prints `2` (not `2.27...`).

Finally, it is worth mentioning the all operators can be used inside of `()`.
In fact, the program above and the program below are equivalent:

    ~(50/22):

The reason is that `()` essentially switch the mode of the value accumulator
between them to accumulate instead of overwrite.  Whenever a binary operator
occurs, the value already read in stored as the first operand and the value
accumulator is reset so the second operand can be read in.  Note that the binary
operators are the only operators that do this.

### Instruction Pointer Manipulation

Although the instruction pointer reads left to right by default, you can move it
and redirect it.

The first operator the manipulates the instruction pointer is the `.` which
moves the instruction pointer onto the start (cell 0) over the next line and
prints a newline.

The below program prints `Hello world` twice.

    Hello world.
    Hello world!

The operators `;` and `^` move the instruction pointer down one line and up one
line respectively *instead* of moving left (or right).  This highlights an
important property of instruction pointer: it can only move on one axis at once
(with the exception of the `.` operator). As an example, we can use the caret
operator to create a simple infinite loop.

    a.
    ^

This code prints `a` over and over again forever. 

The next two operators change the direction the instruction pointer moves on the
X-axis.  The `{` changes the instruction pointer to move right (as it does by
default) and the `}` operator changes it to moving left.  Thus, an even simpler
infinite loop can be created.

    {a}

This does the exact same thing as the previous program but in one line instead
of two.  Note that the `{}` operator *redirect* the instruction pointer -- they
don't just move left or right once but change how it is moves in general.

Finally, the `,` operator causes the interpreter to "do nothing" whenever it
encounters it.  It simply reads the comma and keeps moving in whatever direction
it was already going.  The character essentially functions as a "no-op".

### Control Flow

*waiw* has three control flow operators: `=`, `<` and `>`.  The first compares
the two values before and after it as strings for equality.  The latter two
compare the two values as numbers, performing a less than or greater than
comparison respectively.

These three operators all behave in the same way: if their condition is true,
the act as a `,` (no-op); if it is false, the act as a `.` (without printing a
newline).

Using this, we can write a simple truth machine program.  For those unfamiliar,
a truth machine is a program that takes in a number and if it is zero, it prints
`0` and if it is `1` (or any other number for our purposes), it prints an `1`
indefinitely.  Here is this program's source code:

    @~:=~00
    {1}

It first reads a value onto the stack.  If it is equal to zero, it proceeds to
the end of the first line, prints `0` and exits (since the `=` acts like a `,`).
Otherwise, it drops onto the next line which is a simple infinite loop over the
value `1`.

## <a name="operators"> Operator Reference

Below is a table of all the operators in *waiw* along with their functions.

| Operator | Function |
| -------- | -------- |
| `,` | no-op |
| `.` | print newline and move IP to start of next line |
| `;` | move IP to cell directly below |
| `^` | move IP to cell directly above |
| `{` | redirect IP right |
| `}` | redirect IP left |
| `!` | end program |
| `[]` | ignore all text between the brackets |
| `\` | escape the next cell |
| `~` | do not print the next value stored into the value accumulator |
| `()` | accumulate all text between the parentheses as a single value |
| `@` | read a line of user input onto the stack with newline trimmed |
| `&` | push the value after it onto the stack |
| `$` | push the value before it onto the stack |
| `_` | store the top value on the stack into the value accumulator |
| `:` | pop the top value on the stack into the value accumulator |
| `|` | reverse the stack |
| `+` | add the values before and after it as integers |
| `-` | subtract the values before and after it as integers |
| `*` | multiply the values before and after it as integers |
| `/` | divide the values before and after it as integers |
| `%` | modulo the values before and after it as integers |
| `=` | move IP to the next line if the values before and after it are unequal |
| `<` | move IP to the next line if the values before and after it are greater than or equal to each other as numbers |
| `>` | move IP to the next line if the values before and after it are less than or equal to each other as numbers |
