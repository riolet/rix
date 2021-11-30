#ifndef RSL_LIST_H
#define RSL_LIST_H

#include "ObjectTree.h"
#include "rsl/rsl.h"

typedef struct StructList{
    IDENT_MPTR_RAW  *data;
    struct StructList *next;
    bool isPrimitive;
} StructList;


void List_$_destructor_$_ (StructList ** a);

#define _$_List_object_at_1(_$v$_arr__,_$v$_idx__) _$v$_arr__[_$v$_idx__]
#define _$_List_object_at_0(_$v$_arr__,_$v$_idx__) &_$v$_arr__[_$v$_idx__]

#define _$_List_set_object_0(_$v$_list, _$v$_elem) ({\
        IDENT_MPTR_RAW * _$v$_dataptr = (IDENT_MPTR_RAW *) _$v$_list->data;\
        IDENT_MPTR_RAW_assign(_$v$_dataptr, _$v$_elem);\
})

#define _$_List_set_object_1(_$v$_list, _$v$_elem) ({\
        /*--Todo-- This must get the right type */\
        typeof(_$v$_elem) * _$v$_list_data = _$v$_list->data;\
        * _$v$_list_data = _$v$_elem;\
        })

#define List_$_List_$_(_$v$_mptr, _$v$_primYtpe, _$v$_type) ({\
    /* Create Empty List  */ \
    StructList  * _$v$_arr = 0;\
    _$_returnAppointer(_$v$_mptr,_$v$_arr,List_$_destructor_$_);\
    _$v$_mptr;\
})


#define _$_List_$_create_new_node(_$v$_list, _$v$_elem, _$v$_primYtpe, _$v$_type) ({\
    StructList  * _$v$_list = calloc(1,sizeof(StructList));\
    xcat(_$_List_set_object_,_$v$_primYtpe)(_$v$_list, _$v$_elem);\
    _$v$_list->isPrimitive = _$v$_primYtpe;\
    _$v$_list->next=0;\
})

// #define List_$_append_$_Generic_$$(_$v$_arr, _$v$_elem, _$v$_primElem,\
//                                                     _$v$_mptr, _$v$_primRet, _$v$_typeRet) ({\
//     _StructList * _$v$_arrptr = (_StructList *) _$v$_arr->obj;\
//     if (_$v$_arrptr) {\
//         while (_$v$_arrptr->next) {\
//            _$v$_arrptr = (_StructList *) _$v$_arrptr->next;\
//         }\
//         _StructList * _$v$_arrptr_new;\
//         _$_List_$_create_new_list(_$v$_arrptr_new, _$v$_primElem, typeof(_$v$_elem));\
//         xcat(_$_List_set_object_,_$v$_primRet)(_$v$_arrptr_new, _$v$_elem);\
//         _$v$_arrptr->next = (StructList *) _$v$_arrptr_new;\
//     }\
//     else {\
//         _$_List_$_create_new_list(_$v$_arrptr, _$v$_primElem, typeof(_$v$_elem));\
//         xcat(_$_List_set_object_,_$v$_primRet)(_$v$_arrptr, _$v$_elem);\
//     }\
//     _$v$_mptr;\
// })

#define List_$_append_$_Generic_$$(_$v$_arr, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet) ({\
    StructList * _$v$_arrptr = _$v$_arr->obj;\
    StructList * _$v$_arrptr_prev = _$v$_arrptr;\
    StructList * _$v$_new_list;\
    _$_List_$_create_new_node(_$v$_new_list, _$v$_elem, _$v$_primElem, typeof(_$v$_elem));\
    if (!_$v$_arrptr) {\
        _$v$_arr->obj=_$v$_new_list;\
    } else {\
        while (_$v$_arrptr->next) {\
            _$v$_arrptr = _$v$_arrptr->next;\
        }\
        _$v$_arrptr->next=_$v$_new_list;\
    }\
    _$v$_mptr;\
})

#define _$_List_get_object_0 (_$v$_list, _$v$_mptr, primRet, typeRet) ({\
    StructList * arrptr = _$v$_list->obj;\
    typeRet *dataptr = (typeRet *) arrptr->data;\
    return *dataptr;\
})

#define _$_List_get_object_1 (_$v$_list, _$v$_mptr, primRet, typeRet) ({\
    StructList * arrptr = __$v$_list->obj;\
    typeRet *dataptr = (typeRet *) arrptr->data;\
    _$_returnAppointer(_$v$_mptr,_$v$_arr,List_$_destructor_$_);\
})


#define List_$_data_$_(_$v$_list, _$v$_mptr, primRet, typeRet) ({ \
    xcat(_$_List_get_object_,_$v$_primYtpe)(_$v$_list, _$v$_mptr, primRet, typeRet);\
    _$v$_mptr;\
})


#define List_$_next_$_(_$v$_arr, _$v$_mptr, primRet, typeRet) ({ \
    StructList * arrptr = _$v$_arr->obj;\
    typeRet *dataptr = (typeRet *) arrptr->next; \
    _$_returnAppointer(_$v$_mptr,_$v$_arr,List_$_destructor_$_);\
})

#endif