#ifndef UUID_PRIVATE_H_
#define UUID_PRIVATE_H_

#include <exec/execbase.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <devices/timer.h>
#include <dos/dos.h>

#include <libraries/uuid.h>

typedef struct {
    uuid_time_t  ts;       /* saved timestamp */
    uuid_node_t  node;     /* saved node ID */
    UWORD        cs;       /* saved clock sequence */
} uuid_state_t;

struct uuid_base {
    struct Library          uuid_LibNode;
    struct ExecBase *       uuid_SysBase;
    struct SignalSemaphore  uuid_GlobalLock;
    struct DosLibrary *     uuid_DOSBase;
    struct timerequest      uuid_TR;
    BPTR                    uuid_SegList;
    uuid_state_t            uuid_State;
    uuid_time_t             uuid_NextUpdate;
    uuid_time_t             uuid_LastTime;
    ULONG                   uuid_RandomSeed;
    UWORD                   uuid_UUIDs_ThisTick;
    UBYTE                   uuid_Initialized;
};

typedef union {
    uuid_t uuid;
    ULONG  u32[4];
    UBYTE  u8[16];
} uuid_container_t;

#ifdef DOSBase
#undef DOSBase
#endif
#define DOSBase (UUIDBase->uuid_DOSBase)

#ifdef TimerBase
#undef TimerBase
#endif
#define TimerBase (UUIDBase->uuid_TR.tr_node.io_Device)

#define UUIDS_PER_TICK 1024

#if defined(__INTELLISENSE__)
#define REGARG(arg, reg) arg
#else
#define REGARG(arg, reg) arg asm(reg)
#endif

#endif /*UUID_PRIVATE_H_*/
