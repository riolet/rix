// Rix Transpiler - Main entry point, orchestrates pipeline

const { Lexer } = require('./lexer');
const { Parser } = require('./parser');
const { CodeGenerator } = require('./codegen');

class Transpiler {
  constructor(options = {}) {
    this.options = options;
  }

  transpile(source) {
    // Tokenize
    const lexer = new Lexer(source);
    const tokens = lexer.tokenize();

    if (this.options.debug) {
      console.error('=== Tokens ===');
      for (const token of tokens) {
        console.error(token.toString());
      }
      console.error('');
    }

    // Parse
    const parser = new Parser(tokens);
    const ast = parser.parse();

    if (this.options.debug) {
      console.error('=== AST ===');
      console.error(JSON.stringify(ast, null, 2));
      console.error('');
    }

    // Generate C code
    const generator = new CodeGenerator();
    const cCode = generator.generate(ast);

    return cCode;
  }
}

module.exports = { Transpiler };
