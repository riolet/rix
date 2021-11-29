#include "ObjectTree.h"
#include "rsl/rsl.h"
#include "rixc.rix_.h"
#include "rsl/RSL_String.h"

IDENT_MPTR_RAW * toRixString(char *c, IDENT_MPTR_RAW * left)
{
    _$_TEMP_OBJ($_mptr_temp);
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String *s = (String *) s_->obj;
    s->buffer = malloc(sizeof(char)*(strlen(c)+1));
    memcpy(s->buffer, c, strlen(c));
    s->cap = strlen(c);
    s->length = strlen(c);
    s->isStaticBuffer = false;
    _$_mptr_prepare(&$_mptr_temp, left);
    return left;
}

// Object *beginClass_R (char *className, char *parentName, Object *typeArgs, bool isPrimitive) {

// _$_VARIABLE(className_R);

// className_R=toRixString(className,className_R);

// _$_VARIABLE(parentName_R);
// parentName_R=toRixString(parentName,parentName_R);

// _$_VARIABLE(typeArgs_R);

// _$_U_VARIABLE(retVal);

// retVal = beginClass_$_String_$_String_$_Object_R_$_bool(className_R,parentName_R,typeArgs_R,isPrimitive, retVal);

// }

Object *verbGetObjAtIdx_R()
{
    compilerDebugPrintf("verbGetObjAtIdx(#)\n");    
    
    _$_U_VARIABLE(retVal);    
    _$_TEMP_OBJ($_mptr_temp);

    String_$_stringlit("getObjectAtIndex",&$_mptr_temp);
    thisIsRidiculous_$_String(&$_mptr_temp,retVal);

    String * s = retVal->obj;
    Object *result = CreateObject(s->buffer, s->buffer, 0, Function, 0);
    
    return result;
}
