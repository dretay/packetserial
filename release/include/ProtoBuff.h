#pragma once

#include <pb.h>
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bithelper.h"
#include "hid.pb.h"
#include "log.h"

typedef bool (*PROTOBUFF_UNMARSHAL_CALLBACK_FNP)(pb_istream_t*);

typedef struct {
    const pb_field_t* type;
    void (*callback)(pb_istream_t* decode_stream, const pb_field_t* type);
} Handler;

#define PROTOBUFF_MAX_HANDLERS 16

struct protobuff {
    size_t (*marshal)(void* src, const pb_field_t fields[], pb_byte_t* buf, size_t bufsize, bool delimited);
    bool (*unmarshal)(pb_byte_t* buf, size_t bufsize, bool delimited);
    bool (*explicit_unmarshal)(pb_byte_t* buf, size_t bufsize, bool delimited, PROTOBUFF_UNMARSHAL_CALLBACK_FNP callback);
    bool (*add_handler)(const pb_field_t* type, void* callback);
    bool (*decode_union)(pb_istream_t* stream, const pb_field_t fields[], void* dest_struct);
    bool (*decode)(pb_istream_t* stream, const pb_field_t fields[], void* dest_struct);
    void (*clear_handlers)(void);
};

extern const struct protobuff ProtoBuff;