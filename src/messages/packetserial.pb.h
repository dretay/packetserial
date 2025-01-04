/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9.3 at Sat Jan  4 03:17:16 2025. */

#ifndef PB_PACKETSERIAL_PB_H_INCLUDED
#define PB_PACKETSERIAL_PB_H_INCLUDED
#include <pb.h>

#include "hid.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _Packet_Flag {
    Packet_Flag_FIRST = 0,
    Packet_Flag_CONTINUE = 1,
    Packet_Flag_LAST = 2,
    Packet_Flag_FIRSTLAST = 3
} Packet_Flag;
#define _Packet_Flag_MIN Packet_Flag_FIRST
#define _Packet_Flag_MAX Packet_Flag_FIRSTLAST
#define _Packet_Flag_ARRAYSIZE ((Packet_Flag)(Packet_Flag_FIRSTLAST+1))

/* Struct definitions */
typedef PB_BYTES_ARRAY_T(16) Packet_data_t;
typedef struct _Packet {
    Packet_data_t data;
    uint8_t sequence_number;
    uint8_t crc;
    Packet_Flag flag;
/* @@protoc_insertion_point(struct:Packet) */
} Packet;

typedef struct _UnionMessage {
    bool has_device_descriptor;
    DeviceDescriptor device_descriptor;
    bool has_hid_report;
    HIDReport hid_report;
    bool has_input_report;
    InputReport input_report;
/* @@protoc_insertion_point(struct:UnionMessage) */
} UnionMessage;

/* Default values for struct fields */

/* Initializer values for message structs */
#define UnionMessage_init_default                {false, DeviceDescriptor_init_default, false, HIDReport_init_default, false, InputReport_init_default}
#define Packet_init_default                      {{0, {0}}, 0, 0, _Packet_Flag_MIN}
#define UnionMessage_init_zero                   {false, DeviceDescriptor_init_zero, false, HIDReport_init_zero, false, InputReport_init_zero}
#define Packet_init_zero                         {{0, {0}}, 0, 0, _Packet_Flag_MIN}

/* Field tags (for use in manual encoding/decoding) */
#define Packet_data_tag                          1
#define Packet_sequence_number_tag               2
#define Packet_crc_tag                           3
#define Packet_flag_tag                          4
#define UnionMessage_device_descriptor_tag       1
#define UnionMessage_hid_report_tag              2
#define UnionMessage_input_report_tag            3

/* Struct field encoding specification for nanopb */
extern const pb_field_t UnionMessage_fields[4];
extern const pb_field_t Packet_fields[5];

/* Maximum encoded size of messages (where known) */
#define UnionMessage_size                        471
#define Packet_size                              32

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define PACKETSERIAL_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif