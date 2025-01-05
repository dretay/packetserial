/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9.3 at Sun Jan  5 20:46:27 2025. */

#ifndef PB_ALARM_PB_H_INCLUDED
#define PB_ALARM_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _Alarm {
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
    bool enabled;
/* @@protoc_insertion_point(struct:Alarm) */
} Alarm;

typedef struct _Lighting {
    uint32_t color;
    uint32_t brightness;
/* @@protoc_insertion_point(struct:Lighting) */
} Lighting;

typedef struct _Speak {
    char data[256];
/* @@protoc_insertion_point(struct:Speak) */
} Speak;

typedef struct _Time {
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
/* @@protoc_insertion_point(struct:Time) */
} Time;

/* Default values for struct fields */

/* Initializer values for message structs */
#define Speak_init_default                       {""}
#define Time_init_default                        {0, 0, 0}
#define Alarm_init_default                       {0, 0, 0, 0}
#define Lighting_init_default                    {0, 0}
#define Speak_init_zero                          {""}
#define Time_init_zero                           {0, 0, 0}
#define Alarm_init_zero                          {0, 0, 0, 0}
#define Lighting_init_zero                       {0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define Alarm_hours_tag                          1
#define Alarm_minutes_tag                        2
#define Alarm_seconds_tag                        3
#define Alarm_enabled_tag                        4
#define Lighting_color_tag                       1
#define Lighting_brightness_tag                  2
#define Speak_data_tag                           1
#define Time_hours_tag                           1
#define Time_minutes_tag                         2
#define Time_seconds_tag                         3

/* Struct field encoding specification for nanopb */
extern const pb_field_t Speak_fields[2];
extern const pb_field_t Time_fields[4];
extern const pb_field_t Alarm_fields[5];
extern const pb_field_t Lighting_fields[3];

/* Maximum encoded size of messages (where known) */
#define Speak_size                               259
#define Time_size                                18
#define Alarm_size                               20
#define Lighting_size                            12

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define ALARM_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
