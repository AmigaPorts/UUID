#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/uuid.h>
#include <proto/timer.h>
#include <proto/dos.h>

#include <stdint.h>
#include "uuid_private.h"

static void uuid_generate_random(uuid_t *uuid, struct uuid_base *UUIDBase);
static void uuid_generate_time(uuid_t *uuid, struct uuid_base *UUIDBase);

void _UUID_Generate(
    REGARG(uuid_type_t type, "d0"),
    REGARG(uuid_t * uuid, "a0"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;

    if (uuid == NULL)
        return;

    ObtainSemaphore(&UUIDBase->uuid_GlobalLock);

    if (type == UUID_TYPE_DCE_RANDOM)
        uuid_generate_random(uuid, UUIDBase);
    else
        uuid_generate_time(uuid, UUIDBase);

    ReleaseSemaphore(&UUIDBase->uuid_GlobalLock);
}

static ULONG uuid_rand(struct uuid_base *UUIDBase)
{
    return (UUIDBase->uuid_RandomSeed = 
        UUIDBase->uuid_RandomSeed * 1103515245 + 12345) % 0x7fffffff;
}

static void uuid_generate_random(uuid_t *uuid, struct uuid_base *UUIDBase)
{
    UBYTE u[16];
    int i;
   
    for (i=0; i < 16; i++)
        u[i] = uuid_rand(UUIDBase);
    
    UUID_Unpack(u, uuid);
    
    uuid->clock_seq_hi_and_reserved &= 0x3f;
    uuid->clock_seq_hi_and_reserved |= 0x80;
    uuid->time_hi_and_version &= 0x0fff;
    uuid->time_hi_and_version |= 0x4000;
}

static void uuid_get_current_time(uuid_time_t *time, struct uuid_base *UUIDBase)
{
    uuid_time_t time_now;
    struct timeval tv;
    
    for (;;)
    {
        GetSysTime(&tv);
        time_now = ((uint64_t)tv.tv_secs + 2922) * 10000000 +
                   ((uint64_t)tv.tv_micro) * 10 +
                   ((uint64_t)0x01B21DD213814000LL);

        if (time_now != UUIDBase->uuid_LastTime)
        {
            UUIDBase->uuid_UUIDs_ThisTick = 0;
            UUIDBase->uuid_LastTime = time_now;
            break;
        }
        
        if (UUIDBase->uuid_UUIDs_ThisTick < UUIDS_PER_TICK)
        {
            UUIDBase->uuid_UUIDs_ThisTick++;
            break;
        }
    }
    *time = time_now + UUIDBase->uuid_UUIDs_ThisTick;
}

static void uuid_get_node(uuid_node_t *node, struct uuid_base *UUIDBase)
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;

    if (!UUIDBase->uuid_Initialized)
    {
        if (!DOSBase)
            DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
        
        if (!(DOSBase && GetVar("uuid_state", (UBYTE*)&UUIDBase->uuid_State, sizeof(uuid_state_t),
                              GVF_BINARY_VAR | GVF_DONT_NULL_TERM) == sizeof(uuid_state_t)))
        {
            int i;
            UUIDBase->uuid_State.ts = UUIDBase->uuid_LastTime;
            UUIDBase->uuid_State.cs = uuid_rand(UUIDBase);
            for (i=0; i < 6; i++)
            {
                UUIDBase->uuid_State.node.nodeID[i] = uuid_rand(UUIDBase);                
            }
            UUIDBase->uuid_State.node.nodeID[0] |= 0x01;
        }
        UUIDBase->uuid_Initialized = 1;
        
        if (DOSBase)
            SetVar("uuid_state", (UBYTE*)&UUIDBase->uuid_State, sizeof(uuid_state_t),
                              GVF_BINARY_VAR | GVF_DONT_NULL_TERM | GVF_SAVE_VAR);
    }
    *node = UUIDBase->uuid_State.node;
}

static void uuid_get_state(uint16_t *cs, uuid_time_t *timestamp, uuid_node_t *node, struct uuid_base *UUIDBase)
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;

    if (!UUIDBase->uuid_Initialized)
    {
        if (!DOSBase)
            DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);

        if (!(DOSBase && GetVar("uuid_state", (UBYTE*)&UUIDBase->uuid_State, sizeof(uuid_state_t),
                                GVF_BINARY_VAR | GVF_DONT_NULL_TERM) == sizeof(uuid_state_t)))
        {
            int i;
            UUIDBase->uuid_State.ts = UUIDBase->uuid_LastTime;
            UUIDBase->uuid_State.cs = uuid_rand(UUIDBase);
            for (i=0; i < 6; i++)
            {
                UUIDBase->uuid_State.node.nodeID[i] = uuid_rand(UUIDBase);                
            }
            UUIDBase->uuid_State.node.nodeID[0] |= 0x01;
        }
        UUIDBase->uuid_Initialized = 1;
    }
    
    *node = UUIDBase->uuid_State.node;
    *timestamp = UUIDBase->uuid_State.ts;
    *cs = UUIDBase->uuid_State.cs;
}

static void uuid_set_state(uint16_t cs, uuid_time_t timestamp, uuid_node_t node, struct uuid_base *UUIDBase)
{
    struct ExecBase *SysBase = UUIDBase->uuid_SysBase;

    UUIDBase->uuid_State.node = node;
    UUIDBase->uuid_State.ts = timestamp;
    UUIDBase->uuid_State.cs = cs;
    
    if (timestamp >= UUIDBase->uuid_NextUpdate)
    {
        /* Update nonvolatile variable */       
        if (!DOSBase)
            DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
        
        if (DOSBase)
            SetVar("uuid_state", (UBYTE*)&UUIDBase->uuid_State, sizeof(uuid_state_t),
                              GVF_GLOBAL_ONLY | GVF_SAVE_VAR | LV_VAR);
        
        UUIDBase->uuid_NextUpdate = timestamp + (10 * 10 * 1000000);
    }    
}

static int _memcmp(APTR _src, APTR _dst, int size)
{
    UBYTE *src = _src;
    UBYTE *dst = _dst;

    while(size--)
        if (*src++ != *dst++)
            return 1;
}

static int _memcpy(APTR _dst, APTR _src, int size)
{
    UBYTE *src = _src;
    UBYTE *dst = _dst;
    
    while(size--)
        *dst++ = *src++;
}

static void uuid_generate_time(uuid_t *uuid, struct uuid_base *UUIDBase)
{
    uuid_time_t time, last_time;
    uuid_node_t node, last_node;
    uint16_t clockseq;
    
    uuid_get_current_time(&time, UUIDBase);
    uuid_get_node(&node, UUIDBase);
    uuid_get_state(&clockseq, &last_time, &last_node, UUIDBase);
    
    if (_memcmp(&node, &last_node, sizeof(node)))
        clockseq = uuid_rand(UUIDBase);
    else if (time < last_time)
        clockseq++;
    
    uuid_set_state(clockseq, time, node, UUIDBase);
    
    uuid->time_low = (uint32_t)(time & 0xFFFFFFFF);
    uuid->time_mid = (uint16_t)((time >> 32) & 0xFFFF);
    uuid->time_hi_and_version = (uint16_t)((time >> 48) & 0x0FFF); 
    uuid->time_hi_and_version |= (1 << 12); 
    uuid->clock_seq_low = clockseq & 0xFF;
    uuid->clock_seq_hi_and_reserved = (clockseq & 0x3F00) >> 8;
    uuid->clock_seq_hi_and_reserved |= 0x80;
    _memcpy(&uuid->node, &node, sizeof uuid->node);
}
