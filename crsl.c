#include "ObjectTree.h"
#include "crsl.h"
#include "ritc.h"

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
        CreateObject("Boolean", "Boolean" COMPILER_SEP "BaseType", objBaseType, Type,
                     "Boolean");
    setFlags(temp2, FLAG_EXTERNAL);
    setFlags(temp2, FLAG_PRIMITIVE);
    addSymbol(root, temp2);
    temp2 =
        CreateObject("Ternary", "Ternary" COMPILER_SEP "BaseType", objBaseType, Type,
                     "Ternary");
    setFlags(temp2, FLAG_EXTERNAL);
    setFlags(temp2, FLAG_PRIMITIVE);
    addSymbol(root, temp2);
    temp2 =
        CreateObject("Stream", "Stream" COMPILER_SEP "BaseType", objBaseType, Type,
                     "Stream");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
//    temp2 =
//        CreateObject("String", "String" COMPILER_SEP "BaseType", objBaseType, Type,
//                     "String");
//    setFlags(temp2, FLAG_EXTERNAL);
//    addSymbol(root, temp2);

    temp2 =
        CreateObject("Number", "Number" COMPILER_SEP "BaseType", objBaseType, Type,
                     "Number *");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
    temp3 =
        CreateObject("Integer", "Integer" COMPILER_SEP "Number", temp2, Type, "Integer");
    setFlags(temp3, FLAG_EXTERNAL);
    setFlags(temp3, FLAG_PRIMITIVE);
    addSymbol(root, temp3);
    temp3 = CreateObject("Float", "Float" COMPILER_SEP "Number", temp2, Type, "Float");
    setFlags(temp3, FLAG_EXTERNAL);
    setFlags(temp3, FLAG_PRIMITIVE);
    addSymbol(root, temp3);
    temp3 = CreateObject("Char", "Char" COMPILER_SEP "Number", temp2, Type, "Char");
    setFlags(temp3, FLAG_EXTERNAL);
    setFlags(temp3, FLAG_PRIMITIVE);
    addSymbol(root, temp3);

    temp4 =
        CreateObject("System", "System" COMPILER_SEP "BaseType", objBaseType, Type,
                     "System");
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    temp4 =
            CreateObject(IDENT_RETVAR, IDENT_RETVAR COMPILER_SEP "BaseType", objBaseType, Type,
                         IDENT_RETVAR);
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    temp4 =
            CreateObject(IDENT_HEAP_RETVAR, IDENT_HEAP_RETVAR COMPILER_SEP "BaseType", objBaseType, Type,
                         IDENT_HEAP_RETVAR);
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    // ==============  Basetype constructor ===============

    rslFunc =
            CreateObject("BaseType", "BaseType" COMPILER_SEP "BaseType",
                         0, Function, IDENT_RETVAR);
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "BaseType");
    addSymbol(root, rslFunc);

    // ==============  Exponent functions ===============

    rslFunc =
        CreateObject("exponent", "Float" COMPILER_SEP "exponent" COMPILER_SEP "Integer",
                     0, Function, "Float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("exponent", "Integer" COMPILER_SEP "exponent" COMPILER_SEP "Float",
                     0, Function, "Float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("exponent", "Float" COMPILER_SEP "exponent" COMPILER_SEP "Float", 0,
                     Function, "Float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("exponent", "Integer" COMPILER_SEP "exponent" COMPILER_SEP "Integer",
                     0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ============== Sqrt functions ==============
    // ============== TODO sqrt(Integer) should be a float? ==============
    rslFunc =
        CreateObject("sqrt", "Integer" COMPILER_SEP "sqrt" COMPILER_SEP, 0, Function,
                     "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  String Functions ===============

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
        CreateObject("plus", "Integer" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "Integer", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "Float" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "Float", 0,
                     Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc =
            CreateObject("getObjectAtIndex", "String" COMPILER_SEP "getObjectAtIndex" COMPILER_SEP "Integer", 0,
                         Function, "Char");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  Conditional Functions ===============

    rslFunc = CreateObject("if", "Boolean" COMPILER_SEP "if", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("elif", "Boolean" COMPILER_SEP "elif" COMPILER_SEP "Boolean", 0,
                     Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "Boolean");
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("else", "Boolean" COMPILER_SEP "else", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("tf",
                     "Boolean" COMPILER_SEP "tf" COMPILER_SEP GENERIC_PARAM "0"
                     COMPILER_SEP GENERIC_PARAM "1", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addParam(rslFunc, "Generic_$$");
    addParam(rslFunc, "Generic_$$");
    addSymbol(root, rslFunc);
    //============= Python style conditional Functions =============
    rslFunc = CreateObject("if", "if" COMPILER_SEP "Boolean", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    //setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("elif", "elif" COMPILER_SEP "Boolean", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    //setFlags(rslFunc, FLAG_SAVERESULT);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("else", "else" COMPILER_SEP, 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    //setFlags(rslFunc, FLAG_SAVERESULT);
    addSymbol(root, rslFunc);

    //============= Ternary Functions =============
    rslFunc =
        CreateObject("compare", "Integer" COMPILER_SEP "compare" COMPILER_SEP "Integer",
                     0, Function, "Ternary");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
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
    // ==============  Boolean true, false as Dummies ===============
    // TODO: Introduce constants
    rslFunc = CreateObject("false", "false" COMPILER_SEP, 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("true", "true" COMPILER_SEP, 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addSymbol(root, rslFunc);

    // ==============  Looping Functions ===============

    rslFunc =
        CreateObject("while", "Boolean" COMPILER_SEP "while" COMPILER_SEP, 0, Function,
                     "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("while", "while" COMPILER_SEP "Boolean", 0, Function, "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);

    rslFunc =
        CreateObject("for", "for" COMPILER_SEP "Integer" COMPILER_SEP "Integer", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_ASSIGNMENT);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("for",
                     "for" COMPILER_SEP "Integer" COMPILER_SEP "Integer" COMPILER_SEP
                     "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_ASSIGNMENT);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  Print Functions ===============
    rslFunc =
            CreateObject("print", "print" COMPILER_SEP "Char", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "print" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print" COMPILER_SEP "Float", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "print" COMPILER_SEP "String", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "Stream" COMPILER_SEP "print" COMPILER_SEP "Integer", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "Stream" COMPILER_SEP "print" COMPILER_SEP "Float", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("print", "Stream" COMPILER_SEP "print" COMPILER_SEP "String", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "Float", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "String", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("echo", "Stream" COMPILER_SEP "echo" COMPILER_SEP "Integer", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("echo", "Stream" COMPILER_SEP "echo" COMPILER_SEP "loat", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc =
        CreateObject("echo", "Stream" COMPILER_SEP "echo" COMPILER_SEP "String", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    // ==============  System Functions ===============

    rslFunc = CreateObject("args", "args" COMPILER_SEP "Integer", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  Integer Functions ===============
    rslFunc =
        CreateObject("Integer", "Integer" COMPILER_SEP "Integer" COMPILER_SEP "String", 0,
                     Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    // ==============  Condreturn Functions ===============
    rslFunc =
        CreateObject("condreturn",
                     "Boolean" COMPILER_SEP "condreturn" COMPILER_SEP GENERIC_PARAM "0",
                     0, Function, "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
}
