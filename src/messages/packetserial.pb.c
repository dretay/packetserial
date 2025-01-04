/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.3 at Sat Jan  4 03:17:16 2025. */

#include "packetserial.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t UnionMessage_fields[4] = {
    PB_FIELD(  1, MESSAGE , OPTIONAL, STATIC  , FIRST, UnionMessage, device_descriptor, device_descriptor, &DeviceDescriptor_fields),
    PB_FIELD(  2, MESSAGE , OPTIONAL, STATIC  , OTHER, UnionMessage, hid_report, device_descriptor, &HIDReport_fields),
    PB_FIELD(  3, MESSAGE , OPTIONAL, STATIC  , OTHER, UnionMessage, input_report, hid_report, &InputReport_fields),
    PB_LAST_FIELD
};

const pb_field_t Packet_fields[5] = {
    PB_FIELD(  1, BYTES   , REQUIRED, STATIC  , FIRST, Packet, data, data, 0),
    PB_FIELD(  2, UINT32  , REQUIRED, STATIC  , OTHER, Packet, sequence_number, data, 0),
    PB_FIELD(  3, UINT32  , REQUIRED, STATIC  , OTHER, Packet, crc, sequence_number, 0),
    PB_FIELD(  4, UENUM   , REQUIRED, STATIC  , OTHER, Packet, flag, crc, 0),
    PB_LAST_FIELD
};



/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(UnionMessage, device_descriptor) < 65536 && pb_membersize(UnionMessage, hid_report) < 65536 && pb_membersize(UnionMessage, input_report) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_UnionMessage_Packet)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(UnionMessage, device_descriptor) < 256 && pb_membersize(UnionMessage, hid_report) < 256 && pb_membersize(UnionMessage, input_report) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_UnionMessage_Packet)
#endif


/* @@protoc_insertion_point(eof) */