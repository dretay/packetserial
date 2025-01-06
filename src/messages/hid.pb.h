/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9.3 at Mon Jan  6 03:33:52 2025. */

#ifndef PB_HID_PB_H_INCLUDED
#define PB_HID_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _DeviceDescriptor {
    char iManufacturer[64];
    char iProduct[64];
    uint32_t bMaxPacketSize0;
/* @@protoc_insertion_point(struct:DeviceDescriptor) */
} DeviceDescriptor;

typedef struct _HIDReport {
    char data[256];
/* @@protoc_insertion_point(struct:HIDReport) */
} HIDReport;

typedef struct _InputReport {
    char data[64];
/* @@protoc_insertion_point(struct:InputReport) */
} InputReport;

/* Default values for struct fields */

/* Initializer values for message structs */
#define DeviceDescriptor_init_default            {"", "", 0}
#define HIDReport_init_default                   {""}
#define InputReport_init_default                 {""}
#define DeviceDescriptor_init_zero               {"", "", 0}
#define HIDReport_init_zero                      {""}
#define InputReport_init_zero                    {""}

/* Field tags (for use in manual encoding/decoding) */
#define DeviceDescriptor_iManufacturer_tag       1
#define DeviceDescriptor_iProduct_tag            2
#define DeviceDescriptor_bMaxPacketSize0_tag     3
#define HIDReport_data_tag                       1
#define InputReport_data_tag                     1

/* Struct field encoding specification for nanopb */
extern const pb_field_t DeviceDescriptor_fields[4];
extern const pb_field_t HIDReport_fields[2];
extern const pb_field_t InputReport_fields[2];

/* Maximum encoded size of messages (where known) */
#define DeviceDescriptor_size                    138
#define HIDReport_size                           259
#define InputReport_size                         66

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define HID_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
