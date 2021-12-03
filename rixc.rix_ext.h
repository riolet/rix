#ifndef RIXC_RIX__H
#define RIXC_RIX__H

#include "ObjectTree.h"
#include "rsl/rsl.h"

#define OBJ_TYPE OBJ_TYPE_R

IDENT_MPTR_RAW * toRixString(char *c, IDENT_MPTR_RAW * left);
Object *beginClass_R (char *className, char *parentName, Object *typeArgs, bool isPrimitive);
Object *verbGetObjAtIdx_R();

#endif