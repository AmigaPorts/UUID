#include <proto/exec.h>
#include "uuid_private.h"

static char value(char chr)
{
    if (chr >= '0' && chr <= '9')
        chr = chr - '0';
    else if (chr >= 'a' && chr <= 'f')
        chr = 10 + (chr - 'a');
    else if (chr >= 'A' && chr <= 'F')
        chr = 10 + (chr - 'F');
    
    return chr;
}

static int _isxdigit(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    
    if (c >= 'A' && c <= 'F')
        return 1;

    if (c >= 'a' && c <= 'f')
        return 1;

    return 0;
}

static int _strlen(const char *str)
{
    int i = 0;

    if (str != NULL)
    {
        while (*str++) i++;
    }
    
    return i;
}

int _UUID_Parse(
    REGARG(const char * in, "a0"),
    REGARG(uuid_t * uuid, "a1"),
    REGARG(struct uuid_base * UUIDBase, "a6"))
{
    int i;
    uuid_t tmp = {0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0}};
    
    if (_strlen(in) == UUID_STRLEN)
    {
        for (i=0; i < 8; i++, in++)
        {
            if (_isxdigit(*in))
            {
                tmp.time_low = (tmp.time_low  << 4) | value(*in); 
            }
            else return 0;
        }
        
        if (*in++ != '-')
            return 0;
        
        for (i=0; i < 4; i++, in++)
        {
            if (_isxdigit(*in))
            {
                tmp.time_mid = (tmp.time_mid << 4) | value(*in); 
            }
            else return 0;
        }

        if (*in++ != '-')
            return 0;
        
        for (i=0; i < 4; i++, in++)
        {
            if (_isxdigit(*in))
            {
                tmp.time_hi_and_version = 
                    (tmp.time_hi_and_version << 4) | value(*in); 
            }
            else return 0;
        }

        if (*in++ != '-')
            return 0;
                
        if (!(_isxdigit(in[0]) && _isxdigit(in[1])))
            return 0;
        
        tmp.clock_seq_hi_and_reserved = (value(in[0]) << 4) | value(in[1]);
        
        if (!(_isxdigit(in[2]) && _isxdigit(in[3])))
            return 0;
        
        tmp.clock_seq_low = (value(in[2]) << 4) | value(in[3]);
        in +=4;
        
        if (*in++ != '-')
            return 0;
        
        for (i=0; i < 6; i++, in+=2)
        {
            if (!(_isxdigit(in[0]) && _isxdigit(in[1])))
                return 0;

            tmp.node[i] = (value(in[0]) << 4) | value(in[1]);
        }
        
        if (uuid != NULL)
            *uuid = tmp;

        return 1;
    }
    
    return 0;
}
