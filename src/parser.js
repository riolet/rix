// Rix Parser - Builds AST from tokens

const { TokenType } = require('./lexer');

// AST Node types
class ASTNode {
  constructor(type) {
    this.type = type;
  }
}

class Program extends ASTNode {
  constructor(body) {
    super('Program');
    this.body = body;
  }
}

class ClassDef extends ASTNode {
  constructor(name, parent, members) {
    super('ClassDef');
    this.name = name;
    this.parent = parent;
    this.members = members;
  }
}

class MemberVar extends ASTNode {
  constructor(name, varType) {
    super('MemberVar');
    this.name = name;
    this.varType = varType;
  }
}

class Constructor extends ASTNode {
  constructor(params, body) {
    super('Constructor');
    this.params = params;
    this.body = body;
  }
}

class Method extends ASTNode {
  constructor(name, returnType, params, body, isOneLiner = false) {
    super('Method');
    this.name = name;
    this.returnType = returnType;
    this.params = params;
    this.body = body;
    this.isOneLiner = isOneLiner;
  }
}

class FunctionDef extends ASTNode {
  constructor(name, returnType, params, body, isOneLiner = false) {
    super('FunctionDef');
    this.name = name;
    this.returnType = returnType;
    this.params = params;
    this.body = body;
    this.isOneLiner = isOneLiner;
  }
}

class VarDecl extends ASTNode {
  constructor(name, initializer, varType = null) {
    super('VarDecl');
    this.name = name;
    this.initializer = initializer;
    this.varType = varType;
  }
}

class Assignment extends ASTNode {
  constructor(target, value) {
    super('Assignment');
    this.target = target;
    this.value = value;
  }
}

class IfStmt extends ASTNode {
  constructor(condition, thenBody, elseBody = null) {
    super('IfStmt');
    this.condition = condition;
    this.thenBody = thenBody;
    this.elseBody = elseBody;
  }
}

class ForStmt extends ASTNode {
  constructor(varName, start, end, body) {
    super('ForStmt');
    this.varName = varName;
    this.start = start;
    this.end = end;
    this.body = body;
  }
}

class WhileStmt extends ASTNode {
  constructor(condition, body) {
    super('WhileStmt');
    this.condition = condition;
    this.body = body;
  }
}

class BinaryExpr extends ASTNode {
  constructor(left, operator, right) {
    super('BinaryExpr');
    this.left = left;
    this.operator = operator;
    this.right = right;
  }
}

class UnaryExpr extends ASTNode {
  constructor(operator, operand) {
    super('UnaryExpr');
    this.operator = operator;
    this.operand = operand;
  }
}

class CallExpr extends ASTNode {
  constructor(callee, args) {
    super('CallExpr');
    this.callee = callee;
    this.args = args;
  }
}

class MemberAccess extends ASTNode {
  constructor(object, member) {
    super('MemberAccess');
    this.object = object;
    this.member = member;
  }
}

class Identifier extends ASTNode {
  constructor(name) {
    super('Identifier');
    this.name = name;
  }
}

class IntLiteral extends ASTNode {
  constructor(value) {
    super('IntLiteral');
    this.value = value;
  }
}

class BoolLiteral extends ASTNode {
  constructor(value) {
    super('BoolLiteral');
    this.value = value;
  }
}

class StringLiteral extends ASTNode {
  constructor(value) {
    super('StringLiteral');
    this.value = value;
  }
}

class Instantiation extends ASTNode {
  constructor(className, args) {
    super('Instantiation');
    this.className = className;
    this.args = args;
  }
}

class SelfRef extends ASTNode {
  constructor(member = null) {
    super('SelfRef');
    this.member = member;
  }
}

class TernaryExpr extends ASTNode {
  constructor(condition, trueExpr, falseExpr) {
    super('TernaryExpr');
    this.condition = condition;
    this.trueExpr = trueExpr;
    this.falseExpr = falseExpr;
  }
}

class ArgsCall extends ASTNode {
  constructor(index = null) {
    super('ArgsCall');
    this.index = index;
  }
}

class SqrtCall extends ASTNode {
  constructor(expr) {
    super('SqrtCall');
    this.expr = expr;
  }
}

class TypeCast extends ASTNode {
  constructor(targetType, expr) {
    super('TypeCast');
    this.targetType = targetType;
    this.expr = expr;
  }
}

class PrintStmt extends ASTNode {
  constructor(expr) {
    super('PrintStmt');
    this.expr = expr;
  }
}

class ReturnStmt extends ASTNode {
  constructor(expr) {
    super('ReturnStmt');
    this.expr = expr;
  }
}

class ExternDecl extends ASTNode {
  constructor(name, returnType, params) {
    super('ExternDecl');
    this.name = name;
    this.returnType = returnType;
    this.params = params;
  }
}

class IncludeDecl extends ASTNode {
  constructor(path, isSystem) {
    super('IncludeDecl');
    this.path = path;
    this.isSystem = isSystem;
  }
}

class IndexAccess extends ASTNode {
  constructor(object, index) {
    super('IndexAccess');
    this.object = object;
    this.index = index;
  }
}

class Parser {
  constructor(tokens) {
    this.tokens = tokens;
    this.pos = 0;
    this.knownClasses = new Set();
  }

  peek(offset = 0) {
    const idx = this.pos + offset;
    return idx < this.tokens.length ? this.tokens[idx] : this.tokens[this.tokens.length - 1];
  }

  current() {
    return this.peek(0);
  }

  advance() {
    const token = this.current();
    this.pos++;
    return token;
  }

  check(type) {
    return this.current().type === type;
  }

  match(...types) {
    if (types.some(t => this.check(t))) {
      return this.advance();
    }
    return null;
  }

  expect(type, message) {
    if (this.check(type)) {
      return this.advance();
    }
    const token = this.current();
    throw new Error(`${message} at line ${token.line}:${token.column}, got ${token.type} (${token.value})`);
  }

  skipNewlines() {
    while (this.match(TokenType.NEWLINE)) {}
  }

  parse() {
    const body = [];

    this.skipNewlines();

    while (!this.check(TokenType.EOF)) {
      const stmt = this.parseTopLevel();
      if (stmt) {
        body.push(stmt);
      }
      this.skipNewlines();
    }

    return new Program(body);
  }

  parseTopLevel() {
    // Include directive
    if (this.check(TokenType.INCLUDE)) {
      return this.parseInclude();
    }

    // Extern declaration
    if (this.check(TokenType.EXTERN)) {
      return this.parseExtern();
    }

    // Check for class definition: IDENT ::
    if (this.check(TokenType.IDENTIFIER) && this.peek(1).type === TokenType.CLASS_DEF) {
      return this.parseClassDef();
    }

    // Check for function definition: IDENT -> type (...) or IDENT(...)
    if (this.check(TokenType.IDENTIFIER) &&
        (this.peek(1).type === TokenType.ARROW || this.peek(1).type === TokenType.LPAREN)) {
      return this.parseFunctionDef();
    }

    return this.parseStatement();
  }

  parseInclude() {
    this.expect(TokenType.INCLUDE, 'Expected include');
    let path;
    let isSystem = false;
    if (this.check(TokenType.LT)) {
      // System include: include <stdio.h>
      this.advance();
      path = '';
      while (!this.check(TokenType.GT) && !this.check(TokenType.NEWLINE)) {
        path += this.advance().value;
      }
      this.match(TokenType.GT);
      isSystem = true;
    } else {
      // Local include: include "file.h"
      path = this.expect(TokenType.STRING_LITERAL, 'Expected include path').value;
    }
    this.skipNewlines();
    return new IncludeDecl(path, isSystem);
  }

  parseExtern() {
    this.expect(TokenType.EXTERN, 'Expected extern');
    const name = this.expect(TokenType.IDENTIFIER, 'Expected function name').value;

    // Return type is optional - defaults to void
    let returnType = 'void';
    if (this.match(TokenType.ARROW)) {
      returnType = this.parseType();
    }

    this.expect(TokenType.LPAREN, 'Expected (');
    const params = this.parseParams();
    this.expect(TokenType.RPAREN, 'Expected )');
    this.skipNewlines();
    return new ExternDecl(name, returnType, params);
  }

  parseClassDef() {
    const name = this.expect(TokenType.IDENTIFIER, 'Expected class name').value;
    this.knownClasses.add(name);
    this.expect(TokenType.CLASS_DEF, 'Expected ::');

    let parent = null;
    if (this.check(TokenType.IDENTIFIER)) {
      parent = this.advance().value;
    }

    this.skipNewlines();
    this.expect(TokenType.INDENT, 'Expected indented class body');

    const members = [];
    while (!this.check(TokenType.DEDENT) && !this.check(TokenType.EOF)) {
      this.skipNewlines();
      if (this.check(TokenType.DEDENT)) break;

      const member = this.parseClassMember();
      if (member) {
        members.push(member);
      }
    }

    this.match(TokenType.DEDENT);

    return new ClassDef(name, parent, members);
  }

  parseClassMember() {
    // Constructor: $(params)
    if (this.check(TokenType.DOLLAR) && this.peek(1).type === TokenType.LPAREN) {
      return this.parseConstructor();
    }

    // Method: name->type(params)
    if (this.check(TokenType.IDENTIFIER) && this.peek(1).type === TokenType.ARROW) {
      return this.parseMethod();
    }

    // Member variable: name = type
    if (this.check(TokenType.IDENTIFIER) && this.peek(1).type === TokenType.ASSIGN) {
      const name = this.advance().value;
      this.expect(TokenType.ASSIGN, 'Expected =');
      const varType = this.parseType();
      this.skipNewlines();
      return new MemberVar(name, varType);
    }

    // Skip unknown
    this.advance();
    return null;
  }

  parseConstructor() {
    this.expect(TokenType.DOLLAR, 'Expected $');
    this.expect(TokenType.LPAREN, 'Expected (');
    const params = this.parseParams();
    this.expect(TokenType.RPAREN, 'Expected )');
    this.skipNewlines();

    const body = this.parseBlock();

    return new Constructor(params, body);
  }

  parseMethod() {
    const name = this.expect(TokenType.IDENTIFIER, 'Expected method name').value;
    this.expect(TokenType.ARROW, 'Expected ->');
    const returnType = this.parseType();
    this.expect(TokenType.LPAREN, 'Expected (');
    const params = this.parseParams();
    this.expect(TokenType.RPAREN, 'Expected )');

    // One-liner: = expr
    if (this.match(TokenType.ASSIGN)) {
      const body = this.parseExpression();
      this.skipNewlines();
      return new Method(name, returnType, params, body, true);
    }

    // Block body
    this.skipNewlines();
    const body = this.parseBlock();
    return new Method(name, returnType, params, body, false);
  }

  parseFunctionDef() {
    const name = this.expect(TokenType.IDENTIFIER, 'Expected function name').value;

    // Return type is optional - defaults to void
    let returnType = 'void';
    if (this.match(TokenType.ARROW)) {
      returnType = this.parseType();
    }

    this.expect(TokenType.LPAREN, 'Expected (');
    const params = this.parseParams();
    this.expect(TokenType.RPAREN, 'Expected )');

    // One-liner: = expr
    if (this.match(TokenType.ASSIGN)) {
      const body = this.parseExpression();
      this.skipNewlines();
      return new FunctionDef(name, returnType, params, body, true);
    }

    // Block body
    this.skipNewlines();
    const body = this.parseBlock();
    return new FunctionDef(name, returnType, params, body, false);
  }

  parseParams() {
    const params = [];

    if (this.check(TokenType.RPAREN)) {
      return params;
    }

    do {
      const type = this.parseType();
      const name = this.expect(TokenType.IDENTIFIER, 'Expected parameter name').value;
      params.push({ type, name });
    } while (this.match(TokenType.COMMA));

    return params;
  }

  parseType() {
    if (this.match(TokenType.INT)) {
      return 'int';
    }
    if (this.match(TokenType.STRING)) {
      return 'String';
    }
    if (this.match(TokenType.BOOL)) {
      return 'bool';
    }
    if (this.match(TokenType.VOID)) {
      return 'void';
    }
    if (this.check(TokenType.IDENTIFIER)) {
      return this.advance().value;
    }
    throw new Error(`Expected type at line ${this.current().line}`);
  }

  parseBlock() {
    const body = [];

    if (!this.check(TokenType.INDENT)) {
      return body;
    }

    this.expect(TokenType.INDENT, 'Expected indented block');

    while (!this.check(TokenType.DEDENT) && !this.check(TokenType.EOF)) {
      this.skipNewlines();
      if (this.check(TokenType.DEDENT)) break;

      const stmt = this.parseStatement();
      if (stmt) {
        body.push(stmt);
      }
    }

    this.match(TokenType.DEDENT);

    return body;
  }

  parseStatement() {
    this.skipNewlines();

    // If statement
    if (this.check(TokenType.IF)) {
      return this.parseIfStmt();
    }

    // For loop
    if (this.check(TokenType.FOR)) {
      return this.parseForStmt();
    }

    // While loop
    if (this.check(TokenType.WHILE)) {
      return this.parseWhileStmt();
    }

    // Else (handled by if)
    if (this.check(TokenType.ELSE)) {
      return null;
    }

    // Return statement
    if (this.check(TokenType.RETURN)) {
      return this.parseReturnStmt();
    }

    // Print statement
    if (this.check(TokenType.IDENTIFIER) && this.current().value === 'print') {
      return this.parsePrintStmt();
    }

    // Local variable declaration: .name = ...
    if (this.check(TokenType.DOT) && this.peek(1).type === TokenType.IDENTIFIER) {
      this.advance(); // consume .
      const name = this.advance().value;
      this.expect(TokenType.ASSIGN, 'Expected =');
      const initializer = this.parseExpression();
      this.skipNewlines();
      return new VarDecl(name, initializer);
    }

    // Variable declaration with := or assignment with =
    if (this.check(TokenType.IDENTIFIER)) {
      const name = this.advance().value;

      // Declaration: name := expr
      if (this.match(TokenType.DECLARE_ASSIGN)) {
        const initializer = this.parseExpression();
        this.skipNewlines();
        return new VarDecl(name, initializer);
      }

      // Assignment: name = expr
      if (this.match(TokenType.ASSIGN)) {
        const value = this.parseExpression();
        this.skipNewlines();
        return new Assignment(new Identifier(name), value);
      }

      // Compound assignment: name += expr or name -= expr
      if (this.match(TokenType.PLUS_ASSIGN)) {
        const value = this.parseExpression();
        this.skipNewlines();
        return new Assignment(new Identifier(name), new BinaryExpr(new Identifier(name), '+', value));
      }
      if (this.match(TokenType.MINUS_ASSIGN)) {
        const value = this.parseExpression();
        this.skipNewlines();
        return new Assignment(new Identifier(name), new BinaryExpr(new Identifier(name), '-', value));
      }

      // Member assignment: obj.member = expr
      if (this.match(TokenType.DOT)) {
        const member = this.expect(TokenType.IDENTIFIER, 'Expected member name').value;
        if (this.match(TokenType.ASSIGN)) {
          const value = this.parseExpression();
          this.skipNewlines();
          return new Assignment(new MemberAccess(new Identifier(name), member), value);
        }
        // Not an assignment - back up and parse as expression
        this.pos -= 2; // back up past member and dot
      }

      // Expression starting with identifier (method call, etc.)
      this.pos--; // back up
      const expr = this.parseExpression();
      this.skipNewlines();
      return expr;
    }

    // Self assignment: $.member = expr
    if (this.check(TokenType.DOLLAR)) {
      this.advance();
      this.expect(TokenType.DOT, 'Expected .');
      const member = this.expect(TokenType.IDENTIFIER, 'Expected member name').value;
      this.expect(TokenType.ASSIGN, 'Expected =');
      const value = this.parseExpression();
      this.skipNewlines();
      return new Assignment(new SelfRef(member), value);
    }

    // Expression statement
    const expr = this.parseExpression();
    this.skipNewlines();
    return expr;
  }

  parsePrintStmt() {
    this.advance(); // consume 'print'
    this.expect(TokenType.LPAREN, 'Expected (');
    const expr = this.parseExpression();
    this.expect(TokenType.RPAREN, 'Expected )');
    this.skipNewlines();
    return new PrintStmt(expr);
  }

  parseReturnStmt() {
    this.expect(TokenType.RETURN, 'Expected return');
    let expr = null;
    // Check if there's an expression (not newline or dedent)
    if (!this.check(TokenType.NEWLINE) && !this.check(TokenType.DEDENT) && !this.check(TokenType.EOF)) {
      expr = this.parseExpression();
    }
    this.skipNewlines();
    return new ReturnStmt(expr);
  }

  parseIfStmt() {
    this.expect(TokenType.IF, 'Expected if');
    const condition = this.parseExpression();
    this.skipNewlines();

    const thenBody = this.parseBlock();

    let elseBody = null;
    this.skipNewlines();
    if (this.check(TokenType.ELSE)) {
      this.advance();
      this.skipNewlines();
      elseBody = this.parseBlock();
    }

    return new IfStmt(condition, thenBody, elseBody);
  }

  parseForStmt() {
    this.expect(TokenType.FOR, 'Expected for');
    const varName = this.expect(TokenType.IDENTIFIER, 'Expected loop variable').value;
    this.expect(TokenType.DECLARE_ASSIGN, 'Expected :=');
    const start = this.parseExpression();
    this.expect(TokenType.TO, 'Expected to');
    const end = this.parseExpression();
    this.skipNewlines();

    const body = this.parseBlock();

    return new ForStmt(varName, start, end, body);
  }

  parseWhileStmt() {
    this.expect(TokenType.WHILE, 'Expected while');
    const condition = this.parseExpression();
    this.skipNewlines();

    const body = this.parseBlock();

    return new WhileStmt(condition, body);
  }

  parseExpression() {
    return this.parseComparison();
  }

  parseComparison() {
    let left = this.parseAdditive();

    while (this.check(TokenType.LE) || this.check(TokenType.GE) ||
           this.check(TokenType.LT) || this.check(TokenType.GT) ||
           this.check(TokenType.EQ) || this.check(TokenType.NE)) {
      const op = this.advance().value;
      const right = this.parseAdditive();
      left = new BinaryExpr(left, op, right);
    }

    return left;
  }

  parseAdditive() {
    let left = this.parseMultiplicative();

    while (this.check(TokenType.PLUS) || this.check(TokenType.MINUS)) {
      const op = this.advance().value;
      const right = this.parseMultiplicative();
      left = new BinaryExpr(left, op, right);
    }

    return left;
  }

  parseMultiplicative() {
    let left = this.parseUnary();

    while (this.check(TokenType.STAR) || this.check(TokenType.SLASH) || this.check(TokenType.PERCENT)) {
      const op = this.advance().value;
      const right = this.parseUnary();
      left = new BinaryExpr(left, op, right);
    }

    return left;
  }

  parseUnary() {
    if (this.check(TokenType.MINUS)) {
      const op = this.advance().value;
      const operand = this.parseUnary();
      return new UnaryExpr(op, operand);
    }

    return this.parsePostfix();
  }

  parsePostfix() {
    let expr = this.parsePrimary();

    while (true) {
      // Array indexing: expr[index]
      if (this.match(TokenType.LBRACKET)) {
        const index = this.parseExpression();
        this.expect(TokenType.RBRACKET, 'Expected ]');
        expr = new IndexAccess(expr, index);
      }
      // Method call or member access: .member or .method()
      else if (this.match(TokenType.DOT)) {
        const member = this.expect(TokenType.IDENTIFIER, 'Expected member name').value;

        // Check for .tf(a, b) - ternary
        if (member === 'tf' && this.check(TokenType.LPAREN)) {
          this.advance(); // consume (
          const trueExpr = this.parseExpression();
          this.expect(TokenType.COMMA, 'Expected ,');
          const falseExpr = this.parseExpression();
          this.expect(TokenType.RPAREN, 'Expected )');
          expr = new TernaryExpr(expr, trueExpr, falseExpr);
        }
        // Method call: .method()
        else if (this.check(TokenType.LPAREN)) {
          this.advance(); // consume (
          const args = this.parseArgs();
          this.expect(TokenType.RPAREN, 'Expected )');
          expr = new CallExpr(new MemberAccess(expr, member), args);
        }
        // Member access: .member
        else {
          expr = new MemberAccess(expr, member);
        }
      }
      // Function call: expr(args)
      else if (this.check(TokenType.LPAREN) && expr.type === 'Identifier') {
        this.advance(); // consume (
        const args = this.parseArgs();
        this.expect(TokenType.RPAREN, 'Expected )');
        expr = new CallExpr(expr, args);
      }
      else {
        break;
      }
    }

    return expr;
  }

  parsePrimary() {
    // Parenthesized expression
    if (this.match(TokenType.LPAREN)) {
      const expr = this.parseExpression();
      this.expect(TokenType.RPAREN, 'Expected )');
      return expr;
    }

    // Self reference: $ or $.member
    if (this.check(TokenType.DOLLAR)) {
      this.advance();
      if (this.match(TokenType.DOT)) {
        const member = this.expect(TokenType.IDENTIFIER, 'Expected member name').value;
        // Check for method call: $.method()
        if (this.check(TokenType.LPAREN)) {
          this.advance();
          const args = this.parseArgs();
          this.expect(TokenType.RPAREN, 'Expected )');
          return new CallExpr(new SelfRef(member), args);
        }
        return new SelfRef(member);
      }
      return new SelfRef();
    }

    // Integer literal
    if (this.check(TokenType.INT_LITERAL)) {
      return new IntLiteral(this.advance().value);
    }

    // String literal
    if (this.check(TokenType.STRING_LITERAL)) {
      return new StringLiteral(this.advance().value);
    }

    // Boolean literals
    if (this.match(TokenType.TRUE)) {
      return new BoolLiteral(true);
    }
    if (this.match(TokenType.FALSE)) {
      return new BoolLiteral(false);
    }

    // Type cast: int(expr) or String(expr)
    if (this.check(TokenType.INT) || this.check(TokenType.STRING)) {
      const type = this.advance().value;
      if (this.check(TokenType.LPAREN)) {
        this.advance();
        const expr = this.parseExpression();
        this.expect(TokenType.RPAREN, 'Expected )');
        return new TypeCast(type, expr);
      }
      // Not a cast, just the type identifier
      return new Identifier(type);
    }

    // args() builtin
    if (this.check(TokenType.IDENTIFIER) && this.current().value === 'args') {
      this.advance();
      if (this.match(TokenType.LPAREN)) {
        let index = null;
        if (!this.check(TokenType.RPAREN)) {
          index = this.parseExpression();
        }
        this.expect(TokenType.RPAREN, 'Expected )');
        return new ArgsCall(index);
      }
      return new Identifier('args');
    }

    // sqrt() builtin
    if (this.check(TokenType.IDENTIFIER) && this.current().value === 'sqrt') {
      this.advance();
      this.expect(TokenType.LPAREN, 'Expected (');
      const expr = this.parseExpression();
      this.expect(TokenType.RPAREN, 'Expected )');
      return new SqrtCall(expr);
    }

    // Identifier or class instantiation
    if (this.check(TokenType.IDENTIFIER)) {
      const name = this.advance().value;

      // Class instantiation: ClassName(args)
      if (this.knownClasses.has(name) && this.check(TokenType.LPAREN)) {
        this.advance(); // consume (
        const args = this.parseArgs();
        this.expect(TokenType.RPAREN, 'Expected )');
        return new Instantiation(name, args);
      }

      return new Identifier(name);
    }

    throw new Error(`Unexpected token ${this.current().type} at line ${this.current().line}`);
  }

  parseArgs() {
    const args = [];

    if (this.check(TokenType.RPAREN)) {
      return args;
    }

    do {
      args.push(this.parseExpression());
    } while (this.match(TokenType.COMMA));

    return args;
  }
}

module.exports = {
  Parser,
  Program, ClassDef, MemberVar, Constructor, Method, FunctionDef,
  VarDecl, Assignment, IfStmt, ForStmt, WhileStmt, ReturnStmt,
  BinaryExpr, UnaryExpr, CallExpr, MemberAccess, IndexAccess,
  Identifier, IntLiteral, StringLiteral, BoolLiteral, Instantiation,
  SelfRef, TernaryExpr, ArgsCall, SqrtCall, TypeCast, PrintStmt,
  ExternDecl, IncludeDecl
};
