/* Automatically generated header (sfdc 1.11e)! Do not edit! */

#ifndef PROTO_UUID_H
#define PROTO_UUID_H

#include <clib/uuid_protos.h>

#ifndef _NO_INLINE
# if defined(__GNUC__)
#  ifdef __AROS__
#   include <defines/uuid.h>
#  else
#   include <inline/uuid.h>
#  endif
# else
#  include <pragmas/uuid_pragmas.h>
# endif
#endif /* _NO_INLINE */

#ifdef __amigaos4__
# include <interfaces/uuid.h>
# ifndef __NOGLOBALIFACE__
   extern struct UUIDIFace *IUUID;
# endif /* __NOGLOBALIFACE__*/
#endif /* !__amigaos4__ */
#ifndef __NOLIBBASE__
  extern APTR
# ifdef __CONSTLIBBASEDECL__
   __CONSTLIBBASEDECL__
# endif /* __CONSTLIBBASEDECL__ */
  UUIDBase;
#endif /* !__NOLIBBASE__ */

#endif /* !PROTO_UUID_H */
