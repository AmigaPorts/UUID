#ifndef LIBRARIES_UUID_H
#define LIBRARIES_UUID_H

#include <exec/types.h>

typedef unsigned long long uuid_time_t;

typedef struct {
    UBYTE    nodeID[6]; 
} uuid_node_t;
 
typedef struct {
    ULONG   time_low;
    UWORD   time_mid;
    UWORD   time_hi_and_version; 
    UBYTE   clock_seq_hi_and_reserved;
    UBYTE   clock_seq_low;
    UBYTE   node[6];
} uuid_t;

typedef enum {
    UUID_NAMESPACE_DNS = 1,
    UUID_NAMESPACE_URL,
    UUID_NAMESPACE_OID,
    UUID_NAMESPACE_X500
} uuid_namespace_t;

typedef enum {
    UUID_TYPE_DCE_TIME = 1,
    UUID_TYPE_DCE_RANDOM = 4
} uuid_type_t;

typedef enum {
    UUID_VARIANT_NCS = 0,
    UUID_VARIANT_DCE = 1,
    UUID_VARIANT_MICROSOFT = 2,
    UUID_VARIANT_OTHER = 3
} uuid_variant_t;

#define UUID_STRLEN     36

#endif /*LIBRARIES_UUID_H*/
