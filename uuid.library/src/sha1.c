#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/uuid.h>
#include <proto/timer.h>
#include <proto/dos.h>

#include <stdint.h>
#include "uuid_private.h"

static int _strlen(const char *str)
{
    int i = 0;

    if (str != NULL)
    {
        while (*str++) i++;
    }
    
    return i;
}

void sha1(
    REGARG(const char * string, "a0"),
    REGARG(ULONG h[5], "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    /* Prepare the string */
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;
    int length = _strlen(string);

    if (length != 0)
    {
        /*
            Length shall contain bit 1 at end padded with zeros to 64 byte boundary.
            Finally, at the end of message there must be 64-bit Length.
        */
        int padded_length = (length + 3 + 63) & ~63;

        union {
            char * u8;
            ULONG * u32;
        } ptr;
        ptr.u8 = AllocMem(padded_length, MEMF_ANY | MEMF_CLEAR);
        
        if (ptr.u8 != NULL)
        {
            ULONG *w = AllocMem(80 * 4, MEMF_ANY);

            if (w != NULL)
            {
                /* Copy string to message block */
                for (int i=0; i < length; i++)
                {
                    ptr.u8[i] = string[i];
                }

                /* End bit */
                ptr.u8[length] = 0x80;

                /* Store original length as number of bits */
                ptr.u32[padded_length / 4 - 1] = length << 3;   /* Bit length! */
                ptr.u8[padded_length - 5] = length >> 29;       /* overflow stored here! */

                /* Init sha1 hash */
                h[0] = 0x67452301;
                h[1] = 0xEFCDAB89;
                h[2] = 0x98BADCFE;
                h[3] = 0x10325476;
                h[4] = 0xC3D2E1F0;

                for (int pos = 0; pos < padded_length; pos+=64)
                {
                    /* Get 16 longwords from message */
                    CopyMem(&ptr.u8[pos], w, 64);

                    /* Mix remaining 64 words */
                    for (int i = 16; i < 80; i++)
                    {
                        w[i] = (w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]);
                        w[i] = (w[i] >> 31) | (w[i] << 1);
                    }

                    ULONG a = h[0];
                    ULONG b = h[1];
                    ULONG c = h[2];
                    ULONG d = h[3];
                    ULONG e = h[4];

                    for (int i=0; i < 80; i++)
                    {
                        ULONG f, k;

                        if (i < 20)
                        {
                            f = (b & c) | (~b & d);
                            k = 0x5A827999;
                        }
                        else if(i < 40)
                        {
                            f = b ^ c ^ d;
                            k = 0x6ED9EBA1;
                        }
                        else if (i < 60)
                        {
                            f = (b & c) | (b & d) | (c & d);
                            k = 0x8F1BBCDC;
                        }
                        else
                        {
                            f = b ^ c ^ d;
                            k = 0xCA62C1D6;
                        }

                        ULONG temp = ((a << 5) | (a >> (32 - 5))) + f + e + k + w[i];
                        e = d;
                        d = c;
                        c = (b << 30) | (b >> 2);
                        b = a;
                        a = temp;
                    }

                    h[0] += a;
                    h[1] += b;
                    h[2] += c;
                    h[3] += d;
                    h[4] += e;
                }

                FreeMem(w, 80 * 4);
            }
                        
            FreeMem(ptr.u8, padded_length);
        }
    }
}
