// Rix Lexer - Tokenizes Rix source into token stream

const TokenType = {
  // Identifiers and literals
  IDENTIFIER: 'IDENTIFIER',
  INT_LITERAL: 'INT_LITERAL',
  STRING_LITERAL: 'STRING_LITERAL',

  // Operators
  CLASS_DEF: 'CLASS_DEF',       // ::
  ASSIGN: 'ASSIGN',             // =
  DECLARE_ASSIGN: 'DECLARE_ASSIGN', // :=
  PLUS_ASSIGN: 'PLUS_ASSIGN',   // +=
  MINUS_ASSIGN: 'MINUS_ASSIGN', // -=
  ARROW: 'ARROW',               // ->
  PLUS: 'PLUS',                 // +
  MINUS: 'MINUS',               // -
  STAR: 'STAR',                 // *
  SLASH: 'SLASH',               // /
  PERCENT: 'PERCENT',           // %
  LE: 'LE',                     // <=
  GE: 'GE',                     // >=
  LT: 'LT',                     // <
  GT: 'GT',                     // >
  EQ: 'EQ',                     // ==
  NE: 'NE',                     // !=

  // Delimiters
  LPAREN: 'LPAREN',             // (
  RPAREN: 'RPAREN',             // )
  COMMA: 'COMMA',               // ,
  DOT: 'DOT',                   // .
  DOLLAR: 'DOLLAR',             // $

  // Keywords
  IF: 'IF',
  ELSE: 'ELSE',
  FOR: 'FOR',
  TO: 'TO',
  WHILE: 'WHILE',
  RETURN: 'RETURN',
  EXTERN: 'EXTERN',
  INCLUDE: 'INCLUDE',
  INT: 'INT',
  STRING: 'STRING',
  BOOL: 'BOOL',
  TRUE: 'TRUE',
  FALSE: 'FALSE',
  VOID: 'VOID',

  // Brackets
  LBRACKET: 'LBRACKET',         // [
  RBRACKET: 'RBRACKET',         // ]

  // Structure
  NEWLINE: 'NEWLINE',
  INDENT: 'INDENT',
  DEDENT: 'DEDENT',
  EOF: 'EOF',
};

const KEYWORDS = {
  'if': TokenType.IF,
  'else': TokenType.ELSE,
  'for': TokenType.FOR,
  'to': TokenType.TO,
  'while': TokenType.WHILE,
  'return': TokenType.RETURN,
  'extern': TokenType.EXTERN,
  'include': TokenType.INCLUDE,
  'int': TokenType.INT,
  'String': TokenType.STRING,
  'bool': TokenType.BOOL,
  'true': TokenType.TRUE,
  'false': TokenType.FALSE,
  'void': TokenType.VOID,
};

class Token {
  constructor(type, value, line, column) {
    this.type = type;
    this.value = value;
    this.line = line;
    this.column = column;
  }

  toString() {
    return `Token(${this.type}, ${JSON.stringify(this.value)}, ${this.line}:${this.column})`;
  }
}

class Lexer {
  constructor(source) {
    this.source = source;
    this.pos = 0;
    this.line = 1;
    this.column = 1;
    this.tokens = [];
    this.indentStack = [0];
    this.atLineStart = true;
  }

  peek(offset = 0) {
    const idx = this.pos + offset;
    return idx < this.source.length ? this.source[idx] : '\0';
  }

  advance() {
    const ch = this.source[this.pos++];
    if (ch === '\n') {
      this.line++;
      this.column = 1;
    } else {
      this.column++;
    }
    return ch;
  }

  skipComment() {
    while (this.peek() !== '\n' && this.peek() !== '\0') {
      this.advance();
    }
  }

  readString() {
    const startLine = this.line;
    const startCol = this.column;
    this.advance(); // consume opening quote
    let value = '';
    while (this.peek() !== '"' && this.peek() !== '\0') {
      if (this.peek() === '\\') {
        this.advance();
        const escaped = this.advance();
        switch (escaped) {
          case 'n': value += '\n'; break;
          case 'r': value += '\r'; break;
          case 't': value += '\t'; break;
          case '\\': value += '\\'; break;
          case '"': value += '"'; break;
          default: value += escaped;
        }
      } else {
        value += this.advance();
      }
    }
    if (this.peek() === '"') {
      this.advance(); // consume closing quote
    }
    return new Token(TokenType.STRING_LITERAL, value, startLine, startCol);
  }

  readNumber() {
    const startLine = this.line;
    const startCol = this.column;
    let value = '';
    while (/[0-9]/.test(this.peek())) {
      value += this.advance();
    }
    return new Token(TokenType.INT_LITERAL, parseInt(value, 10), startLine, startCol);
  }

  readIdentifier() {
    const startLine = this.line;
    const startCol = this.column;
    let value = '';
    while (/[a-zA-Z0-9_]/.test(this.peek())) {
      value += this.advance();
    }
    const type = KEYWORDS[value] || TokenType.IDENTIFIER;
    return new Token(type, value, startLine, startCol);
  }

  handleIndentation() {
    let indent = 0;
    while (this.peek() === ' ') {
      this.advance();
      indent++;
    }
    // Handle tabs as 2 spaces
    while (this.peek() === '\t') {
      this.advance();
      indent += 2;
    }

    // Skip blank lines and comment-only lines
    if (this.peek() === '\n' || this.peek() === '#') {
      return;
    }

    const currentIndent = this.indentStack[this.indentStack.length - 1];

    if (indent > currentIndent) {
      this.indentStack.push(indent);
      this.tokens.push(new Token(TokenType.INDENT, indent, this.line, 1));
    } else if (indent < currentIndent) {
      while (this.indentStack.length > 1 &&
             this.indentStack[this.indentStack.length - 1] > indent) {
        this.indentStack.pop();
        this.tokens.push(new Token(TokenType.DEDENT, indent, this.line, 1));
      }
    }
  }

  tokenize() {
    while (this.pos < this.source.length) {
      // Handle start of line (indentation)
      if (this.atLineStart) {
        this.handleIndentation();
        this.atLineStart = false;
      }

      const ch = this.peek();

      // Skip whitespace (except newlines)
      if (ch === ' ' || ch === '\t') {
        this.advance();
        continue;
      }

      // Comments
      if (ch === '#') {
        this.skipComment();
        continue;
      }

      // Newlines
      if (ch === '\n') {
        // Only emit NEWLINE if there are meaningful tokens on this line
        if (this.tokens.length > 0 &&
            this.tokens[this.tokens.length - 1].type !== TokenType.NEWLINE &&
            this.tokens[this.tokens.length - 1].type !== TokenType.INDENT) {
          this.tokens.push(new Token(TokenType.NEWLINE, '\n', this.line, this.column));
        }
        this.advance();
        this.atLineStart = true;
        continue;
      }

      // Two-character operators
      if (ch === ':' && this.peek(1) === ':') {
        const token = new Token(TokenType.CLASS_DEF, '::', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === ':' && this.peek(1) === '=') {
        const token = new Token(TokenType.DECLARE_ASSIGN, ':=', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '-' && this.peek(1) === '>') {
        const token = new Token(TokenType.ARROW, '->', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '<' && this.peek(1) === '=') {
        const token = new Token(TokenType.LE, '<=', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '>' && this.peek(1) === '=') {
        const token = new Token(TokenType.GE, '>=', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '=' && this.peek(1) === '=') {
        const token = new Token(TokenType.EQ, '==', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '!' && this.peek(1) === '=') {
        const token = new Token(TokenType.NE, '!=', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '+' && this.peek(1) === '=') {
        const token = new Token(TokenType.PLUS_ASSIGN, '+=', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }
      if (ch === '-' && this.peek(1) === '=') {
        const token = new Token(TokenType.MINUS_ASSIGN, '-=', this.line, this.column);
        this.advance(); this.advance();
        this.tokens.push(token);
        continue;
      }

      // Single-character operators and delimiters
      const singleCharTokens = {
        '=': TokenType.ASSIGN,
        '+': TokenType.PLUS,
        '-': TokenType.MINUS,
        '*': TokenType.STAR,
        '/': TokenType.SLASH,
        '%': TokenType.PERCENT,
        '<': TokenType.LT,
        '>': TokenType.GT,
        '(': TokenType.LPAREN,
        ')': TokenType.RPAREN,
        '[': TokenType.LBRACKET,
        ']': TokenType.RBRACKET,
        ',': TokenType.COMMA,
        '.': TokenType.DOT,
        '$': TokenType.DOLLAR,
      };

      if (singleCharTokens[ch]) {
        this.tokens.push(new Token(singleCharTokens[ch], ch, this.line, this.column));
        this.advance();
        continue;
      }

      // String literals
      if (ch === '"') {
        this.tokens.push(this.readString());
        continue;
      }

      // Numbers
      if (/[0-9]/.test(ch)) {
        this.tokens.push(this.readNumber());
        continue;
      }

      // Identifiers and keywords
      if (/[a-zA-Z_]/.test(ch)) {
        this.tokens.push(this.readIdentifier());
        continue;
      }

      // Unknown character - skip it
      this.advance();
    }

    // Emit remaining DEDENTs
    while (this.indentStack.length > 1) {
      this.indentStack.pop();
      this.tokens.push(new Token(TokenType.DEDENT, 0, this.line, this.column));
    }

    // Ensure final NEWLINE
    if (this.tokens.length > 0 &&
        this.tokens[this.tokens.length - 1].type !== TokenType.NEWLINE) {
      this.tokens.push(new Token(TokenType.NEWLINE, '\n', this.line, this.column));
    }

    this.tokens.push(new Token(TokenType.EOF, null, this.line, this.column));

    return this.tokens;
  }
}

module.exports = { Lexer, Token, TokenType };
