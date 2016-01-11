#ifndef RSL_ARRAY_H
#define RSL_ARRAY_H

#include "ObjectTree.h"
#include "rsl/rsl.h"

enum { WORD_SIZE = sizeof(uintmax_t) * sizeof(char) };

typedef struct {
    size_t last;
    uintmax_t *bitArray;
    IDENT_MPTR_RAW  *data;
} StructArray;

#define WORD_OFFSET(b) ((b) / WORD_SIZE)
#define BIT_OFFSET(b)  ((b) % WORD_SIZE)

#define setBit(w, i) ({w[WORD_OFFSET(i)] |= (1 << BIT_OFFSET(i));})
#define clearBit(w, i) ({w[WORD_OFFSET(i)] &= ~(1 << BIT_OFFSET(i));})
#define testBit(w,i) ({(w[WORD_OFFSET(i)] & (1 << BIT_OFFSET(i)))!=0;})


void Array_$_destructor_$_ (StructArray ** a);

//#define _$_Array_type_0(_$v$_type) _$v$_type *
#define _$_Array_type_0(_$v$_type) StructArray __attribute__ ((__cleanup__(Array_$_destructor_$_))) *
#define _$_Array_type_1(_$v$_type) _$v$_type *

#define _$_Array_Declare_0(_$v$_size__,_$v$_type) StructArray * _$v$_arr = alloca(sizeof(StructArray));
#define _$_Array_Declare_1(_$v$_size__,_$v$_type) _$v$_type _$v$_arr[_$v$_size__]

#define _$_Array_Initialize_0(_$v$_size__,_$v$_arr)  ({ \
    _$v$_arr->last=0;\
    _$v$_arr->bitArray=alloca(((_$v$_size__/WORD_SIZE)+1) * sizeof(uintmax_t));\
    memset (_$v$_arr->bitArray,0,((_$v$_size__/WORD_SIZE)+1) * sizeof(uintmax_t)); \
    _$v$_arr->data = (void *) alloca(_$v$_size__ * sizeof(IDENT_MPTR_RAW));\
    memset (_$v$_arr->data,0,_$v$_size__ * sizeof(IDENT_MPTR_RAW)); \
})

#define _$_Array_Initialize_1(_$v$_size__,_$v$_type) //

#define _$_Array_object_at_0(_$v$_arr__,_$v$_idx__) &_$v$_arr__->data[_$v$_idx__]
#define _$_Array_object_at_1(_$v$_arr__,_$v$_idx__) _$v$_arr__[_$v$_idx__]


#define _$_Array_set_object_at_0(_$v$_arr, _$v$_idx, _$v$_elem) ({\
    if (_$v$_idx>_$v$_arr->last) {\
        _$v$_arr->last=_$v$_idx;\
        debugPrintf("Last %d\n",_$v$_idx);\
    }\
    setBit(_$v$_arr->bitArray,_$v$_idx);\
    IDENT_MPTR_RAW_assign(&_$v$_arr->data[_$v$_idx], _$v$_elem);\
})

#define _$_Array_set_object_at_1(_$v$_arr, _$v$_idx, _$v$_elem) ({\
        _$v$_arr[_$v$_idx] = _$v$_elem;\
        })

#define Array_$_Array_$_int(_$v$_size__, _$v$_primYtpe, _$v$_type) ({\
    xcat(_$_Array_Declare_,_$v$_primYtpe)(_$v$_size__,_$v$_type);\
    xcat(_$_Array_Initialize_,_$v$_primYtpe)(_$v$_size__,_$v$_arr);\
    _$v$_arr;\
})


#define Array_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$v$_arr, _$v$_idx, _$v$_primIdx, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet) ({\
    xcat(_$_Array_set_object_at_,_$v$_primRet)(_$v$_arr, _$v$_idx, _$v$_elem);\
    _$v$_mptr;\
})


#define Array_$_getObjectAtIndex_$_int(_$v$_arr, _$v$_idx, _$v$_mptr, _$v$_primRet, _$v$_typeRet) ({ \
    xcat(_$_Array_object_at_,_$v$_primRet)(_$v$_arr,_$v$_idx);                              \
})

#endif