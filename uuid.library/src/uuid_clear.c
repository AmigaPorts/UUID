#include <proto/exec.h>
#include "uuid_private.h"

void _UUID_Clear(
    REGARG(uuid_t * uuid, "a0"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    if (uuid != NULL)
    {
        uuid_container_t cont;
        cont.uuid = *uuid;
        cont.u32[0] = 0;
        cont.u32[1] = 0;
        cont.u32[2] = 0;
        cont.u32[3] = 0;
        *uuid = cont.uuid;
    }
}
