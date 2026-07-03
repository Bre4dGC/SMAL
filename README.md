# smalc — SMAL language interpreter

SMAL (Syntax Minimalist Algorithmic Language) is a bracketless, keyword-free programming language. `smalc` runs SMAL programs.

## Build

```sh
cc -o smalc main.c smalc.c
```

## Usage

```
smalc run <file>    # execute a .smal file
smalc repl          # start interactive session
smalc help          # show commands
smalc version       # show version info
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

### Fibonacci — print first `n` Fibonacci numbers

```smal
ifgt
f = 0
g = 1
> i
i > 0 @
    < f
    t = g
    g + f
    f = t
    i - 1
```

### Euclid's GCD

```smal
ab
a = 1 ~ 100
b = 1 ~ 100
b > 0 @
    a > b ?
        a - b
    a < b ?
        b - a
    a : b ?
        b - a
< a
```

## License

MIT
