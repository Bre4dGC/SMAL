# Smal - Syntax Minimalist Algorithmic Language

A ultra-minimalist, bracketless, and keyword-free programming language designed for simplicity and easy parsing.

## 1. General

- Variables: Single-character names only (`a-z`). Initialized to 0 upon declaration.

- Declarations: Variables must be declared before use by listing them separated by spaces.

- Comments: Everything after the `#` symbol is ignored.

- Whitespace: Indentation (4 spaces or 1 tab) is used to define blocks for loops and conditionals.

## 2. Syntax & Operations

### Variables & Assignment

```smal
x y z       # Declare variables x, y, and z (all set to 0)
x = 100     # Assign 100 to x
y = 33      # Assign 33 to y
z = x - y   # Linear arithmetic expression (z becomes 67)
```

### Input / Output (I/O)

No functions allowed. Stream symbols define data flow direction.

`>` Input (Read from console into variable)
`<` Output (Print variable value to console)

```smal
a
> a         # Read user input into 'a'
< a         # Print the value of 'a'
```

### Conditionals (?)

Evaluates the expression. If true, executes the indented block below it.

```smal
x = 10
y = 20

x < y ?
    > x     # Print only if x < y
```

### Loops (@)

Repeats the indented block as long as the expression before @ remains true.

```smal
i = 0
i < 5 @
    > i
    i + 1
```

## 3. Complete Example
A program that reads a number n and prints the sum of all numbers from 1 to n:

```smal
# Variable declaration
n i s

# Get user input
> n

# Initialization
i = 1
s = 0

# Loop block
i < n @
    s + i
    i + 1

# Output result
< s
```