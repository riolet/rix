#ifndef RSL_VECTOR_H
#define RSL_VECTOR_H

#include "ObjectTree.h"
#include "rsl/rsl.h"
#include "rsl/RSL_Array.h"

void Vector_$_destructor_$_ (IDENT_MPTR_RAW * a);

typedef struct {
    size_t size;
    size_t length;
    uintmax_t *bitVector;
    void  *data;
    bool isPrimitive;
} StructVector;

void Vector_$_destructor_$_ (IDENT_MPTR_RAW * a);

#define _$_Vector_object_at_1(_$v$_arr__,_$v$_idx__) _$v$_arr__[_$v$_idx__]
#define _$_Vector_object_at_0(_$v$_arr__,_$v$_idx__) &_$v$_arr__[_$v$_idx__]

#define _$_Vector_set_object_at_0(_$v$_arrptr, _$v$_idx, _$v$_elem) ({\
        IDENT_MPTR_RAW * _$v$_dataptr = (IDENT_MPTR_RAW *) _$v$_arrptr->data;\
        IDENT_MPTR_RAW_assign(&_$v$_dataptr[_$v$_idx], _$v$_elem);\
})

#define _$_Vector_set_object_at_1(_$v$_arrptr, _$v$_idx, _$v$_elem) ({\
        /*--Todo-- This must get the right type */\
        typeof(_$v$_elem) * _$v$_dataptr = _$v$_arrptr->data;\
        _$v$_dataptr[_$v$_idx] = _$v$_elem;\
        })

#define Vector_$_Vector_$_int(_$v$_size__, _$v$_mptr, _$v$_primYtpe, _$v$_type) ({\
    StructVector  * _$v$_arr = calloc(1,sizeof(StructVector));\
    _$v$_arr->size=_$v$_size__;\
    _$v$_arr->length=0;\
    _$v$_arr->bitVector=calloc((_$v$_size__/WORD_SIZE)+1,sizeof(uintmax_t));\
    if (!_$v$_primYtpe) {\
        _$v$_arr->data = (void *) calloc(_$v$_size__, sizeof(IDENT_MPTR_RAW));\
    } else {\
        _$v$_arr->data = (void *) calloc(_$v$_size__, sizeof(_$v$_type));\
    }\
    _$v$_arr->isPrimitive = _$v$_primYtpe;\
    _$_returnAppointer(_$v$_mptr,_$v$_arr,Vector_$_destructor_$_);\
    _$v$_mptr;\
})


#define Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$v$_arr, _$v$_idx, _$v$_primIdx, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet) ({\
    StructVector * _$v$_arrptr = (StructVector *) _$v$_arr->obj;\
    if (_$v$_idx>_$v$_arrptr->length) {\
        _$v$_arrptr->length=_$v$_idx;\
    }\
    setBit(_$v$_arrptr->bitVector,_$v$_idx);\
    xcat(_$_Vector_set_object_at_,_$v$_primRet)(_$v$_arrptr, _$v$_idx, _$v$_elem);\
    _$v$_mptr;\
})


#define Vector_$_getObjectAtIndex_$_int(arr, idx, mptr, primRet, typeRet) ({ \
    StructVector * arrptr = (StructVector *) arr->obj;                        \
    typeRet *dataptr = (typeRet *) arrptr->data; \
    xcat(_$_Vector_object_at_,primRet)(dataptr,idx);                              \
})

#define Vector_$_isNull_$_int(_$v$_arr, _$v$_idx) ({ \
    StructVector * _$v$_arrptr = (StructVector *) _$v$_arr->obj;    \
    !testBit(_$v$_arrptr->bitVector, _$v$_idx);                                   \
})

#define Vector_$_length_$_(_$v$_arr) ({   \
    StructVector * _$v$_arrptr = (StructVector *) _$v$_arr->obj;\
    _$v$_arrptr->length;                        \
})

#define Vector_$_capacity_$_(_$v$_arr) ({  \
    StructVector * _$v$_arrptr = (StructVector *) _$v$_arr->obj;\
    _$v$_arrptr->size;                         \
})

#endif