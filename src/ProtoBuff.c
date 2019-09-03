#include "ProtoBuff.h"

static volatile Handler handlers[PROTOBUFF_MAX_HANDLERS];
static uint16_t handlers_usage_mask = 0;

static const pb_field_t* decode_unionmessage_type(pb_istream_t* stream)
{
    pb_wire_type_t wire_type;
    uint32_t tag;
    bool eof;
    while (pb_decode_tag(stream, &wire_type, &tag, &eof)) {
        if (wire_type == PB_WT_STRING) {
            const pb_field_t* field;
            for (field = UnionMessage_fields; field->tag != 0; field++) {
                if (field->tag == tag && (field->type & PB_LTYPE_SUBMESSAGE)) {
                    /* Found our field. */
                    return field->ptr;
                }
            }
        }

        /* Wasn't our field.. */
        pb_skip_field(stream, wire_type);
    }

    return NULL;
}

static bool decode_union(pb_istream_t* stream, const pb_field_t fields[], void* dest_struct)
{
    pb_istream_t substream;
    bool status;
    if (!pb_make_string_substream(stream, &substream)) {
        return false;
    }

    status = pb_decode(&substream, fields, dest_struct);
    pb_close_string_substream(stream, &substream);
    return status;
}

static size_t marshal(void* src, const pb_field_t fields[], pb_byte_t* buf, size_t bufsize, bool delimited)
{
    /* Create a stream that will write to our buffer. */
    pb_ostream_t stream = pb_ostream_from_buffer(buf, bufsize);

    /* Now we are ready to encode the message! */
    if (delimited) {
        log_trace("attempting to encode a delimited message");
        if (!pb_encode_delimited(&stream, fields, src)) {
            return -1;
        }
    } else {
        log_trace("attempting to encode a non-delimited message");
        if (!pb_encode(&stream, fields, src)) {
            return -1;
        }
    }
    return stream.bytes_written;
}
static bool idx_valid(int8_t idx)
{
    return idx >= 0 && idx < PROTOBUFF_MAX_HANDLERS;
}
static int8_t find_handler(const pb_field_t* type)
{
    uint8_t i = 0;
    for (i = 0; i < PROTOBUFF_MAX_HANDLERS; i++) {
        if (bitRead(handlers_usage_mask, i) == 1) {
            if (handlers[i].type == type) {
                return i;
            }
        }
    }
    return -1;
}
static bool _unmarshal(pb_byte_t* buf, size_t bufsize, bool delimited, PROTOBUFF_UNMARSHAL_CALLBACK_FNP callback)
{
    pb_istream_t parent_stream = pb_istream_from_buffer(buf, bufsize);
    pb_istream_t* decode_stream = &parent_stream;
    pb_istream_t sub_stream;
    bool retval = false;
    if (delimited) {
        log_trace("attempting to decode a delimited message");
        if (!pb_make_string_substream(&parent_stream, &sub_stream)) {
            log_error("unable to make substream");
            return false;
        }
        decode_stream = &sub_stream;
    } else {
        log_debug("attempting to decode a non-delimited message");
    }

    retval = callback(decode_stream);

    if (delimited) {
        if (!pb_close_string_substream(&parent_stream, decode_stream)) {
            log_error("unable to close substream");
            return false;
        }
    }
    return retval;
}
static bool explicit_unmarshal(pb_byte_t* buf, size_t bufsize, bool delimited, PROTOBUFF_UNMARSHAL_CALLBACK_FNP callback)
{
    return _unmarshal(buf, bufsize, delimited, callback);
}
static bool _unmarshal_lookup_callback(pb_istream_t* decode_stream)
{
    const pb_field_t* type = decode_unionmessage_type(decode_stream);
    int idx = find_handler(type);
    if (idx_valid(idx)) {
        handlers[idx].callback(decode_stream, type);
        return true;
    } else {
        log_error("unknown type");
        return false;
    }
}
static bool unmarshal(pb_byte_t* buf, size_t bufsize, bool delimited)
{
    return _unmarshal(buf, bufsize, delimited, _unmarshal_lookup_callback);
}

static int8_t get_unused_idx(void)
{
    uint8_t i = 0;
    for (i = 0; i < PROTOBUFF_MAX_HANDLERS; i++) {
        if (bitRead(handlers_usage_mask, i) == 0) {
            bitSet(handlers_usage_mask, i);
            return i;
        }
    }
    return -1;
}
static bool add_handler(const pb_field_t* type, void* callback)
{
    int idx = find_handler(type);
    if (idx_valid(idx)) {
        log_error("type entry already exists");
        return false;
    }
    idx = get_unused_idx();
    if (idx_valid(idx)) {
        handlers[idx].callback = callback;
        handlers[idx].type = type;
        return true;
    } else {
        log_error("handler list is full");
    }
    return false;
}
static void clear_handlers(void)
{
    memset((void*)handlers, 0, sizeof(handlers));
    handlers_usage_mask = 0;
}

const struct protobuff ProtoBuff = {
    .marshal = marshal,
    .unmarshal = unmarshal,
    .add_handler = add_handler,
    .decode_union = decode_union,
    .clear_handlers = clear_handlers,
    .explicit_unmarshal = explicit_unmarshal,
    .decode = pb_decode,
};