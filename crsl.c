/* These should be written in pure Rix */

#include "ObjectTree.h"
#include "crsl.h"
#include "rixc.h"

void defineRSLSymbols(Object * root)
{
    Object *objBaseType, *temp2, *temp3, *temp4, *temp5, *rslFunc;

    // ==============  Built-in Types ===============

    objBaseType = CreateObject("BaseType", "BaseType", 0, Type, "BaseType");
    setFlags(objBaseType, FLAG_EXTERNAL);
    addSymbol(root, objBaseType);

    temp2 =
        CreateObject("Generic_$$", "Generic_$$" COMPILER_SEP "BaseType", objBaseType,
                     Type, "Generic_$$");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    temp2 =
            CreateObject("Generic_YTYPE$$", "Generic_YTYPE$$" COMPILER_SEP "BaseType", objBaseType,
                         Type, "Generic_YTYPE$$");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    temp2 =
            CreateObject("Generic_ZTYPE$$", "Generic_ZTYPE$$" COMPILER_SEP "BaseType", objBaseType,
                         Type, "Generic_ZTYPE$$");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    temp2 =
        CreateObject("bool", "bool" COMPILER_SEP "BaseType", objBaseType, Type,
                     "bool");
    setFlags(temp2, FLAG_EXTERNAL);
    setFlags(temp2, FLAG_PRIMITIVE);
    addSymbol(root, temp2);

    temp2 =
        CreateObject("tern", "tern" COMPILER_SEP "BaseType", objBaseType, Type,
                     "tern");
    setFlags(temp2, FLAG_EXTERNAL);
    setFlags(temp2, FLAG_PRIMITIVE);
    addSymbol(root, temp2);

    temp2 =
        CreateObject("Number", "Number" COMPILER_SEP "BaseType", objBaseType, Type,
                     "Number");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    temp3 =
        CreateObject("int", "int" COMPILER_SEP "Number", temp2, Type, "int");
    setFlags(temp3, FLAG_EXTERNAL);
    setFlags(temp3, FLAG_PRIMITIVE);
    addSymbol(root, temp3);
    temp3 = CreateObject("float", "float" COMPILER_SEP "Number", temp2, Type, "float");
    setFlags(temp3, FLAG_EXTERNAL);
    setFlags(temp3, FLAG_PRIMITIVE);
    addSymbol(root, temp3);

    temp3 = CreateObject("char", "char" COMPILER_SEP "Number", temp2, Type, "char");
    setFlags(temp3, FLAG_EXTERNAL);
    setFlags(temp3, FLAG_PRIMITIVE);
    addSymbol(root, temp3);

    temp4 =
        CreateObject("System", "System" COMPILER_SEP "BaseType", objBaseType, Type,
                     "System");
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    temp4 =
            CreateObject(IDENT_MPTR, IDENT_MPTR COMPILER_SEP "BaseType", objBaseType, Type,
                         IDENT_MPTR);
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    temp4 =
            CreateObject(IDENT_HEAP_MPTR, IDENT_HEAP_MPTR COMPILER_SEP "BaseType", objBaseType, Type,
                         IDENT_HEAP_MPTR);
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    // ==============  Basetype constructor ===============

    rslFunc =
            CreateObject("BaseType", "BaseType" COMPILER_SEP "BaseType" COMPILER_SEP,
                         0, Constructor, IDENT_MPTR);
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "BaseType");
    addSymbol(root, rslFunc);

    // ==============  Exponent functions ===============

    rslFunc =
        CreateObject("exponent", "float" COMPILER_SEP "exponent" COMPILER_SEP "int",
                     0, Function, "float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "float");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("exponent", "int" COMPILER_SEP "exponent" COMPILER_SEP "float",
                     0, Function, "float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addParam(rslFunc, "float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("exponent", "float" COMPILER_SEP "exponent" COMPILER_SEP "float", 0,
                     Function, "float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "float");
    addParam(rslFunc, "float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("exponent", "int" COMPILER_SEP "exponent" COMPILER_SEP "int",
                     0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);

    // ============== Sqrt functions ==============
    // ============== TODO sqrt(int) should be a float? ==============
    rslFunc =
        CreateObject("sqrt", "int" COMPILER_SEP "sqrt" COMPILER_SEP, 0, Function,
                     "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);

    // ==============  String Functions ===============
//    rslFunc =
//            CreateObject("String", "String" COMPILER_SEP "String" COMPILER_SEP, 0,
//                         Constructor, IDENT_MPTR);
//    setFlags(rslFunc, FLAG_EXTERNAL);
//    addParam(rslFunc, "String");
//    addSymbol(root, rslFunc);

    rslFunc =
            CreateObject("assign", "String" COMPILER_SEP "String" COMPILER_SEP, 0,
                         Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("assign", "String" COMPILER_SEP "assign" COMPILER_SEP "String", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "int" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "int", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "float" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "float");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "float", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "float");
    addSymbol(root, rslFunc);
    rslFunc =
            CreateObject("getObjectAtIndex", "String" COMPILER_SEP "getObjectAtIndex" COMPILER_SEP "int", 0,
                         Function, "Char");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);

    // ==============  Conditional Functions ===============

    rslFunc = CreateObject("if", "bool" COMPILER_SEP "if", 0, Function, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("elif", "bool" COMPILER_SEP "elif" COMPILER_SEP "bool", 0,
                     Function, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "bool");
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("else", "bool" COMPILER_SEP "else", 0, Function, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("tf",
                     "bool" COMPILER_SEP "tf" COMPILER_SEP GENERIC_PARAM
                     COMPILER_SEP GENERIC_PARAM, 0, Function, "Generic_$$");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "bool");
    addParam(rslFunc, "Generic_$$");
    addParam(rslFunc, "Generic_$$");
    addGenericType(rslFunc, 0, 1);
    addSymbol(root, rslFunc);
    //============= Python style conditional Functions =============
    rslFunc = CreateObject("if", "if" COMPILER_SEP "bool", 0, Function, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    //setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("elif", "elif" COMPILER_SEP "bool", 0, Function, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    //setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("else", "else" COMPILER_SEP, 0, Function, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    //setFlags(rslFunc, FLAG_SAVERESULT);
    addSymbol(root, rslFunc);

    //============= Ternary Functions =============
    rslFunc =
        CreateObject("compare", "int" COMPILER_SEP "compare" COMPILER_SEP "int",
                     0, Function, "Ternary");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("gel",
                     "Ternary" COMPILER_SEP "gel" COMPILER_SEP GENERIC_PARAM "0"
                     COMPILER_SEP GENERIC_PARAM "1" COMPILER_SEP GENERIC_PARAM "2", 0,
                     Function, "Generic_$$");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Ternary");
    addParam(rslFunc, "Generic_$$");
    addParam(rslFunc, "Generic_$$");
    addParam(rslFunc, "Generic_$$");
    addGenericType(rslFunc, 0, 1);
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("leg",
                     "Ternary" COMPILER_SEP "leg" COMPILER_SEP GENERIC_PARAM "0"
                     COMPILER_SEP GENERIC_PARAM "1" COMPILER_SEP GENERIC_PARAM "2", 0,
                     Function, "Generic_$$");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Ternary");
    addParam(rslFunc, "Generic_$$");
    addParam(rslFunc, "Generic_$$");
    addParam(rslFunc, "Generic_$$");
    addGenericType(rslFunc, 0, 1);
    addSymbol(root, rslFunc);
    // ==============  bool true, false as Dummies ===============
    // TODO: Introduce constants
    rslFunc = CreateObject("false", "false", 0, Variable, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_GLOBAL);
    setFlags(rslFunc, FLAG_NO_CODEGEN);
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("true", "true", 0, Variable, "bool");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_GLOBAL);
    setFlags(rslFunc, FLAG_NO_CODEGEN);
    addSymbol(root, rslFunc);

    // ==============  Looping Functions ===============

    rslFunc =
        CreateObject("while", "bool" COMPILER_SEP "while" COMPILER_SEP, 0, Function,
                     "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("while", "while" COMPILER_SEP "bool", 0, Function, "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("for", "for" COMPILER_SEP "int" COMPILER_SEP "int", 0,
                     Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_ASSIGNMENT);
    addParam(rslFunc, "int");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("for",
                     "for" COMPILER_SEP "int" COMPILER_SEP "int" COMPILER_SEP
                     "int", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_ASSIGNMENT);
    addParam(rslFunc, "int");
    addParam(rslFunc, "int");
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    // ==============  Print Functions ===============
    rslFunc =
            CreateObject("print", "print" COMPILER_SEP "Char", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "print" COMPILER_SEP "int", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print" COMPILER_SEP "float", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "print" COMPILER_SEP "String", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    rslFunc =
            CreateObject("echo", "echo" COMPILER_SEP "Char", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "int", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "float", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "String", 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    // ==============  System Functions ===============

    rslFunc = CreateObject("args", "args" COMPILER_SEP "int", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "int");
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("args", "args" COMPILER_SEP, 0, Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addSymbol(root, rslFunc);

    // ==============  int Functions ===============
    rslFunc =
        CreateObject("int", "int" COMPILER_SEP "int" COMPILER_SEP "String", 0,
                     Function, "int");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    // ==============  Condreturn Functions ===============
    rslFunc =
        CreateObject("condreturn",
                     "bool" COMPILER_SEP "condreturn" COMPILER_SEP GENERIC_PARAM "0",
                     0, Function, "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "bool");
    addSymbol(root, rslFunc);
}
