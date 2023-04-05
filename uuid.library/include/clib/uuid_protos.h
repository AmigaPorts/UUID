/* Automatically generated header (sfdc 1.11e)! Do not edit! */

#ifndef CLIB_UUID_PROTOS_H
#define CLIB_UUID_PROTOS_H

/*
**   $VER: uuid_protos.h 0.1.0 $Id: uuid_lib.sfd 0.1.0 $
**
**   C prototypes. For use with 32 bit integers only.
**
**   Copyright (c) 2001 Amiga, Inc.
**       All Rights Reserved
*/

#include <libraries/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* "uuid.library" */
int UUID_Compare(const uuid_t * u1, const uuid_t * u2);
void UUID_GetNameSpace(uuid_namespace_t NameSpace, uuid_t * uuid);
int UUID_Parse(const char * in, uuid_t * uuid);
void UUID_Unparse(const uuid_t * uuid, char * out);
void UUID_Pack(const uuid_t * uuid, UBYTE * out);
void UUID_Unpack(const UBYTE * in, uuid_t * uuid);
void UUID_Clear(uuid_t * uuid);
void UUID_Copy(const uuid_t * src, uuid_t * dst);
void UUID_Generate(uuid_type_t type, uuid_t * uuid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CLIB_UUID_PROTOS_H */
