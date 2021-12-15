#include "List.h"

List _$_List_$_List_$_ ()
{
    List list = list_new();
    //list->free = (void *) _$_cleanup;
    return list;
}

void List_$_destructor_$_ (List * list)
{
    // debugPrintf("\nCleaning up list\n");
    //list->free=(void *) _$_cleanup;
    list_destroy(list);
}

void List_$_iterator_destructor_$_ (list_iterator_t ** list_iter)
{
    // debugPrintf("\nCleaning up list iterator\n");
    list_iterator_destroy(*list_iter);
}

List _$_List_$_append_non_primitive(List list, void * element)
{   
    list_rpush(&list, list_node_new(element));
    return list;
}

// bool _$_List_$_contains_non_primitive(NonPrimObj * ListObject, NonPrimObj * element) {
//     list_t * l = ListObject->obj;
//     void * val = element->obj;    
//     list_node_t *node = list_find(l, "some value");
// }