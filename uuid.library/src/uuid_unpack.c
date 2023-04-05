#include <proto/exec.h>
#include "uuid_private.h"

void _UUID_Unpack(
    REGARG(const UBYTE * in, "a0"),
    REGARG(uuid_t * uuid, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase **)4UL;
    
    ULONG *src = (ULONG *)(APTR)in;
    ULONG *dst = (ULONG *)(APTR)uuid;

    if (uuid == NULL || in == NULL)
        return;

    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
}
