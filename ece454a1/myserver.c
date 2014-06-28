#include <stdio.h>
#include "ece454rpc_types.h"

int ret_int;
return_type r;

return_type add(const int nparams, arg_type* a)
{
    //return r;
    if(nparams < 2 ) {
	/* Error! */
        
	r.return_val = NULL;
	r.return_size = 0;
	return r;
    }

    if(a->arg_size != sizeof(int) ||
       a->next->arg_size != sizeof(int)) {
	/* Error! */
  //      printf("error in a->arg_size calc \n");
	r.return_val = NULL;
	r.return_size = 0;
	return r;
    }
    int i = *(int *)(a->arg_val);
    int j = *(int *)(a->next->arg_val);
//    printf("i: %i   j: %i \n",i,j);
    int k = *(int *)(a->next->next->arg_val);
//    printf ("k: %i\n",k);
    ret_int = i+j+k;
    r.return_val = (void *)(&ret_int);
    r.return_size = sizeof(int);
   // printf("value of return value: %d\n", (*(int *)r.return_val));
    return r;
}

int main() {
    register_procedure("addtwo", 3, add);
    register_procedure("add2",2,add);
    register_procedure("testAdd2",2,add);
    register_procedure("23s",2,add);
    launch_server();

    /* should never get here, because
       launch_server(); runs forever. */

    return 0;
}
