# Rix Project Context

## Project Overview

**Rix** is a concise, object-oriented programming language that transpiles to C. It is designed to have a clean, Python-like syntax while maintaining the performance of native C code. The project consists of a compiler/transpiler written in JavaScript (Node.js) and a VSCode extension for syntax highlighting.

### Key Features
*   **Syntax**: Indentation-based, Python-like.
*   **Paradigm**: Object-oriented (classes, inheritance).
*   **Output**: Transpiles to C (requires `gcc` or similar to compile the output).
*   **Performance**: Aims for near-native C performance.

## Directory Structure

*   `src/`: Contains the core logic of the transpiler.
    *   `transpiler.js`: Main pipeline orchestration.
    *   `lexer.js`: Tokenizes the input Rix code.
    *   `parser.js`: Parses tokens into an Abstract Syntax Tree (AST).
    *   `codegen.js`: Generates C code from the AST.
*   `rixc`: The CLI entry point (Node.js script) for the transpiler.
*   `example.rix`: A sample Rix program demonstrating syntax.
*   `benchmarks/`: Contains benchmark scripts for Rix, C, and Python.
*   `vscode-rix/`: Source code for the VSCode extension.

## Usage

### Transpiling Rix to C

To transpile a Rix file to C, use the `rixc` script with Node.js:

```bash
node rixc <input.rix> -o <output.c>
```

**Example:**

```bash
node rixc example.rix -o example.c
```

### Compiling the Generated C Code

After transpiling, compile the C code using a standard C compiler (like `gcc`):

```bash
gcc example.c -o example -lm
```

*   `-lm`: Links the math library (often required for generated code).

### Running

Execute the compiled binary:

```bash
./example
```

## Development Conventions

*   **Compiler Implementation**: The compiler is written in JavaScript.
    *   Code is modularized into `Lexer`, `Parser`, and `CodeGenerator`.
    *   New language features require updates across all three stages in `src/`.
*   **Testing**: Currently, testing seems to be manual or via `example.rix` and benchmarks.
*   **Code Style**: Standard JavaScript conventions.
*   **Output C Code**: The generated C code uses standard libraries (`stdio.h`, `stdlib.h`, `math.h`) and aims to be human-readable to some extent (handles indentation in generation).
