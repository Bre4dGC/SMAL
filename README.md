# SMAL Programming Language

SMAL (Syntax Minimalist Algorithmic Language) is a bracketless, keyword-free programming language. `smac` runs SMAL programs.

## Build

```sh
cc -o smal main.c smal.c
```

## Usage

```
smal run <file>    # execute a .smal file
smal repl          # start interactive session
smal help          # show commands
smal version       # show version info
```

In the REPL, type `Q` to quit, `C` to clear all variables.

## Language

### Variables

26 variables (`a`–`z`). Declare them on one line. Each starts at 0.

```smal
x y z
xyz     # valid
```

Assignment uses `=`:

```smal
x = 100
y = x + 50
```

### Arrays

Each variable can hold up to 128 values.

```smal
a = 1 2 3 4 5

a = 5       # without indexing it uses only the first value
a"4 = 1     # sets the 5th value (0-indexed) to 1

s = a"      # sets size of array 'a' - 5
```

### Arithmetic

| Op | Meaning    |
|----|------------|
| `+`| Addition   |
| `-`| Subtraction|
| `*`| Multiply   |
| `/`| Divide     |
| `~`| Random     |

`~` returns a random integer between the left operand and the left + right operand.

```smal
# random number from 1 to 100
r = 1 ~ 100
```

### I/O

`>` reads input into a variable. `<` prints a variable or number.

```smal
> x
< x        # x = 42
```

### Conditionals (`?`)

A comparison followed by `?` executes the indented block below it if the comparison is true.

```smal
x > 10 ?
    < x
```

### Loops (`@`)

A comparison followed by `@` repeats the indented block while the comparison holds.

```smal
i = 0
i < 5 @
    < i
    i + 1
```

Comparisons: `:` (equal), `!` (not equal), `<` (less than), `>` (greater than).

### Nesting

Conditionals and loops support up to 3 levels of nesting. Blocks use 4 spaces or 1 tab.

### Comments

Everything after `#` to end of line is ignored.

## Examples

You can see examples of the language in the `example/` directory.

## License

MIT
