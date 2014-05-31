#include <stdio.h>
#include "ece454rpc_types.h"

int main()
{
    int a = -10, b = 25, c = 50;
    return_type ans = make_remote_call("localhost",
	                               10000,
                                   "addtwo", 3,
	                               sizeof(int), (void *)(&a),
	                               sizeof(int), (void *)(&b),
                                   sizeof(int), (void *)(&c));
    int i = *(int *)(ans.return_val);
    printf("client, got result: %d\n", i);

    return 0;
}
