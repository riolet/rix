#include "ext/Wafer/Wafer_.h"
#include "wafer.h"
#include "waferapi.h"

IDENT_MPTR_RAW * Request_$_Request_$_ (Request * request, IDENT_MPTR_RAW * $_mptr_in) {
    $_mptr_in->ctr=0;
    $_mptr_in->ptr=0;
    $_mptr_in->obj=request;
    $_mptr_in->destructor = NULL;
}

IDENT_MPTR_RAW * Response_$_Response_$_ (Response * response, IDENT_MPTR_RAW * $_mptr_in) {
    $_mptr_in->ctr=0;
    $_mptr_in->ptr=0;
    $_mptr_in->obj=response;
    $_mptr_in->destructor = NULL;
}

/* Print prints the param and a newline char */
int Response_$_print_$_String(IDENT_MPTR_RAW * response_, IDENT_MPTR_RAW * s_)
{
    String * s = (String *) s_->obj;
    Response * response = (Response *) response_->obj;
    int result = resPrintf(response,"%s",s->buffer);
    return result;
}
