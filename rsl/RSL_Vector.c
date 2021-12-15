//TODO Move to RSL_ARRAy
#include "rsl/RSL_Array.h"
#include "rsl/RSL_Vector.h"

void Vector_$_destructor_$_ (NonPrimObj * a)
{
    debugPrintf("\nCleaning up array \n");
    StructVector * arrptr = (StructVector *) a->obj;
    NonPrimObj * dataptr = (NonPrimObj *) arrptr->data;
    debugPrintf("Array last %d size %d\n",(int) arrptr->length,(int) arrptr->size);

    int i;
    if (!arrptr->isPrimitive) {
        for (i = 0; i <= arrptr->length; i++) {
            if (testBit(arrptr->bitVector, i)) {
                debugPrintf("Clean at %d\n", i);
                _$_cleanup(&dataptr[i]);
            } else {
                debugPrintf("Nothing to clean at %d\n", i);
            }
        }
    }
    free(arrptr->bitVector);
    free(arrptr->data);
    free(arrptr);
}