#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "hid.pb.h"
#include <Crc8.h>
#include <pb.h>
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "ProtoBuff.h"
#include "bithelper.h"
#include "log.h"
#include "type_shortcuts.h"

typedef bool (*PACKETSERIAL_HANDLER_FNP)(u8*, size_t);

#define member_size(type, member) sizeof(((type*)0)->member)

struct packetserial {
    bool (*send)(const char* buffer, size_t size);
    void (*update)(void);
    void (*register_rx_handler)(void* handler);
    PACKETSERIAL_HANDLER_FNP(*get_rx_handler)
    (void);
    PACKETSERIAL_HANDLER_FNP(*get_tx_handler)
    (void);
    void (*register_tx_handler)(void* handler);
    void (*clear_handlers)(void);
    u8 (*calculate_crc)(const char* buffer, size_t size);
    void (*build_packet)(Packet* packet, const char* buffer, size_t size, u8 sequence_number, Packet_Flag flag);
};

extern const struct packetserial PacketSerial;