# Clang-Checkers

Implement checker based on Misra-C 2012

## Rules ongoing


## Rules not include

* R7_1:  Octal constants shall not be used

* R7_2:  A "u" or "U" suffix shall be applied to all integer constants that are represented in an unsigned type


## Rules completed

* R12_3:  The comma operator should not be used

* R12_5:  The sizeof operator shall not have an operand which is a function parameter declared as “array of type”

* R16_2: A switch label shall only be used when the most closely-enclosing compound statement is the body of a switch statement
# Accelerating compile

* Change ld to gold or lld
  * lld not compilable

# Reference

https://github.com/rettichschnidi/clang-misracpp2008



# Get macro

/lib/Frontend/DiagnosticRenderer.cpp:423:
