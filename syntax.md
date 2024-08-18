# C= Syntax Specification

## Main structure and Goals

The **C=** Language aims to be as simple and expandable as possible, while having a C-Level syntax. This translates into 3 main principles :
 
  - We will try to implement as few features as possible in the compiler, keeping only operators (for speed), basic types (int-equivalent,and maybe floats), declaration and function calls.
  - **C=** will allow for type creation with a simple and powerful low-level interface, and for syntaxial remodelling with macro-like evaluators.
  - The compiler and UBI shall both be flexible, with a mix of fixed principles and runtime-modified syntax parameters.

In practice, implementing theses features requires design a completely new kind of *flexible* compiler/interpreter. Indeed, having a hard-coded syntax evalutaor would require us to either limit the flexibility of the syntax or recompile the compiling interface itself at each compile run (*Lot of compile sorry, it will get clearer soon I hope*).

## Implementation

### Base Syntax

<ADD C LIKE BS>

### *Flexible* Interface

**Types**

Declaring custom types ...

**Tokens**

In order to fully achieve full syntax flexibility, we have added the possibility to modify the tokenizer at compile-time, in order to create completely 
new expressions.
This feature is quite simple : 
```
token $ ;
```
The code above creates a new token to be handled by the compiling stack :
We can now create modules incorporating `$` in expressions.


**Modules**

Modules are a feature specific to **C=**. They allow you to redefine, inside a program, the implementation of the langage (or a part of the language), through special statements. Using the module keyword, and `<>` signs to symbolize the token value we can define a parseable expression to be associated with a set of pre existing tokens.

DRAFT 1:
```
// Basically regex enabled macros
module `<parsing-regex>` {
    //stuff it actually does
}
```
EX :
```
module (<(><TYPE><)><VALUE> ) {
    TYPE VALUE = VALUE
}
```


