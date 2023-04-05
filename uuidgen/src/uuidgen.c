#define __NOLIBBASE__

#include <exec/execbase.h>
#include <libraries/uuid.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/uuid.h>
#include <stdlib.h>

int main(int);

/* Startup code including workbench message support */
int _start()
{
    struct ExecBase *SysBase = *(struct ExecBase **)4;
    struct Process *p = NULL;
    struct WBStartup *wbmsg = NULL;
    int ret = 0;

    p = (struct Process *)SysBase->ThisTask;

    if (p->pr_CLI == 0)
    {
        WaitPort(&p->pr_MsgPort);
        wbmsg = (struct WBStartup *)GetMsg(&p->pr_MsgPort);
    }

    ret = main(wbmsg ? 1 : 0);

    if (wbmsg)
    {
        Forbid();
        ReplyMsg((struct Message *)wbmsg);
    }

    return ret;
}

static const char version[] __attribute__((used)) = "$VER: " VERSION_STRING;

struct ExecBase * SysBase = NULL;
struct DosLibrary * DOSBase = NULL;
struct Library * UUIDBase = NULL;

#define RDA_TEMPLATE "RANDOM/S,AMOUNT/K/N"

enum {
    OPT_RANDOM,
    OPT_AMOUNT,

    OPT_COUNT
};

LONG result[OPT_COUNT];

int OpenLibs()
{
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 37);
    if (DOSBase == NULL)
        return 0;
    
    UUIDBase = OpenLibrary("uuid.library", 0);
    if (UUIDBase == NULL)
        return 0;
        
    return 1;
}

void CloseLibs()
{
    if (UUIDBase != NULL) CloseLibrary(UUIDBase);
    if (DOSBase != NULL) CloseLibrary((struct Library *)DOSBase);
}

int main(int wb)
{
    struct RDArgs *args;
    SysBase = *(struct ExecBase **)4;
    struct timerequest *tr;

    if (!OpenLibs())
    {
        CloseLibs();
        return -1;
    }
    
    args = ReadArgs(RDA_TEMPLATE, result, NULL);

    if (args)
    {
        ULONG amount = 1;
        BOOL random = result[OPT_RANDOM];
        if (result[OPT_AMOUNT])
        {
            amount = *(ULONG*)(result[OPT_AMOUNT]);
        }

        char uuidstr[UUID_STRLEN+1];
        uuid_t uuid;
        uuidstr[UUID_STRLEN] = 0;

        while(amount--)
        {
            /* Generate the uuid identifier */
            if (random)
                UUID_Generate(UUID_TYPE_DCE_RANDOM, &uuid);
            else
                UUID_Generate(UUID_TYPE_DCE_TIME, &uuid);
            
            /* unparse it into human-readable format */
            UUID_Unparse(&uuid, uuidstr);
            
            Printf("%s\n", (ULONG)&uuidstr[0]);
        }

        FreeArgs(args);
    }
    
    CloseLibs();

    return 0;
}
