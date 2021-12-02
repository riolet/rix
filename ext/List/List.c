#include "List.h"

IDENT_MPTR_RAW * _$_List_$_List_$_ (IDENT_MPTR_RAW * $_mptr_in)
{
    list_t *list = list_new();
    //list->free = (void *) _$_cleanup;
    return _$_returnAppointer($_mptr_in,list,List_$_destructor_$_);
}

void List_$_destructor_$_ (list_t ** list)
{
    debugPrintf("\nCleaning up list\n");
    (*list)->free=(void *) _$_cleanup;
    list_destroy(*list);
}

void List_$_iterator_destructor_$_ (list_iterator_t ** list_iter)
{
    debugPrintf("\nCleaning up list iterator\n");
    list_iterator_destroy(*list_iter);
}

IDENT_MPTR_RAW * _$_List_$_append_non_primitive(IDENT_MPTR_RAW * list, IDENT_MPTR_RAW * element, IDENT_MPTR_RAW * $_mptr_in)
{   
    _$_TEMP_OBJ(xcat($_TEMP_OBJ,__LINE__))
    list_t * l = list->obj;
    list_rpush(l, list_node_new(element));
    element->ptr=list;
    list->ctr++;    
    $_mptr_in=list;
}

bool _$_List_$_contains_non_primitive(IDENT_MPTR_RAW * ListObject, IDENT_MPTR_RAW * element) {
    list_t * l = ListObject->obj;
    void * val = element->obj;    
    list_node_t *node = list_find(l, "some value");
}