#include <proto/exec.h>
#include "uuid_private.h"

void _UUID_Pack(
    REGARG(const uuid_t * uuid, "a0"),
    REGARG(UBYTE * out, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase **)4UL;
    
    ULONG *src = (ULONG *)(APTR)uuid;
    ULONG *dst = (ULONG *)(APTR)out;

    if (uuid == NULL || out == NULL)
        return;

    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
}
