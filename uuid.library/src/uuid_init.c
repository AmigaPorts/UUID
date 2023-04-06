/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>

#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/input.h>
#include <proto/timer.h>

#include <stdint.h>
#include "uuid_private.h"

int __attribute__((no_reorder)) _start()
{
    return -1;
}

extern const char libraryEnd;
extern const char libraryName[];
extern const char libraryIdString[];
extern const ULONG InitTable[];

const struct Resident RomTag __attribute__((used)) = {
    RTC_MATCHWORD,
    (struct Resident *)&RomTag,
    (APTR)&libraryEnd,
    RTF_AUTOINIT,
    LIBRARY_VERSION,
    NT_LIBRARY,
    LIBRARY_PRIORITY,
    (char *)((ULONG)&libraryName),
    (char *)((ULONG)&libraryIdString),
    (APTR)InitTable,
};

const char libraryName[] = LIBRARY_NAME;
const char libraryIdString[] = VERSION_STRING;

static struct uuid_base * OpenLib(
    REGARG(ULONG version, "d0"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    UUIDBase->uuid_LibNode.lib_OpenCnt++;
    UUIDBase->uuid_LibNode.lib_Flags &= ~LIBF_DELEXP;

    return UUIDBase;
}

static ULONG ExpungeLib(REGARG(struct uuid_base * UUIDBase, "a6"))
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;
    BPTR segList = 0;

    if (UUIDBase->uuid_LibNode.lib_OpenCnt == 0)
    {
        /* Remove library from Exec's list */
        Remove(&UUIDBase->uuid_LibNode.lib_Node);

        /* Close all eventually opened libraries */
        CloseDevice((struct IORequest *)&UUIDBase->uuid_TR);

        if (UUIDBase->uuid_DOSBase != NULL)
        {
            /* Store the state */
            SetVar("uuid_state", (UBYTE*)&UUIDBase->uuid_State, sizeof(uuid_state_t),
                GVF_GLOBAL_ONLY | GVF_SAVE_VAR | LV_VAR);
        
            CloseLibrary((struct Library *)UUIDBase->uuid_DOSBase);
        }

        /* Save seglist */
        segList = UUIDBase->uuid_SegList;

        /* Remove I2C_Base itself - free the memory */
        ULONG size = UUIDBase->uuid_LibNode.lib_NegSize + UUIDBase->uuid_LibNode.lib_PosSize;
        FreeMem((APTR)((ULONG)UUIDBase - UUIDBase->uuid_LibNode.lib_NegSize), size);
    }
    else
    {
        /* Library is still in use, set delayed expunge flag */
        UUIDBase->uuid_LibNode.lib_Flags |= LIBF_DELEXP;
    }

    /* Return 0 or segList */
    return segList;
}

static ULONG CloseLib(REGARG(struct uuid_base * UUIDBase, "a6"))
{
    if (UUIDBase->uuid_LibNode.lib_OpenCnt != 0)
        UUIDBase->uuid_LibNode.lib_OpenCnt--;

    if (UUIDBase->uuid_LibNode.lib_OpenCnt == 0)
    {
        if (UUIDBase->uuid_LibNode.lib_Flags & LIBF_DELEXP)
            return ExpungeLib(UUIDBase);
    }

    return 0;
}

static uint32_t ExtFunc()
{
    return 0;
}

static uint64_t splitmix64(uint64_t state) {
	uint64_t result = state + 0x9E3779B97f4A7C15;
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}

struct uuid_base * LibInit(
    REGARG(struct uuid_base * base, "d0"),
    REGARG(BPTR seglist, "a0"),
    REGARG(struct ExecBase * SysBase, "a6"))
{
    struct timeval tv;
    uuid_time_t time_now;

    struct uuid_base *UUIDBase = base;
    UUIDBase->uuid_SegList = seglist;
    UUIDBase->uuid_LibNode.lib_Revision = LIBRARY_REVISION;
    UUIDBase->uuid_SysBase = SysBase;
   
    /* Set up global lock (class and interface locks are separate!) */
    InitSemaphore(&UUIDBase->uuid_GlobalLock);
    
    /* UUID not yet initialized */
    UUIDBase->uuid_Initialized = 0;

    /* I need timer.device in order to obtain system time */
    if (OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *)&UUIDBase->uuid_TR, 0))
    {
        /* timer.device failed. what to do... */
        return NULL;
    }

    /* get the system time and convert it to UUID time */
    GetSysTime(&tv);
    time_now = UUIDBase->uuid_NextUpdate = UUIDBase->uuid_LastTime = 
        ((uint64_t)tv.tv_secs + 2922) * 10000000 +
        ((uint64_t)tv.tv_micro) * 10 +
        ((uint64_t)0x01B21DD213814000LL);

    /* Seed the random generator */
    UUIDBase->uuid_RandomSeed = splitmix64(time_now); 
    UUIDBase->uuid_UUIDs_ThisTick = 0;

    /* Try to open dos.library for GetVar/SetVar */
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
    if (DOSBase)
    {       
        /* DOS is there. Try to get the last UUID state. */
        if (GetVar("uuid_state", (UBYTE*)&UUIDBase->uuid_State, sizeof(uuid_state_t),
               GVF_BINARY_VAR | GVF_DONT_NULL_TERM) == sizeof(uuid_state_t))
        {
            /* UUID is initialized now */
            UUIDBase->uuid_Initialized = 1;
        }
    }

    return UUIDBase;
}


int _UUID_Compare(
    REGARG(const uuid_t * u1, "a0"),
    REGARG(const uuid_t * u2, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_GetNameSpace(
    REGARG(uuid_namespace_t NameSpace, "d0"),
    REGARG(uuid_t * uuid, "a0"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

int _UUID_Parse(
    REGARG(const char * in, "a0"),
    REGARG(uuid_t * uuid, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_Unparse(
    REGARG(const uuid_t * uuid, "a0"),
    REGARG(char * out, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_Pack(
    REGARG(const uuid_t * uuid, "a0"),
    REGARG(UBYTE * out, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_Unpack(
    REGARG(const UBYTE * in, "a0"),
    REGARG(uuid_t * uuid, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_Clear(
    REGARG(uuid_t * uuid, "a0"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_Copy(
    REGARG(const uuid_t * src, "a0"),
    REGARG(uuid_t * dst, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_Generate(
    REGARG(uuid_type_t type, "d0"),
    REGARG(uuid_t * uuid, "a0"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

void _UUID_GenerateV5(
    REGARG(const uuid_t *ns, "a0"),
    REGARG(const char *string, "a1"),
    REGARG(uuid_t * uuid, "a2"),
    REGARG(struct uuid_base * UUIDBase, "a6"));

static ULONG functable[] = {
    (ULONG)OpenLib,
    (ULONG)CloseLib,
    (ULONG)ExpungeLib,
    (ULONG)ExtFunc,
    (ULONG)_UUID_Compare,
    (ULONG)_UUID_GetNameSpace,
    (ULONG)_UUID_Parse,
    (ULONG)_UUID_Unparse,
    (ULONG)_UUID_Pack,
    (ULONG)_UUID_Unpack,
    (ULONG)_UUID_Clear,
    (ULONG)_UUID_Copy,
    (ULONG)_UUID_Generate,
    (ULONG)_UUID_GenerateV5,
    -1
};

const ULONG InitTable[4] = {
    sizeof(struct uuid_base),
    (ULONG)functable,
    0,
    (ULONG)LibInit
};
