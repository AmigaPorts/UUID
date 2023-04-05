#include <proto/exec.h>
#include "uuid_private.h"

void _UUID_Copy(
    REGARG(const uuid_t * src, "a0"),
    REGARG(uuid_t * dst, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    if (src != NULL && dst != NULL)
    {
        *dst = *src;
    }   
}
