/* Automatically generated header (sfdc 1.11e)! Do not edit! */

#ifndef _INLINE_UUID_H
#define _INLINE_UUID_H

#ifndef _SFDC_VARARG_DEFINED
#define _SFDC_VARARG_DEFINED
#ifdef __HAVE_IPTR_ATTR__
typedef APTR _sfdc_vararg __attribute__((iptr));
#else
typedef ULONG _sfdc_vararg;
#endif /* __HAVE_IPTR_ATTR__ */
#endif /* _SFDC_VARARG_DEFINED */

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif /* !__INLINE_MACROS_H */

#ifndef UUID_BASE_NAME
#define UUID_BASE_NAME UUIDBase
#endif /* !UUID_BASE_NAME */

#define UUID_Compare(___u1, ___u2) \
      LP2(0x1e, int, UUID_Compare , const uuid_t *, ___u1, a0, const uuid_t *, ___u2, a1,\
      , UUID_BASE_NAME)

#define UUID_GetNameSpace(___NameSpace, ___uuid) \
      LP2NR(0x24, UUID_GetNameSpace , uuid_namespace_t, ___NameSpace, d0, uuid_t *, ___uuid, a0,\
      , UUID_BASE_NAME)

#define UUID_Parse(___in, ___uuid) \
      LP2(0x2a, int, UUID_Parse , const char *, ___in, a0, uuid_t *, ___uuid, a1,\
      , UUID_BASE_NAME)

#define UUID_Unparse(___uuid, ___out) \
      LP2NR(0x30, UUID_Unparse , const uuid_t *, ___uuid, a0, char *, ___out, a1,\
      , UUID_BASE_NAME)

#define UUID_Pack(___uuid, ___out) \
      LP2NR(0x36, UUID_Pack , const uuid_t *, ___uuid, a0, UBYTE *, ___out, a1,\
      , UUID_BASE_NAME)

#define UUID_Unpack(___in, ___uuid) \
      LP2NR(0x3c, UUID_Unpack , const UBYTE *, ___in, a0, uuid_t *, ___uuid, a1,\
      , UUID_BASE_NAME)

#define UUID_Clear(___uuid) \
      LP1NR(0x42, UUID_Clear , uuid_t *, ___uuid, a0,\
      , UUID_BASE_NAME)

#define UUID_Copy(___src, ___dst) \
      LP2NR(0x48, UUID_Copy , const uuid_t *, ___src, a0, uuid_t *, ___dst, a1,\
      , UUID_BASE_NAME)

#define UUID_Generate(___type, ___uuid) \
      LP2NR(0x4e, UUID_Generate , uuid_type_t, ___type, d0, uuid_t *, ___uuid, a0,\
      , UUID_BASE_NAME)

#endif /* !_INLINE_UUID_H */
