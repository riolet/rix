//TODO Move to RSL_ARRAy
#include "rsl/RSL_Array.h"

void Array_$_destructor_$_ (StructArray ** a)
{
    debugPrintf("\nCleaning up array \n");
    debugPrintf("Array last %d\n",(int) (*a)->last);
    int i;
    for (i = 0; i <= (*a)->last; i++) {
        if (testBit((*a)->bitArray, i)) {
            debugPrintf("Cleaning at %d\n", i);
            _$_cleanup(&(*a)->data[i]);
        } else {
            debugPrintf("Nothing to clean at %d\n", i);
        }
    }
}