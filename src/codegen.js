// Rix Code Generator - Generates C code from AST

class CodeGenerator {
  constructor() {
    this.output = [];
    this.indent = 0;
    this.classes = new Map();
    this.currentClass = null;
    this.currentMethodParams = new Set();
    this.functions = new Map();
    this.externs = new Map();
    this.includes = [];
    this.localVars = new Map();
    this.globalVars = new Map();
  }

  emit(line) {
    const indentStr = '    '.repeat(this.indent);
    this.output.push(indentStr + line);
  }

  emitRaw(line) {
    this.output.push(line);
  }

  generate(ast) {
    // First pass: collect class and function info
    this.collectDeclarations(ast);

    // Generate header - standard includes
    this.emitRaw('#include <stdio.h>');
    this.emitRaw('#include <stdlib.h>');
    this.emitRaw('#include <string.h>');
    this.emitRaw('#include <stdbool.h>');
    this.emitRaw('#include <math.h>');

    // User includes
    for (const inc of this.includes) {
      if (inc.isSystem) {
        this.emitRaw(`#include <${inc.path}>`);
      } else {
        this.emitRaw(`#include "${inc.path}"`);
      }
    }
    this.emitRaw('');

    // Forward declarations
    this.emitRaw('// Forward declarations');
    for (const [name] of this.classes) {
      this.emitRaw(`typedef struct ${name} ${name};`);
    }
    this.emitRaw('');

    // Helper for string concatenation
    this.emitRaw('// String helper');
    this.emitRaw('char* str_concat(const char* a, const char* b) {');
    this.emitRaw('    char* result = malloc(strlen(a) + strlen(b) + 1);');
    this.emitRaw('    strcpy(result, a);');
    this.emitRaw('    strcat(result, b);');
    this.emitRaw('    return result;');
    this.emitRaw('}');
    this.emitRaw('');

    // Helper for int to string
    this.emitRaw('char* int_to_str(int n) {');
    this.emitRaw('    char* result = malloc(32);');
    this.emitRaw('    sprintf(result, "%d", n);');
    this.emitRaw('    return result;');
    this.emitRaw('}');
    this.emitRaw('');

    // Generate struct definitions
    this.emitRaw('// Struct definitions');
    for (const node of ast.body) {
      if (node.type === 'ClassDef') {
        this.generateStructDef(node);
      }
    }

    // Generate function prototypes
    this.emitRaw('// Function prototypes');
    for (const node of ast.body) {
      if (node.type === 'ClassDef') {
        this.generateClassPrototypes(node);
      } else if (node.type === 'FunctionDef') {
        this.generateFunctionPrototype(node);
      }
    }
    this.emitRaw('');

    // Generate class methods and constructors
    for (const node of ast.body) {
      if (node.type === 'ClassDef') {
        this.generateClassMethods(node);
      }
    }

    // Generate standalone functions
    for (const node of ast.body) {
      if (node.type === 'FunctionDef') {
        this.generateFunction(node);
      }
    }

    // Generate main function with remaining statements
    this.generateMain(ast);

    return this.output.join('\n');
  }

  collectDeclarations(ast) {
    for (const node of ast.body) {
      if (node.type === 'ClassDef') {
        const members = {};
        const methods = {};
        for (const m of node.members) {
          if (m.type === 'MemberVar') {
            members[m.name] = m.varType;
          } else if (m.type === 'Method') {
            methods[m.name] = m.returnType;
          }
        }
        this.classes.set(node.name, {
          parent: node.parent,
          members,
          methods,
          node
        });
      } else if (node.type === 'FunctionDef') {
        this.functions.set(node.name, node.returnType);
      } else if (node.type === 'ExternDecl') {
        this.externs.set(node.name, node.returnType);
      } else if (node.type === 'IncludeDecl') {
        this.includes.push(node);
      }
    }
  }

  generateStructDef(node) {
    this.emitRaw(`struct ${node.name} {`);

    // If has parent, include base struct
    if (node.parent) {
      this.emitRaw(`    ${node.parent} base;`);
    }

    // Member variables
    for (const m of node.members) {
      if (m.type === 'MemberVar') {
        const cType = this.toCType(m.varType);
        this.emitRaw(`    ${cType} ${m.name};`);
      }
    }

    this.emitRaw('};');
    this.emitRaw('');
  }

  generateClassPrototypes(node) {
    const name = node.name;

    // Constructor
    for (const m of node.members) {
      if (m.type === 'Constructor') {
        const params = m.params.map(p => `${this.toCType(p.type)} ${p.name}`).join(', ');
        this.emitRaw(`${name}* ${name}_new(${params || 'void'});`);
      }
    }

    // Methods
    for (const m of node.members) {
      if (m.type === 'Method') {
        const retType = this.toCType(m.returnType);
        const params = [`${name}* self`, ...m.params.map(p => `${this.toCType(p.type)} ${p.name}`)].join(', ');
        this.emitRaw(`${retType} ${name}_${m.name}(${params});`);
      }
    }
  }

  generateFunctionPrototype(node) {
    const retType = this.toCType(node.returnType);
    const params = node.params.map(p => `${this.toCType(p.type)} ${p.name}`).join(', ');
    this.emitRaw(`${retType} ${node.name}(${params || 'void'});`);
  }

  generateClassMethods(node) {
    this.currentClass = node.name;
    const name = node.name;

    // Constructor
    for (const m of node.members) {
      if (m.type === 'Constructor') {
        // Track constructor parameters so we don't convert them to self->
        this.currentMethodParams = new Set(m.params.map(p => p.name));

        const params = m.params.map(p => `${this.toCType(p.type)} ${p.name}`).join(', ');
        this.emitRaw(`${name}* ${name}_new(${params || 'void'}) {`);
        this.indent++;
        this.emit(`${name}* self = malloc(sizeof(${name}));`);

        for (const stmt of m.body) {
          this.generateStatement(stmt);
        }

        this.emit('return self;');
        this.indent--;
        this.emitRaw('}');
        this.emitRaw('');

        this.currentMethodParams = new Set();
      }
    }

    // Methods
    for (const m of node.members) {
      if (m.type === 'Method') {
        // Track method parameters so we don't convert them to self->
        this.currentMethodParams = new Set(m.params.map(p => p.name));

        const retType = this.toCType(m.returnType);
        const params = [`${name}* self`, ...m.params.map(p => `${this.toCType(p.type)} ${p.name}`)].join(', ');
        this.emitRaw(`${retType} ${name}_${m.name}(${params}) {`);
        this.indent++;

        if (m.isOneLiner) {
          const expr = this.generateExpr(m.body);
          this.emit(`return ${expr};`);
        } else {
          for (const stmt of m.body) {
            this.generateStatement(stmt);
          }
        }

        this.indent--;
        this.emitRaw('}');
        this.emitRaw('');

        this.currentMethodParams = new Set();
      }
    }

    this.currentClass = null;
  }

  generateFunction(node) {
    // Track function parameters
    this.currentMethodParams = new Set(node.params.map(p => p.name));

    const retType = this.toCType(node.returnType);
    const params = node.params.map(p => `${this.toCType(p.type)} ${p.name}`).join(', ');
    this.emitRaw(`${retType} ${node.name}(${params || 'void'}) {`);
    this.indent++;

    if (node.isOneLiner) {
      const expr = this.generateExpr(node.body);
      this.emit(`return ${expr};`);
    } else {
      for (const stmt of node.body) {
        this.generateStatement(stmt);
      }
    }

    this.indent--;
    this.emitRaw('}');
    this.emitRaw('');

    this.currentMethodParams = new Set();
  }

  generateMain(ast) {
    this.emitRaw('int main(int argc, char** argv) {');
    this.indent++;

    // Filter statements that should go in main
    for (const node of ast.body) {
      if (node.type !== 'ClassDef' && node.type !== 'FunctionDef') {
        this.generateStatement(node);
      }
    }

    this.emit('return 0;');
    this.indent--;
    this.emitRaw('}');
  }

  generateStatement(node) {
    switch (node.type) {
      case 'VarDecl':
        this.generateVarDecl(node);
        break;
      case 'Assignment':
        this.generateAssignment(node);
        break;
      case 'IfStmt':
        this.generateIfStmt(node);
        break;
      case 'ForStmt':
        this.generateForStmt(node);
        break;
      case 'WhileStmt':
        this.generateWhileStmt(node);
        break;
      case 'PrintStmt':
        this.generatePrintStmt(node);
        break;
      case 'ReturnStmt':
        this.generateReturnStmt(node);
        break;
      default:
        // Expression statement
        const expr = this.generateExpr(node);
        if (expr) {
          this.emit(`${expr};`);
        }
    }
  }

  generateVarDecl(node) {
    const init = this.generateExpr(node.initializer);
    const type = this.inferType(node.initializer);
    const cType = this.toCType(type);

    this.localVars.set(node.name, type);
    this.emit(`${cType} ${node.name} = ${init};`);
  }

  generateAssignment(node) {
    const value = this.generateExpr(node.value);

    if (node.target.type === 'SelfRef') {
      const member = node.target.member;
      // Check if member is in current class or inherited
      const classInfo = this.classes.get(this.currentClass);
      if (classInfo && classInfo.members[member]) {
        this.emit(`self->${member} = ${value};`);
      } else if (classInfo && classInfo.parent) {
        // Check if it's an inherited member
        const parentMember = this.getClassMember(classInfo.parent, member);
        if (parentMember) {
          this.emit(`self->base.${member} = ${value};`);
        } else {
          this.emit(`self->${member} = ${value};`);
        }
      } else {
        this.emit(`self->${member} = ${value};`);
      }
    } else if (node.target.type === 'Identifier') {
      this.emit(`${node.target.name} = ${value};`);
    } else if (node.target.type === 'MemberAccess') {
      const obj = this.generateExpr(node.target.object);
      this.emit(`${obj}->${node.target.member} = ${value};`);
    } else {
      const target = this.generateExpr(node.target);
      this.emit(`${target} = ${value};`);
    }
  }

  generateIfStmt(node) {
    const cond = this.generateExpr(node.condition);
    this.emit(`if (${cond}) {`);
    this.indent++;

    for (const stmt of node.thenBody) {
      this.generateStatement(stmt);
    }

    this.indent--;

    if (node.elseBody && node.elseBody.length > 0) {
      this.emit('} else {');
      this.indent++;

      for (const stmt of node.elseBody) {
        this.generateStatement(stmt);
      }

      this.indent--;
    }

    this.emit('}');
  }

  generateForStmt(node) {
    const start = this.generateExpr(node.start);
    const end = this.generateExpr(node.end);

    this.emit(`for (int ${node.varName} = ${start}; ${node.varName} < ${end}; ${node.varName}++) {`);
    this.indent++;

    this.localVars.set(node.varName, 'int');

    for (const stmt of node.body) {
      this.generateStatement(stmt);
    }

    this.indent--;
    this.emit('}');
  }

  generateWhileStmt(node) {
    const cond = this.generateExpr(node.condition);
    this.emit(`while (${cond}) {`);
    this.indent++;

    for (const stmt of node.body) {
      this.generateStatement(stmt);
    }

    this.indent--;
    this.emit('}');
  }

  generatePrintStmt(node) {
    const exprType = this.inferType(node.expr);
    const expr = this.generateExpr(node.expr);

    if (exprType === 'int') {
      this.emit(`printf("%d\\n", ${expr});`);
    } else {
      this.emit(`printf("%s\\n", ${expr});`);
    }
  }

  generateReturnStmt(node) {
    if (node.expr) {
      const expr = this.generateExpr(node.expr);
      this.emit(`return ${expr};`);
    } else {
      this.emit('return;');
    }
  }

  generateExpr(node) {
    if (!node) return '';

    switch (node.type) {
      case 'IntLiteral':
        return String(node.value);

      case 'BoolLiteral':
        return node.value ? 'true' : 'false';

      case 'StringLiteral':
        return `"${this.escapeString(node.value)}"`;

      case 'Identifier':
        // Check if this identifier is a class member (and not a method param or local)
        if (this.currentClass && !this.currentMethodParams.has(node.name) && !this.localVars.has(node.name)) {
          const memberType = this.getClassMember(this.currentClass, node.name);
          if (memberType) {
            return `self->${node.name}`;
          }
        }
        return node.name;

      case 'SelfRef':
        if (node.member) {
          // Check if member is in current class or inherited
          if (this.currentClass) {
            const classInfo = this.classes.get(this.currentClass);
            if (classInfo && classInfo.members[node.member]) {
              return `self->${node.member}`;
            } else if (classInfo && classInfo.parent) {
              const parentMember = this.getClassMember(classInfo.parent, node.member);
              if (parentMember) {
                return `self->base.${node.member}`;
              }
            }
          }
          return `self->${node.member}`;
        }
        return 'self';

      case 'BinaryExpr':
        return this.generateBinaryExpr(node);

      case 'UnaryExpr':
        return `(${node.operator}${this.generateExpr(node.operand)})`;

      case 'TernaryExpr':
        return `((${this.generateExpr(node.condition)}) ? (${this.generateExpr(node.trueExpr)}) : (${this.generateExpr(node.falseExpr)}))`;

      case 'CallExpr':
        return this.generateCallExpr(node);

      case 'MemberAccess':
        return this.generateMemberAccess(node);

      case 'IndexAccess':
        return `${this.generateExpr(node.object)}[${this.generateExpr(node.index)}]`;

      case 'Instantiation':
        const args = node.args.map(a => this.generateExpr(a)).join(', ');
        return `${node.className}_new(${args})`;

      case 'ArgsCall':
        if (node.index === null) {
          return 'argc';
        }
        return `argv[${this.generateExpr(node.index)}]`;

      case 'SqrtCall':
        return `(int)sqrt((double)${this.generateExpr(node.expr)})`;

      case 'TypeCast':
        if (node.targetType === 'int') {
          return `atoi(${this.generateExpr(node.expr)})`;
        } else if (node.targetType === 'String') {
          return `int_to_str(${this.generateExpr(node.expr)})`;
        }
        return this.generateExpr(node.expr);

      default:
        return '';
    }
  }

  generateBinaryExpr(node) {
    const left = this.generateExpr(node.left);
    const right = this.generateExpr(node.right);
    const leftType = this.inferType(node.left);
    const rightType = this.inferType(node.right);

    // String concatenation
    if (node.operator === '+' && (leftType === 'String' || rightType === 'String')) {
      const leftStr = leftType === 'int' ? `int_to_str(${left})` : left;
      const rightStr = rightType === 'int' ? `int_to_str(${right})` : right;
      return `str_concat(${leftStr}, ${rightStr})`;
    }

    return `(${left} ${node.operator} ${right})`;
  }

  generateCallExpr(node) {
    const args = node.args.map(a => this.generateExpr(a)).join(', ');

    // Method call on member access
    if (node.callee.type === 'MemberAccess') {
      const obj = this.generateExpr(node.callee.object);
      const method = node.callee.member;
      const objType = this.inferType(node.callee.object);

      // Find the class that has this method (may be inherited)
      let className = objType;
      if (className.endsWith('*')) {
        className = className.slice(0, -1);
      }

      const methodInfo = this.getClassMethod(className, method);
      if (methodInfo) {
        // If method is in parent class, need to cast using the base member
        if (methodInfo.className !== className) {
          return `${methodInfo.className}_${method}(&${obj}->base${args ? ', ' + args : ''})`;
        }
        return `${className}_${method}(${obj}${args ? ', ' + args : ''})`;
      }

      return `${className}_${method}(${obj}${args ? ', ' + args : ''})`;
    }

    // Method call on self
    if (node.callee.type === 'SelfRef' && node.callee.member) {
      const method = node.callee.member;
      const methodInfo = this.getClassMethod(this.currentClass, method);

      if (methodInfo && methodInfo.className !== this.currentClass) {
        // Inherited method - use the base member
        return `${methodInfo.className}_${method}(&self->base${args ? ', ' + args : ''})`;
      }

      return `${this.currentClass}_${method}(self${args ? ', ' + args : ''})`;
    }

    // Regular function call
    if (node.callee.type === 'Identifier') {
      return `${node.callee.name}(${args})`;
    }

    return `${this.generateExpr(node.callee)}(${args})`;
  }

  generateMemberAccess(node) {
    const obj = this.generateExpr(node.object);
    return `${obj}->${node.member}`;
  }

  inferType(node) {
    if (!node) return 'int';

    switch (node.type) {
      case 'IntLiteral':
        return 'int';

      case 'BoolLiteral':
        return 'bool';

      case 'StringLiteral':
        return 'String';

      case 'Identifier':
        if (this.localVars.has(node.name)) {
          return this.localVars.get(node.name);
        }
        if (this.globalVars.has(node.name)) {
          return this.globalVars.get(node.name);
        }
        // Check if it's a class member (when inside a method)
        if (this.currentClass && !this.currentMethodParams.has(node.name)) {
          const memberType = this.getClassMember(this.currentClass, node.name);
          if (memberType) {
            return memberType;
          }
        }
        return 'int';

      case 'SelfRef':
        if (node.member && this.currentClass) {
          // Check current class and inherited members
          const memberType = this.getClassMember(this.currentClass, node.member);
          if (memberType) {
            return memberType;
          }
        }
        return 'int';

      case 'BinaryExpr':
        if (node.operator === '+') {
          const leftType = this.inferType(node.left);
          const rightType = this.inferType(node.right);
          if (leftType === 'String' || rightType === 'String') {
            return 'String';
          }
        }
        if (['<=', '>=', '<', '>', '==', '!='].includes(node.operator)) {
          return 'int';
        }
        return this.inferType(node.left);

      case 'TernaryExpr':
        return this.inferType(node.trueExpr);

      case 'CallExpr':
        if (node.callee.type === 'MemberAccess') {
          const objType = this.inferType(node.callee.object);
          let className = objType;
          if (className.endsWith('*')) {
            className = className.slice(0, -1);
          }
          const classInfo = this.classes.get(className);
          if (classInfo && classInfo.methods[node.callee.member]) {
            return classInfo.methods[node.callee.member];
          }
        }
        if (node.callee.type === 'SelfRef' && node.callee.member) {
          const classInfo = this.classes.get(this.currentClass);
          if (classInfo && classInfo.methods[node.callee.member]) {
            return classInfo.methods[node.callee.member];
          }
        }
        if (node.callee.type === 'Identifier') {
          if (this.functions.has(node.callee.name)) {
            return this.functions.get(node.callee.name);
          }
          if (this.externs.has(node.callee.name)) {
            return this.externs.get(node.callee.name);
          }
          // Special cases for C library functions
          if (node.callee.name === 'malloc' || node.callee.name === 'strdup') {
            return 'String';
          }
        }
        return 'int';

      case 'MemberAccess':
        const objType = this.inferType(node.object);
        let className = objType;
        if (className.endsWith('*')) {
          className = className.slice(0, -1);
        }
        const classInfo = this.classes.get(className);
        if (classInfo && classInfo.members[node.member]) {
          return classInfo.members[node.member];
        }
        return 'int';

      case 'Instantiation':
        return node.className + '*';

      case 'ArgsCall':
        return node.index === null ? 'int' : 'String';

      case 'TypeCast':
        return node.targetType;

      default:
        return 'int';
    }
  }

  toCType(type) {
    switch (type) {
      case 'int':
        return 'int';
      case 'String':
        return 'char*';
      case 'bool':
        return 'bool';
      case 'void':
        return 'void';
      default:
        if (this.classes.has(type)) {
          return `${type}*`;
        }
        return type;
    }
  }

  // Get class member type, checking parent classes for inheritance
  getClassMember(className, memberName) {
    const classInfo = this.classes.get(className);
    if (!classInfo) return null;

    if (classInfo.members[memberName]) {
      return classInfo.members[memberName];
    }

    // Check parent class
    if (classInfo.parent) {
      return this.getClassMember(classInfo.parent, memberName);
    }

    return null;
  }

  // Get class method, checking parent classes for inheritance
  getClassMethod(className, methodName) {
    const classInfo = this.classes.get(className);
    if (!classInfo) return null;

    if (classInfo.methods[methodName]) {
      return { className, returnType: classInfo.methods[methodName] };
    }

    // Check parent class
    if (classInfo.parent) {
      return this.getClassMethod(classInfo.parent, methodName);
    }

    return null;
  }

  escapeString(str) {
    return str
      .replace(/\\/g, '\\\\')
      .replace(/"/g, '\\"')
      .replace(/\r/g, '\\r')
      .replace(/\n/g, '\\n')
      .replace(/\t/g, '\\t');
  }
}

module.exports = { CodeGenerator };
