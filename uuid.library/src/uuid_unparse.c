#include <proto/exec.h>
#include "uuid_private.h"

static void output(char *ptr, ULONG val, int size)
{
    ptr += size;
    while (size--)
    {
        int n = val & 15;
        
        if (n < 10)
            *--ptr = '0' + n;
        else
            *--ptr = 'a' + (n - 10);
        
        val = val >> 4;
    }
}

void _UUID_Unparse(
    REGARG(const uuid_t * uuid, "a0"),
    REGARG(char * out, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;

    out[8] = '-';
    out[13] = '-';
    out[18] = '-';
    out[23] = '-';

    output(&out[0], uuid->time_low, 8);
    output(&out[9], uuid->time_mid, 4);
    output(&out[14], uuid->time_hi_and_version, 4);
    
    output(&out[19], uuid->clock_seq_hi_and_reserved, 2);
    output(&out[21], uuid->clock_seq_low, 2);

    for (int i=0; i < 6; i++)
    {
        output(&out[24 + i * 2], uuid->node[i], 2);
    }
}
