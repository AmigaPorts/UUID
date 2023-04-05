#include <proto/exec.h>
#include "uuid_private.h"

#define CHECK(f1, f2) if (f1 != f2) return f1 < f2 ? -1 : 1;

int _UUID_Compare(
    REGARG(const uuid_t * u1, "a0"),
    REGARG(const uuid_t * u2, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    int i;

    if (u1 == NULL || u2 == NULL)
        return -2;
    
    CHECK(u1->time_low, u2->time_low); 
    CHECK(u1->time_mid, u2->time_mid); 
    
    CHECK(u1->time_hi_and_version, u2->time_hi_and_version); CHECK(u1->clock_seq_hi_and_reserved, u2->clock_seq_hi_and_reserved); 
    CHECK(u1->clock_seq_low, u2->clock_seq_low); 

    for (i = 0; i < 6; i++)
    {
        if (u1->node[i] < u2->node[i])
            return -1;
        if (u1->node[i] > u2->node[i])
            return 1;
    }
    return 0;
}
