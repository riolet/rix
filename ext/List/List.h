#ifndef RSL_EXT_LIST_H
#define RSL_EXT_LIST_H

#include "errors.h"
#include "rsl/rsl.h"
#include "ext/List/cliblist.h"

typedef list_t * List;

void List_$_destructor_$_ (List * list);
List _$_List_$_append_non_primitive(List list_, void * element);
/* bool _$_List_$_contains_non_primitive(NonPrimObj * list_, NonPrimObj * element); */

#define _$_List_$_append_0(_$v$_list, _$v$_elem) ({list_rpush(_$v$_list, list_node_new(_$v$_elem));_$v$_list;})
#define _$_List_$_append_1(_$v$_list, _$v$_elem) ({})

#define _$_List_$_contains_0(_$v$_list, _$v$_elem) _$_List_$_contains_non_primitive(_$v$_list, _$v$_elem)
#define _$_List_$_contains_1(_$v$_list, _$v$_elem) ({})

#define List_$non_primitive$_(_$v$_type)  List __attribute__ ((__cleanup__(List_$_destructor_$_)))
#define List_$primitive$_(_$v$_type)  List __attribute__ ((__cleanup__(List_$_destructor_$_)))

#define List_$_foreach_$_Generic_$$(_$v$_list, _$v$_elem, _$v$_primElem) \
    for (  struct { list_node_t *node; list_iterator_t *it;} xcat(loop_struct,__LINE__) = {0,list_iterator_new(_$v$_list, LIST_HEAD)};\
            (xcat(loop_struct,__LINE__).node = list_iterator_next(xcat(loop_struct,__LINE__).it));) {\
            NonPrimObj_assign(_$v$_elem,xcat(loop_struct,__LINE__).node->val);            

List _$_List_$_List_$_ ();

#define List_$_List_$_(_$v$_primYtpe, _$v$_type) list_new()

#define List_$_append_$_Generic_$$(_$v$_list, _$v$_elem, _$v$_primElem) xcat(_$_List_$_append_,_$v$_primElem)(_$v$_list, _$v$_elem)


#define List_$_contains_$_Generic_$$(_$v$_list, _$v$_elem, _$v$_primElem) xcat(_$_List_$_contains_,_$v$_primElem)(_$v$_list, _$v$_elem)

#endif