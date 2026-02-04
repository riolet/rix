# Rix

Rix is a concise, object-oriented programming language that transpiles to C. It features Python-like indentation-based syntax, class inheritance, and compiles to efficient native code.

## Features

- **Clean syntax** - Indentation-based blocks, minimal boilerplate
- **Object-oriented** - Classes, inheritance, constructors, methods
- **Transpiles to C** - Native performance, easy debugging
- **Type inference** - Variable types inferred from initialization

## Quick Start

```bash
# Transpile Rix to C
node rixc example.rix -o example.c

# Compile and run
gcc example.c -o example -lm
./example
```

## Language Syntax

### Classes and Inheritance

```rix
Rectangle ::
    w = int
    h = int

    $(int w, int h)
        $.w = w
        $.h = h

    area->int() = w * h

NamedRectangle :: Rectangle
    name = String

    $(int w, int h, String name)
        $.w = w
        $.h = h
        $.name = name
```

### Functions

```rix
# One-liner function
fib -> int(int n) = (n <= 1).tf(n, fib(n-1) + fib(n-2))

# Multi-statement function
isPrime -> bool(int n)
    if n < 2
        return false
    if n % 2 == 0
        return false
    return true
```

### Variables

```rix
# Declaration with type inference
x := 5
name := "hello"
r := Rectangle(10, 20)

# Assignment
x = x + 1
x += 1
```

### Control Flow

```rix
# If/else
if x > 0
    print("positive")
else
    print("non-positive")

# For loop (exclusive end)
for i := 1 to 10
    print(i)

# While loop
while x > 0
    x -= 1
```

### Operators

| Operator | Description |
|----------|-------------|
| `::` | Class definition |
| `:=` | Declaration with assignment |
| `=` | Assignment |
| `+=`, `-=` | Compound assignment |
| `->` | Function/method return type |
| `.tf(a, b)` | Ternary (condition.tf(true_val, false_val)) |
| `$` | Self reference |
| `$.member` | Self member access |

### Built-in Functions

- `print(expr)` - Print to stdout
- `args()` - Argument count (argc)
- `args(n)` - Get argument n (argv[n])
- `int(expr)` - Convert to int
- `String(expr)` - Convert to string
- `sqrt(n)` - Square root

## Performance

Rix transpiles to clean C code that compiles to efficient native binaries. Benchmark results for counting primes up to 2,000,000:

| Implementation | Time | Speedup |
|----------------|------|---------|
| **Rix** (transpiled to C) | 0.18s | 1.0x |
| Hand-written C | 0.19s | 0.95x |
| Python | 8.36s | 0.02x |

Rix-generated code performs on par with hand-written C and runs **~46x faster than Python**.

### Benchmark Code

**Rix** (`benchmarks/benchmark.rix`):
```rix
isPrime -> bool(int n)
    if n < 2
        return false
    if n == 2
        return true
    if n % 2 == 0
        return false

    upperLimit := sqrt(n)
    i := 3
    while i <= upperLimit
        if n % i == 0
            return false
        i += 2
    return true

limit := 2000000
noPrimes := 0
n := 0

while n <= limit
    if isPrime(n)
        noPrimes += 1
    n += 1

print("pi(" + limit + ") = " + noPrimes)
```

Run the benchmark:
```bash
node rixc benchmarks/benchmark.rix -o benchmarks/benchmark.c
gcc -O2 benchmarks/benchmark.c -o benchmarks/benchmark -lm
./benchmarks/benchmark
```

## VSCode Extension

Syntax highlighting for VSCode is included in the `vscode-rix` directory.

### Installation

```bash
# Symlink to VSCode extensions
ln -s /path/to/rix/vscode-rix ~/.vscode/extensions/rix-language

# Reload VSCode
```

### Features

- Syntax highlighting for all Rix constructs
- Comment toggling (`#`)
- Bracket matching
- Smart indentation

## Project Structure

```
rix/
├── rixc                 # CLI transpiler
├── src/
│   ├── lexer.js         # Tokenizer
│   ├── parser.js        # AST builder
│   ├── codegen.js       # C code generator
│   └── transpiler.js    # Pipeline orchestration
├── benchmarks/
│   ├── benchmark.rix    # Rix benchmark
│   ├── benchmark.c      # C benchmark
│   └── benchmark.py     # Python benchmark
├── vscode-rix/          # VSCode extension
└── example.rix          # Example program
```

## CLI Usage

```
Usage: rixc <input.rix> [-o <output.c>] [--debug]

Options:
  -o <file>    Output file (default: input.c)
  --debug      Print tokens and AST to stderr
  -h, --help   Show help message
```

## Example

```rix
# Rectangle with area calculation
Rectangle ::
    w = int
    h = int

    $(int w, int h)
        $.w = w
        $.h = h

    area->int() = w * h
    perimeter->int() = (w + h) * 2

r := Rectangle(5, 10)
print("Area: " + r.area())
print("Perimeter: " + r.perimeter())
```

Output:
```
Area: 50
Perimeter: 30
```

## License

MIT
