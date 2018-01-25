# Clang-Checkers

Implement checker based on Misra-C 2012

## Rules ongoing


## Rules not include

* R7_1:  Octal constants shall not be used

* R7_2:  A "u" or "U" suffix shall be applied to all integer constants that are represented in an unsigned type


## Rules completed

* R12_3:  The comma operator should not be used

* R12_5:  The sizeof operator shall not have an operand which is a function parameter declared as “array of type”

* R16_2:  A switch label shall only be used when the most closely-enclosing compound statement is the body of a switch statement

* R16_4:  Every switch statement shall have a default label

* R16_5:  A default label shall appear as either the first or the last switch label of a switch statement

* R16_6:  Every switch statement shall have at least two switch-clauses 

* R16_7:  A switch-expression shall not have essentially Boolean type

# Reference

https://github.com/rettichschnidi/clang-misracpp2008

