#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/uuid.h>
#include <proto/timer.h>
#include <proto/dos.h>

#include <stdint.h>
#include "uuid_private.h"

static inline ULONG ROL(ULONG x, int amount)
{
    return (x << amount) | (x >> (32 - amount));
}

static inline ULONG ROR(ULONG x, int amount)
{
    return (x >> amount) | (x << (32 - amount));
}

static inline void CopyBlock(const UBYTE *src, UBYTE *dst, WORD length)
{
    if (length == 0) return;

    while(length--) { *dst++ = *src++; }
}

void sha1(
    REGARG(const UBYTE * message, "a0"),
    REGARG(ULONG length, "d0"),
    REGARG(ULONG h[5], "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    /* Prepare the string */
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;

    if (length != 0)
    {
        ULONG bit_length = length << 3;         // Bit length
        UBYTE bit_length_ext = length >> 29;    // Bit length, MSB

        union {
            UBYTE * u8;
            ULONG * u32;
        } ptr;

        /* Allocate work buffer */
        ULONG *w = AllocMem(80 * sizeof(ULONG), MEMF_ANY);
        ptr.u32 = w;

        if (w != NULL)
        {
            enum {
                NOT_DONE = 0,       // SHA-1 in progress
                MARKER_WRITTEN,     // End marker written
                DONE                // SHA-1 completed
            } state = NOT_DONE;

            /* Init sha1 hash */
            h[0] = 0x67452301;
            h[1] = 0xEFCDAB89;
            h[2] = 0x98BADCFE;
            h[3] = 0x10325476;
            h[4] = 0xC3D2E1F0;

            while (state != DONE)
            {
                /* Get 16 longwords from message */
                if (length >= 64)
                {
                    /* Enough data. Copy entire 64 bytes */
                    CopyBlock(message, ptr.u8, 64);
                    message += 64;
                    length -= 64;
                }
                else if (length >= 55)
                {
                    /* Not enough data, but enough to put marker */
                    CopyBlock(message, ptr.u8, length);
                    
                    /* Fill rest of message with zero */
                    for (int i=length; i < 64; i++)
                        ptr.u8[i] = 0;
                    
                    /* Put marker */
                    ptr.u8[length] = 0x80;
                    state = MARKER_WRITTEN;

                    length = 0;
                }
                else
                {
                    /* Not enough data, but enough to put marker and size */
                    CopyBlock(message, ptr.u8, length);
                    
                    /* Fill rest of message with zero */
                    for (int i=length; i < 64; i++)
                        ptr.u8[i] = 0;
                    
                    /* If marker was not yet written, put it now */
                    if (state != MARKER_WRITTEN)
                    {
                        ptr.u8[length] = 0x80;
                    }

                    /* Put size at end of the block */
                    ptr.u8[59] = bit_length_ext;
                    ptr.u8[60] = bit_length >> 24;
                    ptr.u8[61] = bit_length >> 16;
                    ptr.u8[62] = bit_length >> 8;
                    ptr.u8[63] = bit_length >> 0;

                    /* Ready once this iteration is through */
                    state = DONE;
                }

                /* Mix remaining 64 words */
                for (int i = 16; i < 80; i++)
                {
                    w[i] = ROL((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);
                }

                ULONG a = h[0];
                ULONG b = h[1];
                ULONG c = h[2];
                ULONG d = h[3];
                ULONG e = h[4];

                /* For space reasons do SHA-1 rounds in loop */
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

                    ULONG temp = ROL(a, 5) + f + e + k + w[i];
                    e = d;
                    d = c;
                    c = ROL(b, 30);
                    b = a;
                    a = temp;
                }

                /* Update hash */
                h[0] += a;
                h[1] += b;
                h[2] += c;
                h[3] += d;
                h[4] += e;
            }

            FreeMem(w, 80 * sizeof(ULONG));
        }
    }
}
