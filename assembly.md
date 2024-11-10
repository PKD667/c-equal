# Assembly Intermediate for C Equal compilation

## Instructions

### Basic Turing-complete Set

#### Memory
 - `mov` - Move value from one place to another

#### Arithmetic

 - `add` - Add two values
 - `sub` - Subtract two values
 - `mul` - Multiply treturnwo values
 - `div` - Divide two values

#### Logic
 - `and` - Logical AND
 - `or` - Logical OR
 - `xor` - Logical XOR 
 - `not` - Logical NOT 
 - 'cmp' - Compare two values 
 - 'shl' - Shift left
 - 'shr' - Shift right

#### Control Flow
 - `jmp` - Jump to a location
 - `jz` - Jump if zero

#### Stack
 - `push` - Push a value onto the stack
 - `pop` - Pop a value from the stack

### Call structure
 - `call` - Call a function
 - `ret` - Return from a function

### NOP
 - `nop` - No operation

### Base registers
 - `bsp` - Base Stack pointer
 - `csp` - Current Stack pointer
 - `ip` - Instruction pointer
 - `bp` - Base pointer



## Meta

### Basic Info

 - `.file` - file name (debugging purpose)
 - `.name` - function name 

### Structure

 - `<id>:` Memory adress (`<id>` is a label)

### Data

 - `.data <size (int)> <raw-data>`

