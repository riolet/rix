#include "ObjectTree.h"
#include "rsl/rsl.h"
#include "rixc.rix_ext.h"
#include "rsl/RSL_String.h"
#include "ext/List/List.h"

IDENT_MPTR_RAW *toRixString(char *c, IDENT_MPTR_RAW *left)
{
    _$_TEMP_OBJ($_mptr_temp);
    IDENT_MPTR_RAW *s_ = String_$_String_$_(&$_mptr_temp);
    String *s = (String *)s_->obj;
    s->buffer = malloc(sizeof(char) * (strlen(c) + 1));
    memcpy(s->buffer, c, strlen(c));
    s->cap = strlen(c);
    s->length = strlen(c);
    s->isStaticBuffer = false;
    _$_mptr_prepare(&$_mptr_temp, left);
    return left;
}

IDENT_MPTR_RAW *toRixListString(char *c, IDENT_MPTR_RAW *left)
{
    List list = list_new();
    
    return left;
}

IDENT_MPTR_RAW *toRixObject(Object *o, IDENT_MPTR_RAW *left)
{
     _$_U_VARIABLE(name);
    toRixString(o->name, name);

    _$_U_VARIABLE(fullname);
    toRixString(o->fullname, fullname);

    _$_U_VARIABLE(parentClass);
    toRixObject(o->parentClass, parentClass);

    _$_U_VARIABLE(parentScope);
    toRixObject(o->parentScope, parentScope);

    OBJ_TYPE category = o->category;

    _$_U_VARIABLE(returnType);
    toRixString(o->returnType, returnType);

    _$_U_VARIABLE(genericType);
    toRixString(o->genericType, genericType);

    int genericTypeArgPos = o->genericTypeArgPos;

    _$_U_VARIABLE(resolvedSpecificType);
    toRixString(o->resolvedSpecificType, resolvedSpecificType);

    _$_U_VARIABLE(paramTypes);
    toRixListString(o->paramTypes, paramTypes);

    _$_U_VARIABLE(definedSymbols);
    toRixListObject(o->definedSymbols, definedSymbols);

    _$_U_VARIABLE(code);
    toRixListString(o->code, code);

    int flags = o->flags;

    _$_U_VARIABLE(object_R);

    Object_R_$_Object_R_$_String_$_String_$_Object_R_$_Object_R_$_OBJ_TYPE_R_$_String_$_String_$_int_$_String_$_ListString_R_$_ListObject_R_$_ListString_R_$_int(
        name,
        fullname,
        parentClass,
        parentScope,
        category,
        returnType,
        genericType,
        genericTypeArgPos,
        resolvedSpecificType,
        paramTypes,
        definedSymbols,
        code,
        flags);
    return left;
}

Object *verbGetObjAtIdx_R()
{
    compilerDebugPrintf("verbGetObjAtIdx(#)\n");

    _$_U_VARIABLE(retVal);
    _$_TEMP_OBJ($_mptr_temp);

    String_$_stringlit("getObjectAtIndex", &$_mptr_temp);
    thisIsRidiculous_$_String(&$_mptr_temp, retVal);

    String *s = retVal->obj;
    Object *result = CreateObject(s->buffer, s->buffer, 0, Function, 0);

    return result;
}
