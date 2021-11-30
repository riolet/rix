//TODO Move to RSL_ARRAy
#include "rsl/RSL_List.h"

void List_$_destructor_$_ (StructList ** a)
{
    debugPrintf("\nCleaning up List \n");

    StructList * list = (*a);
    StructList * previous = list;

    while (list) {        
        previous = list;
        _$_cleanup(previous->data);        
        list = previous->next;
        free(previous);
    }
}