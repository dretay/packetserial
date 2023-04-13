#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "messages.pb.h"
#include <Crc8.h>
#include <pb.h>
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "ProtoBuff.h"
#include "bithelper.h"
#include "cobs.h"
#include "hexhelper.h"
#include "log.h"
#include "type_shortcuts.h"


#define PACKET_BUFFER_SIZE 300
typedef bool (*PACKETSERIAL_HANDLER_FNP)(u8*, size_t);

#define member_size(type, member) sizeof(((type*)0)->member)

struct packetserial {
    bool (*send)(const char* buffer, size_t size);
    bool (*process)(const char* buffer, size_t size);
    PACKETSERIAL_HANDLER_FNP(*get_tx_handler)
    (void);
    void (*register_tx_handler)(void* handler);
    void (*register_rx_handler)(void* handler);
    void (*clear_handlers)(void);
    u8 (*calculate_crc)(const char* buffer, size_t size);
    void (*build_packet)(Packet* packet, const char* buffer, size_t size, u8 sequence_number, Packet_Flag flag);
    void (*reset_packet_buffer)(void);
    char* (*get_packet_buffer_offset)(void);
    char* (*get_packet_buffer)(void);
    void (*reset_rx_buffer)(void);
};

extern const struct packetserial PacketSerial;

