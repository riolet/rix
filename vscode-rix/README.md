# Rix Language Support for VSCode

Syntax highlighting for the Rix programming language.

## Features

- Syntax highlighting for `.rix` files
- Comment toggling with `#`
- Bracket matching and auto-closing
- Smart indentation

## Installation

### From source (development)

1. Copy or symlink the `vscode-rix` folder to your VSCode extensions directory:

   **Linux/macOS:**
   ```bash
   ln -s /path/to/vscode-rix ~/.vscode/extensions/rix-language
   ```

   **Windows:**
   ```cmd
   mklink /D %USERPROFILE%\.vscode\extensions\rix-language \path\to\vscode-rix
   ```

2. Restart VSCode or reload the window (`Ctrl+Shift+P` → "Developer: Reload Window")

### Package as VSIX

1. Install vsce: `npm install -g @vscode/vsce`
2. Package: `vsce package`
3. Install the `.vsix` file in VSCode: Extensions → "..." menu → "Install from VSIX..."

## Syntax Highlighting

The extension highlights:

- **Keywords**: `if`, `else`, `for`, `to`
- **Types**: `int`, `String`
- **Classes**: PascalCase identifiers and class definitions (`ClassName ::`)
- **Methods/Functions**: `name->type()` definitions
- **Constructors**: `$(params)`
- **Self references**: `$`, `$.member`
- **Operators**: `::`, `:=`, `=`, `->`, `.tf`, comparison and arithmetic
- **Strings**: Double-quoted with escape sequences
- **Numbers**: Integer literals
- **Comments**: `# line comment`
