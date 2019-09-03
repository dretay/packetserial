#include "PacketSerial.h"

static bool (*tx_handler)(u8* buffer, size_t size);
static bool (*rx_handler)(u8* buffer, size_t size);

static u8 calculate_crc(const char* buffer, size_t size)
{
    u8 crc = 0;
    for (int i = 0; i < size; i++) {
        crc = Crc8(crc, buffer[i]);
    }
    return crc;
}
static void build_packet(Packet* packet, const char* buffer, size_t size, u8 sequence_number, Packet_Flag flag)
{
    strncpy(packet->data, buffer, size);
    packet->crc = calculate_crc(packet->data, size);
    packet->sequence_number = sequence_number;
    packet->flag = flag;
}
static bool send(const char* buffer, size_t size)
{
    unsigned int data_chunk_size = member_size(Packet, data) - 1;

    //take the ceiling
    unsigned int total_chunks = (size + data_chunk_size - 1) / data_chunk_size;
    bool return_status = true;
    const char* buffer_offset = buffer;
    for (int i = 0; i < total_chunks; i++) {
        Packet packet = Packet_init_zero;

        Packet_Flag flag = Packet_Flag_FIRST;
        int packet_data_chunk_size = 0;

        if (i == 0 && total_chunks == 1) {
            log_trace("Constructing firstlast packet in sequence");
            flag = Packet_Flag_FIRSTLAST;
            packet_data_chunk_size = size;
        } else if (i == 0 && total_chunks > 1) {
            log_trace("Constructing first packet in sequence");
            flag = Packet_Flag_FIRST;
            packet_data_chunk_size = data_chunk_size;
        } else if (i > 0 && i < total_chunks - 1) {
            log_trace("Constructing next packet in sequence");
            flag = Packet_Flag_CONTINUE;
            packet_data_chunk_size = data_chunk_size;
        } else {
            log_trace("Constructing last packet in sequence");
            flag = Packet_Flag_LAST;
            packet_data_chunk_size = size % data_chunk_size;
        }
        log_trace("packet data chunk size set to %d", packet_data_chunk_size);

        build_packet(&packet, buffer_offset, packet_data_chunk_size, i, flag);
        buffer_offset += packet_data_chunk_size;
        u8 buffer[Packet_size] = { 0 };

        size_t size = ProtoBuff.marshal(&packet, Packet_fields, buffer, Packet_size, true);

        if (tx_handler != NULL) {
            log_trace("Transmitted %ld bytes of %d buffer for packet %d of %d", size, Packet_size, i, total_chunks - 1);
            bool tx_result = tx_handler(buffer, size);
            return_status = return_status && tx_result;
        } else {
            log_error("tx handler undefined!");
            return false;
        }
    }
    return return_status;
}
static void update(void)
{
}
static void register_rx_handler(void* handler)
{
    rx_handler = handler;
}
static PACKETSERIAL_HANDLER_FNP get_rx_handler(void)
{
    return rx_handler;
}
static void register_tx_handler(void* handler)
{
    tx_handler = handler;
}
static PACKETSERIAL_HANDLER_FNP get_tx_handler(void)
{
    return tx_handler;
}

static void clear_handlers(void)
{
    tx_handler = NULL;
    rx_handler = NULL;
}

const struct packetserial PacketSerial = {
    .send = send,
    .update = update,
    .register_tx_handler = register_tx_handler,
    .register_rx_handler = register_rx_handler,
    .get_rx_handler = get_rx_handler,
    .get_tx_handler = get_tx_handler,
    .clear_handlers = clear_handlers,
    .calculate_crc = calculate_crc,
    .build_packet = build_packet,
};