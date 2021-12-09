#ifndef RIXC_RIX__H
#define RIXC_RIX__H

#include "ObjectTree.h"
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"
#include "ext/List/List.h"

IDENT_MPTR_RAW * toRixString(char *c, IDENT_MPTR_RAW * left);
IDENT_MPTR_RAW * toRixListType(List * olist, ListType *lt);
IDENT_MPTR_RAW * toRixListString(List * olist, ListString *lt);

Object *beginClass_R (char *className, char *parentName, Object *typeArgs, bool isPrimitive);
Object *verbGetObjAtIdx_R();


#endif