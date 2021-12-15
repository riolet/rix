#include "ext/Wafer/Wafer_.h"
#include "wafer.h"
#include "waferapi.h"

NonPrimObj * Request_$_Request_$_ (Request * request, NonPrimObj * $_mptr_in) {
    $_mptr_in->ctr=0;
    $_mptr_in->ptr=0;
    $_mptr_in->obj=request;
    $_mptr_in->destructor = NULL;
}

NonPrimObj * Response_$_Response_$_ (Response * response, NonPrimObj * $_mptr_in) {
    $_mptr_in->ctr=0;
    $_mptr_in->ptr=0;
    $_mptr_in->obj=response;
    $_mptr_in->destructor = NULL;
}

/* Print prints the param and a newline char */
int Response_$_print_$_String(NonPrimObj * response_, NonPrimObj * s_)
{
    String * s = (String *) s_->obj;
    Response * response = (Response *) response_->obj;
    int result = resPrintf(response,"%s",s->buffer);
    return result;
}
