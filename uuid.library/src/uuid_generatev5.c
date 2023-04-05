#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/uuid.h>
#include <proto/timer.h>
#include <proto/dos.h>
#include <proto/uuid.h>

#include <stdint.h>
#include "uuid_private.h"

static ULONG _strlen(const char *str)
{
    ULONG i = 0;

    if (str != NULL)
    {
        while (*str++) i++;
    }
    
    return i;
}

void _UUID_GenerateV5(
    REGARG(const uuid_t *ns, "a0"),
    REGARG(const char *string, "a1"),
    REGARG(uuid_t * uuid, "a2"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;
    union {
        ULONG u32[5];
        UBYTE u8[20];
    } digest;

    APTR message;
    ULONG length;

    if (uuid == NULL || string == NULL || ns == NULL)
        return;
    
    /* Get required length extended by size of uuid. Don't include trailing zero of string */
    length = _strlen(string);
    length += sizeof(uuid_t);

    message = AllocMem(length, MEMF_ANY);

    if (message != NULL)
    {
        /* First pack the namespace uuid to the buffer */
        UUID_Pack(ns, message);
        
        /* Copy the string */
        CopyMem((APTR)string, (APTR)((ULONG)message + sizeof(uuid_t)), length - sizeof(uuid_t));
        
        /* Get SHA-1 digest from the message */
        sha1(message, length, digest.u32, UUIDBase);

        /* Construct uuid from the hash */
        uuid->time_low = digest.u32[0];
        uuid->time_mid = (digest.u8[4] << 8) | digest.u8[5];
        uuid->time_hi_and_version = ((digest.u8[6] << 8) | digest.u8[7]) & 0x0fff;
        /* Set uuid version to 5: SHA-1 based */
        uuid->time_hi_and_version |= 0x5000;
        uuid->clock_seq_hi_and_reserved = (digest.u8[8] & 0x3f) | 0x80;
        uuid->clock_seq_low = digest.u8[9];
        for (int i=0; i < 6; i++)
        {
            uuid->node[i] = digest.u8[10 + i];
        }

        /* Release memory, UUID construction done */
        FreeMem(message, length);
    }
}
