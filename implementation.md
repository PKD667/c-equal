# Implementation Specification 

This **C=** implementation is modular, it includes 3 passes, the <u>Parsing</u>, the <u>Virtual Compiling</u> and then a machine compiling step or and interpreting / virtual machine.

## Parsing

### Tokenizing

Like most languages, the code goes through a simple tokenizer, which convert the human readable code into a list of Tokens, represented as INTs (or ENUMs).

### Parsing into the AST

After tokenization, the code is converted into an Abstract Tree representation. The process comports several modular steps :

1. Using the PEX language (a weird subset of RegEX), we identify expressions, and assign parsing functions to them. 
2. Then, the token list is passed to the parsing function, which creates a corresponding AST struct and adds it to the tree.
3. The PEX parser is called recusrsively, and we go back to 1.


## Virtual Compiling

### Instructions block Initiation

First we create a small set of ASM-like instructions (see [ASM Spec](assembly.md))

### AST Evalutation

With a simple recusrive descent algorithm, we evaluate every AST block (node/branch) into our ASM instructions. The resulting code is added to a big program file.

## Post-Compile Ops

After our code is finally converted to an ASM-Like language, we can choose beetween 2 ways of runnning it, the compiled executable, or the intepreted bytcode.

### Machine Compiling

thanks to a simple map, we convert our ASM to the machine equivalent.

### Intepreting

The ASM is passed to a intepreter, or VM which runs it directly.
