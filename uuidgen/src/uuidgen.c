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

#define RDA_TEMPLATE "RANDOM/S,AMOUNT/K/N,NAMESPACE/K,STRING/K"

enum {
    OPT_RANDOM,
    OPT_AMOUNT,
    OPT_NAMESPACE,
    OPT_STRING,

    OPT_COUNT
};

LONG result[OPT_COUNT];

int OpenLibs()
{
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 37);
    if (DOSBase == NULL)
        return 0;       
    
    UUIDBase = OpenLibrary("uuid.library", 2);
    if (UUIDBase == NULL)
    {
        Printf("This program required uuid.library version 2 or higher\n");
        return 0;
    }
        
        
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

        if (result[OPT_NAMESPACE] || result[OPT_STRING])
        {
            char *ns_str = (char *)result[OPT_NAMESPACE];
            char *str = (char *)result[OPT_STRING];
            
            if (ns_str != NULL && str != NULL)
            {
                uuid_t ns;
                uuid_t uuid;
                int success = 1;

                /* Get either pre-defined namespace or parse specified uuid */
                if (ns_str[0] == 'U' && ns_str[1] == 'R' && ns_str[2] == 'L' && ns_str[3] == 0)
                {
                    UUID_GetNameSpace(UUID_NAMESPACE_URL, &ns);
                }
                else if (ns_str[0] == 'D' && ns_str[1] == 'N' && ns_str[2] == 'S' && ns_str[3] == 0)
                {
                    UUID_GetNameSpace(UUID_NAMESPACE_DNS, &ns);
                }
                else if (ns_str[0] == 'O' && ns_str[1] == 'I' && ns_str[2] == 'D' && ns_str[3] == 0)
                {
                    UUID_GetNameSpace(UUID_NAMESPACE_OID, &ns);
                }
                else if (ns_str[0] == 'X' && ns_str[1] == '5' && ns_str[2] == '0' && ns_str[3] == '0' &&ns_str[4] == 0)
                {
                    UUID_GetNameSpace(UUID_NAMESPACE_X500, &ns);
                }
                else
                {
                    success = UUID_Parse(ns_str, &ns);              
                }

                if (success)
                {
                    UUID_GenerateV5(&ns, str, &uuid);
                    char uuidstr[UUID_STRLEN+1];
                    uuidstr[UUID_STRLEN] = 0;
                
                    /* unparse uuid into human-readable format */
                    UUID_Unparse(&uuid, uuidstr);
                
                    /* Print result */
                    Printf("%s\n", (ULONG)&uuidstr[0]);
                }
                else
                {
                    Printf("ERROR: Invalid namespace! Namespace must be either valid UUID\n");
                    Printf("       or one of DNS, URL, OID or X500\n");
                }
            }
            else
            {
                Printf("ERROR: Both NAMESPACE and STRING must be specified\n");
            }
        }
        else
        {
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
        }

        FreeArgs(args);
    }
    
    CloseLibs();

    return 0;
}
