#include <stdio.h>
#include "ece454rpc_types.h" 

return_type make_remote_call(
    const char *servernameorip,
    const int serverportnumber,
    const char *procedure_name,
    const int nparams,
    ...);

return_type make_remote_call(
    const char *servernameorip,
    const int serverportnumber,
    const char *procedure_name,
    const int nparams,
    ...)
{
    printf("%s\n", "make call");

    return_type r;
    return r;
}
