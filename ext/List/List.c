#include "List.h"

NonPrimObj * _$_List_$_List_$_ (NonPrimObj * $_mptr_in)
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

NonPrimObj * _$_List_$_append_non_primitive(NonPrimObj * list, NonPrimObj * element, NonPrimObj * $_mptr_in)
{   
    _$_TEMP_OBJ(xcat($_TEMP_OBJ,__LINE__))
    list_t * l = list->obj;
    list_rpush(l, list_node_new(element));
    element->ptr=list;
    list->ctr++;    
    $_mptr_in=list;
}

bool _$_List_$_contains_non_primitive(NonPrimObj * ListObject, NonPrimObj * element) {
    list_t * l = ListObject->obj;
    void * val = element->obj;    
    list_node_t *node = list_find(l, "some value");
}