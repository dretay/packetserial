#include "PacketSerial.h"

static char packet_buffer[PACKET_BUFFER_SIZE];

static char rx_buffer[Packet_size + 1];
static char* p_rx_buffer = rx_buffer;
static int rx_buffer_remaining = Packet_size + 1;

static char* packet_buffer_offset = packet_buffer;

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
static void build_packet(Packet* packet, const uint8_t* buffer, size_t size, u8 sequence_number, Packet_Flag flag)
{
    memcpy(packet->data.bytes, buffer, size);
    packet->data.size = size;
    packet->crc = calculate_crc(packet->data.bytes, size);
    packet->sequence_number = sequence_number;
    packet->flag = flag;
}

static bool send(const char* buffer, size_t size)
{
    unsigned int data_chunk_size = member_size(Packet, data.bytes) - 1;

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

        u8 unencoded_buffer[Packet_size] = { 0 };
        u8 encoded_buffer[Packet_size + 1] = { 0 };

        size_t pb_msg_size = ProtoBuff.marshal(&packet, Packet_fields, unencoded_buffer, Packet_size, true);
        cobs_encode_result result = cobs_encode(encoded_buffer, Packet_size + 1, unencoded_buffer, pb_msg_size);
        size_t encoded_size = result.out_len + 1;
        encoded_buffer[encoded_size - 1] = 0;
        log_trace("%d size pb encoded messaged translated to %d size cobs-encoded message", pb_msg_size, encoded_size);

        if (tx_handler != NULL) {
            log_debug("Transmitted %ld bytes of %d buffer for packet %d of %d", encoded_size, Packet_size, i, total_chunks - 1);

            char hex_str[(encoded_size * 2) + 1];
            string2hexstring((char*)encoded_buffer, hex_str);
            log_trace("\t%s", hex_str);

            bool tx_result = tx_handler(encoded_buffer, encoded_size);
            return_status = return_status && tx_result;
        } else {
            log_error("tx handler undefined!");
            return false;
        }
    }
    return return_status;
}

static void register_tx_handler(void* handler)
{
    tx_handler = handler;
}
static void register_rx_handler(void* handler)
{
    rx_handler = handler;
}
static PACKETSERIAL_HANDLER_FNP get_tx_handler(void)
{
    return tx_handler;
}

static void clear_handlers(void)
{
    tx_handler = NULL;
}

static void reset_packet_buffer(void)
{
    packet_buffer_offset = packet_buffer;
    memset(packet_buffer, 0, sizeof(packet_buffer));
}
static void reset_rx_buffer(void)
{
    rx_buffer_remaining = Packet_size + 1;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}
static bool packet_parser(pb_istream_t* stream)
{
    Packet packet;
    if (ProtoBuff.decode(stream, Packet_fields, &packet)) {
        memcpy(packet_buffer_offset, packet.data.bytes, packet.data.size);
        packet_buffer_offset += packet.data.size;
        log_trace("Packet buffer now at %d/%d (%d rx'd)", (packet_buffer_offset - packet_buffer), PACKET_BUFFER_SIZE, packet.data.size);

        if (packet.flag == Packet_Flag_LAST || packet.flag == Packet_Flag_FIRSTLAST) {
            log_trace("Last packet received, attempting to parse");
            if(rx_handler != NULL){
                rx_handler((u8*)packet_buffer, PACKET_BUFFER_SIZE);
                reset_packet_buffer();
            }
            else if (ProtoBuff.unmarshal((u8*)packet_buffer, PACKET_BUFFER_SIZE, true)) {
                log_trace("Parsing successful, resetting buffer");
                reset_packet_buffer();
            } else {
                log_trace("Unable to parse data, resetting buffer");
                reset_packet_buffer();
                return false;
            }
        }
    } else {
        log_trace("Unable to decode stream into pb model");
        return false;
    }
    return true;
}
static bool process(const char* buffer, size_t size)
{
    char hex_str[(size * 2) + 1];
    stringn2hexstring((char*)buffer, hex_str, size);

    log_debug("processing %d byte(s) incoming data: %s", size, hex_str);
    if (size > rx_buffer_remaining) {
        log_error("rx buffer overflow, unable to process packet: needed %d but only %d remaining", size, rx_buffer_remaining);
        return false;
    }
    strncpy(p_rx_buffer, buffer, size);
    p_rx_buffer += size;
    rx_buffer_remaining -= size;
    if (buffer[size - 1] == '\0') {
        u32 msg_size = strlen(rx_buffer);
        log_debug("delimited packet received, attempting to process process %d bytes:", msg_size);

        char full_buffer_hex_str[(msg_size * 2) + 1];
        string2hexstring((char*)rx_buffer, full_buffer_hex_str);
        log_trace("\t%s", full_buffer_hex_str);

        uint8_t decoded_data_array[msg_size];
        cobs_decode(decoded_data_array, msg_size, rx_buffer, msg_size);
        bool return_val = ProtoBuff.explicit_unmarshal((u8*)decoded_data_array, msg_size, true, packet_parser);
        if (return_val) {
            log_debug("packet successfully parsed");
            reset_rx_buffer();
        } else {
            log_error("unable to parse packet!");
        }
        p_rx_buffer = rx_buffer;
        return return_val;
    }
    return true;
}
static char* get_packet_buffer_offset(void)
{
    return packet_buffer_offset;
}
static char* get_packet_buffer(void)
{
    return packet_buffer;
}
const struct packetserial PacketSerial = {
    .send = send,
    .register_tx_handler = register_tx_handler,
	.register_rx_handler = register_rx_handler,
    .get_tx_handler = get_tx_handler,
    .process = process,
    .clear_handlers = clear_handlers,
    .calculate_crc = calculate_crc,
    .build_packet = build_packet,
    .reset_packet_buffer = reset_packet_buffer,
    .get_packet_buffer_offset = get_packet_buffer_offset,
    .get_packet_buffer = get_packet_buffer,
    .reset_rx_buffer = reset_rx_buffer,
};

