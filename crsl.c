/* These should be written in pure Rix */

#include "ObjectTree.h"
#include "crsl.h"
#include "rixc.h"

void defineRSLSymbols(Object * root, bool waferSupport)
{
    Object *objBaseType, *temp2, *temp3, *temp4, *temp5, *numberType, *intType, *floatType,  *rslFunc;

    // ==============  Built-in Types ===============

    objBaseType = CreateObject(BASETYPE, BASETYPE, 0, Type, BASETYPE);
    setFlags(objBaseType, FLAG_EXTERNAL);
    addSymbol(root, objBaseType);

    temp2 =
        CreateObject("Generic_$$", "Generic_$$" COMPILER_SEP BASETYPE, objBaseType,
                     Type, "Generic_$$");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    temp2 =
            CreateObject("Generic_YTYPE$$", "Generic_YTYPE$$" COMPILER_SEP BASETYPE, objBaseType,
                         Type, "Generic_YTYPE$$");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    temp2 =
            CreateObject("Generic_ZTYPE$$", "Generic_ZTYPE$$" COMPILER_SEP BASETYPE, objBaseType,
                         Type, "Generic_ZTYPE$$");
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);

    // temp2 =
    //     CreateObject("bool", "bool" COMPILER_SEP BASETYPE, objBaseType, Type,
    //                  "bool");
    // setFlags(temp2, FLAG_EXTERNAL);
    // setFlags(temp2, FLAG_PRIMITIVE);
    // addSymbol(root, temp2);

    temp2 =
        CreateObject("tern", "tern" COMPILER_SEP BASETYPE, objBaseType, Type,
                     "tern");
    setFlags(temp2, FLAG_EXTERNAL);
    setFlags(temp2, FLAG_PRIMITIVE);
    addSymbol(root, temp2);

    temp4 =
        CreateObject("System", "System" COMPILER_SEP BASETYPE, numberType, Type,
                     "System");
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    temp4 =
            CreateObject(IDENT_MPTR, IDENT_MPTR COMPILER_SEP BASETYPE, objBaseType, Type,
                         IDENT_MPTR);
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    temp4 =
            CreateObject(IDENT_HEAP_MPTR, IDENT_HEAP_MPTR COMPILER_SEP BASETYPE, objBaseType, Type,
                         IDENT_HEAP_MPTR);
    setFlags(temp4, FLAG_EXTERNAL);
    addSymbol(root, temp4);

    // ==============  Basetype constructor ===============

    rslFunc =
            CreateObject(BASETYPE, BASETYPE COMPILER_SEP BASETYPE COMPILER_SEP,
                         0, Constructor, IDENT_MPTR);
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, BASETYPE);
    addSymbol(root, rslFunc);

    // ==============  Exponent functions ===============

    // ============== Sqrt functions ==============
    // ============== TODO sqrt(int) should be a double? ==============

    // ==============  String Functions ===============
//    rslFunc =
//            CreateObject("String", "String" COMPILER_SEP "String" COMPILER_SEP, 0,
//                         Constructor, IDENT_MPTR);
//    setFlags(rslFunc, FLAG_EXTERNAL);
//    addParam(rslFunc, "String");
//    addSymbol(root, rslFunc);

//     rslFunc =
//             CreateObject("assign", "String" COMPILER_SEP "String" COMPILER_SEP, 0,
//                          Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);

//     rslFunc =
//         CreateObject("assign", "String" COMPILER_SEP "assign" COMPILER_SEP "String", 0,
//                      Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);

//     rslFunc =
//         CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
//                      Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("plus", "int" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
//                      Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "int", 0,
//                      Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("plus", "double" COMPILER_SEP "plus" COMPILER_SEP "String", 0,
//                      Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "double");
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "double", 0,
//                      Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addParam(rslFunc, "double");
//     addSymbol(root, rslFunc);
//     rslFunc =
//             CreateObject("getObjectAtIndex", "String" COMPILER_SEP "getObjectAtIndex" COMPILER_SEP "int", 0,
//                          Function, "char");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);

//     // ==============  Conditional Functions ===============

//     rslFunc = CreateObject("if", "bool" COMPILER_SEP "if", 0, Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_SAVERESULT);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("elif", "bool" COMPILER_SEP "elif" COMPILER_SEP "bool", 0,
//                      Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_SAVERESULT);
//     addParam(rslFunc, "bool");
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("else", "bool" COMPILER_SEP "else", 0, Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_SAVERESULT);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);

//     rslFunc =
//         CreateObject("tf",
//                      "bool" COMPILER_SEP "tf" COMPILER_SEP GENERIC_PARAM
//                      COMPILER_SEP GENERIC_PARAM, 0, Function, "Generic_$$");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "bool");
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addGenericType(rslFunc, 0, 1);
//     addSymbol(root, rslFunc);
//     //============= Python style conditional Functions =============
//     rslFunc = CreateObject("if", "if" COMPILER_SEP "bool", 0, Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     //setFlags(rslFunc, FLAG_SAVERESULT);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("elif", "elif" COMPILER_SEP "bool", 0, Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     //setFlags(rslFunc, FLAG_SAVERESULT);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("else", "else" COMPILER_SEP, 0, Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     //setFlags(rslFunc, FLAG_SAVERESULT);
//     addSymbol(root, rslFunc);

//     //============= Ternary Functions =============
//     rslFunc =
//         CreateObject("compare", "int" COMPILER_SEP "compare" COMPILER_SEP "int",
//                      0, Function, "Ternary");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);

//     rslFunc =
//         CreateObject("gel",
//                      "Ternary" COMPILER_SEP "gel" COMPILER_SEP GENERIC_PARAM "0"
//                      COMPILER_SEP GENERIC_PARAM "1" COMPILER_SEP GENERIC_PARAM "2", 0,
//                      Function, "Generic_$$");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "Ternary");
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addGenericType(rslFunc, 0, 1);
//     addSymbol(root, rslFunc);

//     rslFunc =
//         CreateObject("leg",
//                      "Ternary" COMPILER_SEP "leg" COMPILER_SEP GENERIC_PARAM "0"
//                      COMPILER_SEP GENERIC_PARAM "1" COMPILER_SEP GENERIC_PARAM "2", 0,
//                      Function, "Generic_$$");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "Ternary");
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addGenericType(rslFunc, 0, 1);
//     addSymbol(root, rslFunc);
//     // ==============  bool true, false as Dummies ===============
//     // TODO: Introduce constants
//     rslFunc = CreateObject("false", "false", 0, Variable, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_GLOBAL);
//     setFlags(rslFunc, FLAG_NO_CODEGEN);
//     addSymbol(root, rslFunc);

//     rslFunc = CreateObject("true", "true", 0, Variable, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_GLOBAL);
//     setFlags(rslFunc, FLAG_NO_CODEGEN);
//     addSymbol(root, rslFunc);

//     // ==============  Looping Functions ===============

//     rslFunc =
//         CreateObject("while", "bool" COMPILER_SEP "while" COMPILER_SEP, 0, Function,
//                      "void");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("while", "while" COMPILER_SEP "bool", 0, Function, "void");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);

//     rslFunc =
//         CreateObject("for", "for" COMPILER_SEP "NUMBER" COMPILER_SEP "NUMBER", 0,
//                      Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_ASSIGNMENT);
//     addParam(rslFunc, "NUMBER");
//     addParam(rslFunc, "NUMBER");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("for",
//                      "for" COMPILER_SEP "NUMBER" COMPILER_SEP "NUMBER" COMPILER_SEP
//                      "NUMBER", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_ASSIGNMENT);
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addParam(rslFunc, "Generic_$$");
//     addSymbol(root, rslFunc);

    
//     // ==============  Print Functions ===============
//     rslFunc =
//             CreateObject("print", "print" COMPILER_SEP "char", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("print", "print" COMPILER_SEP "int", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("print", "print" COMPILER_SEP "double", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "double");
//     addSymbol(root, rslFunc);
//     rslFunc =
//         CreateObject("print", "print" COMPILER_SEP "String", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);

//     rslFunc =
//             CreateObject("echo", "echo" COMPILER_SEP "char", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("echo", "echo" COMPILER_SEP "int", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("echo", "echo" COMPILER_SEP "double", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "double");
//     addSymbol(root, rslFunc);
//     rslFunc = CreateObject("echo", "echo" COMPILER_SEP "String", 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);

//     // ==============  System Functions ===============

//     rslFunc = CreateObject("args", "args" COMPILER_SEP "int", 0, Function, "String");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "int");
//     addSymbol(root, rslFunc);

//     rslFunc = CreateObject("args", "args" COMPILER_SEP, 0, Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addSymbol(root, rslFunc);

//     // ==============  int Functions ===============
//     rslFunc =
//         CreateObject("int", "int" COMPILER_SEP "int" COMPILER_SEP "String", 0,
//                      Function, "int");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "String");
//     addSymbol(root, rslFunc);

//     // ==============  Condreturn Functions ===============
//     rslFunc =
//         CreateObject("condreturn",
//                      "bool" COMPILER_SEP "condreturn" COMPILER_SEP GENERIC_PARAM "0",
//                      0, Function, "void");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     addParam(rslFunc, "bool");
//     addSymbol(root, rslFunc);

//     if (waferSupport) {
//         // ==============  bool true, false as Dummies ===============
//         // TODO: Introduce constants
//         rslFunc = CreateObject("request", "request", 0, Variable, "Request");
//         setFlags(rslFunc, FLAG_EXTERNAL);
//         setFlags(rslFunc, FLAG_GLOBAL);
//         setFlags(rslFunc, FLAG_NO_CODEGEN);
//         addSymbol(root, rslFunc);

//         rslFunc = CreateObject("response", "response", 0, Variable, "Response");
//         setFlags(rslFunc, FLAG_EXTERNAL);
//         setFlags(rslFunc, FLAG_GLOBAL);
//         setFlags(rslFunc, FLAG_NO_CODEGEN);
//         addSymbol(root, rslFunc);
//     }

//     //To help with the bootstrapping
//     temp3 =
//         CreateObject("OBJ_TYPE", "OBJ_TYPE" COMPILER_SEP BASETYPE, objBaseType, Type, "OBJ_TYPE");
//     setFlags(temp3, FLAG_EXTERNAL);
//     setFlags(temp3, FLAG_PRIMITIVE);
//     addSymbol(root, temp3);

//     //dummy import function
//         rslFunc = CreateObject("import", "import" COMPILER_SEP "String", 0, Function, "bool");
//     setFlags(rslFunc, FLAG_EXTERNAL);
//     setFlags(rslFunc, FLAG_GLOBAL);
//     setFlags(rslFunc, FLAG_NO_CODEGEN);
//     addSymbol(root, rslFunc);
}
