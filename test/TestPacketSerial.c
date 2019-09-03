#include "../src/PacketSerial.h"
#include "unity.h"
#include <string.h>

#define BUFFER_SIZE 101

void setUp(void)
{
}

void tearDown(void)
{
    PacketSerial.clear_handlers();
    ProtoBuff.clear_handlers();
}

//general setup / teardown tests
bool dummy_tx_handler(u8* buffer, size_t size)
{
    return true;
}
bool dummy_rx_handler(u8* buffer, size_t size)
{
    return true;
}

void test_adding_tx_handler(void)
{
    PacketSerial.register_tx_handler(&dummy_tx_handler);
    TEST_ASSERT_EQUAL(dummy_tx_handler, PacketSerial.get_tx_handler());
}
void test_adding_rx_handler(void)
{
    PacketSerial.register_rx_handler(&dummy_rx_handler);
    TEST_ASSERT_EQUAL(dummy_rx_handler, PacketSerial.get_rx_handler());
}
void test_clearing_handlers(void)
{
    PacketSerial.register_rx_handler(&dummy_rx_handler);
    PacketSerial.register_tx_handler(&dummy_tx_handler);
    PacketSerial.clear_handlers();
    TEST_ASSERT_EQUAL(NULL, PacketSerial.get_tx_handler());
    TEST_ASSERT_EQUAL(NULL, PacketSerial.get_rx_handler());
}

void test_crc8(void)
{
    char test1[] = { 't', 'e', 's', 't' };
    char test2[] = { 't', 'e', 's', 's' };
    u8 crc1 = PacketSerial.calculate_crc(test1, sizeof(test1));
    u8 crc11 = PacketSerial.calculate_crc(test1, sizeof(test1));
    u8 crc2 = PacketSerial.calculate_crc(test2, sizeof(test2));

    TEST_ASSERT_EQUAL(crc1, crc11);
    TEST_ASSERT_NOT_EQUAL(crc1, crc2);
}
void test_build_packet(void)
{
    char data[] = { 't', 'e', 's', 't' };
    u8 sequence_number = 1;
    u8 crc = PacketSerial.calculate_crc(data, sizeof(data));
    Packet packet = Packet_init_zero;
    Packet_Flag flag = Packet_Flag_FIRST;
    PacketSerial.build_packet(&packet, data, sizeof(data), sequence_number, flag);

    TEST_ASSERT_EQUAL_STRING(packet.data, data);
    TEST_ASSERT_EQUAL(packet.sequence_number, sequence_number);
    TEST_ASSERT_EQUAL(packet.crc, crc);
    TEST_ASSERT_EQUAL(packet.flag, flag);
}
bool dummy_test_marshal_unmarshal_packet_parser(pb_istream_t* stream)
{
    Packet packet;
    if (ProtoBuff.decode(stream, Packet_fields, &packet)) {
        TEST_ASSERT_EQUAL_STRING("test", packet.data);
    } else {
        TEST_FAIL_MESSAGE("Failed to parse packet");
        return false;
    }
    return true;
}
void test_marshal_unmarshal_packet(void)
{
    u8 buffer[Packet_size] = { 0 };
    char data[] = { 't', 'e', 's', 't' };
    u8 sequence_number = 1;
    u8 crc = PacketSerial.calculate_crc(data, sizeof(data));
    Packet_Flag flag = Packet_Flag_FIRST;
    Packet packet = Packet_init_zero;

    PacketSerial.build_packet(&packet, data, sizeof(data), sequence_number, flag);

    TEST_ASSERT_EQUAL_STRING(packet.data, data);
    TEST_ASSERT_EQUAL(packet.sequence_number, sequence_number);
    TEST_ASSERT_EQUAL(packet.crc, crc);
    TEST_ASSERT_EQUAL(packet.flag, flag);

    size_t size = ProtoBuff.marshal(&packet, Packet_fields, buffer, Packet_size, true);
    printf("\nEncoded message into %ld bytes of %d total\n\n", size, Packet_size);
    TEST_ASSERT_EQUAL(size, 14);
    ProtoBuff.explicit_unmarshal(buffer, Packet_size, true, dummy_test_marshal_unmarshal_packet_parser);
}

bool dummy_test_send_small_packet_packet_parser(pb_istream_t* stream)
{
    Packet packet;
    if (ProtoBuff.decode(stream, Packet_fields, &packet)) {
        TEST_ASSERT_EQUAL_STRING("test", packet.data);
        TEST_ASSERT_EQUAL(packet.flag, Packet_Flag_FIRSTLAST);
    } else {
        TEST_FAIL_MESSAGE("Failed to parse packet");
        return false;
    }
    return true;
}
bool dummy_test_send_small_packet_tx_handler(u8* buffer, size_t size)
{
    return ProtoBuff.explicit_unmarshal(buffer, size, true, dummy_test_send_small_packet_packet_parser);
}

void test_send_small_packet(void)
{

    PacketSerial.register_tx_handler(&dummy_test_send_small_packet_tx_handler);
    char data[] = { 't', 'e', 's', 't' };
    bool success = PacketSerial.send(data, sizeof(data));

    TEST_ASSERT_TRUE(success);
}

char dummy_test_send_big_packet_messages[3][100];
int dummy_test_send_big_packet_counter = 0;
bool dummy_test_send_big_packet_packet_parser(pb_istream_t* stream)
{
    Packet packet;
    if (ProtoBuff.decode(stream, Packet_fields, &packet)) {
        int packet_data_size = member_size(Packet, data);

        strncpy(dummy_test_send_big_packet_messages[dummy_test_send_big_packet_counter], packet.data, packet_data_size);

        dummy_test_send_big_packet_counter++;
        if (packet.flag == Packet_Flag_LAST) {
            TEST_ASSERT_EQUAL_STRING("test1test2test3", dummy_test_send_big_packet_messages[0]);
            TEST_ASSERT_EQUAL_STRING("test4test5test6", dummy_test_send_big_packet_messages[1]);
            TEST_ASSERT_EQUAL_STRING("test7test", dummy_test_send_big_packet_messages[2]);
        }
    } else {
        TEST_FAIL();
        return false;
    }
    return true;
}
bool dummy_test_send_big_packet_tx_handler(u8* buffer, size_t size)
{
    return ProtoBuff.explicit_unmarshal(buffer, size, true, dummy_test_send_big_packet_packet_parser);
}

void test_send_big_packet(void)
{
    PacketSerial.register_tx_handler(&dummy_test_send_big_packet_tx_handler);
    char data[] = "test1test2test3test4test5test6test7test";
    bool success = PacketSerial.send(data, sizeof(data));
    TEST_ASSERT_TRUE(success);
}

void dummy_test_send_little_hid_report_parse_hidreport(pb_istream_t* stream, const pb_field_t* type)
{
    HIDReport report;
    if (ProtoBuff.decode_union(stream, type, &report)) {
        TEST_ASSERT_EQUAL_STRING("hello", report.data);
    } else {
        TEST_FAIL_MESSAGE("Failed to parse report");
    }
}
bool dummy_test_send_little_hid_report_tx_handler(u8* buffer, size_t size)
{
    return ProtoBuff.explicit_unmarshal(buffer, size, true, dummy_test_send_big_packet_packet_parser);
}

void test_send_little_hid_report(void)
{
    bool result = ProtoBuff.add_handler(HIDReport_fields, &dummy_test_send_little_hid_report_parse_hidreport);
    UnionMessage message = UnionMessage_init_zero;
    message.has_hid_report = true;
    strncpy(message.hid_report.data, "hello", strlen("hello") + 1);
    uint8_t my_buffer[100] = { 0 };
    int size = ProtoBuff.marshal(&message, UnionMessage_fields, my_buffer, 100, true);

    PacketSerial.register_tx_handler(&dummy_test_send_little_hid_report_tx_handler);
    bool success = PacketSerial.send((char*)my_buffer, size);
    TEST_ASSERT_TRUE(success);
}

char dummy_test_send_big_hid_report_messages[300];
char* dummy_test_send_big_hid_report_messages_offset = dummy_test_send_big_hid_report_messages;
void dummy_test_send_big_hid_report_parse_hidreport(pb_istream_t* stream, const pb_field_t* type)
{
    HIDReport report;
    if (ProtoBuff.decode_union(stream, type, &report)) {
        TEST_ASSERT_EQUAL_STRING("test1test2test3test4test5test6test7test", report.data);
    } else {
        TEST_FAIL_MESSAGE("Failed to parse report");
    }
}
bool dummy_test_send_big_hid_report_packet_parser(pb_istream_t* stream)
{
    Packet packet;
    if (ProtoBuff.decode(stream, Packet_fields, &packet)) {
        int packet_data_size = strlen(packet.data);
        strncpy(dummy_test_send_big_hid_report_messages_offset, packet.data, packet_data_size);
        dummy_test_send_big_hid_report_messages_offset += packet_data_size;

        if (packet.flag == Packet_Flag_LAST) {
            bool result = ProtoBuff.unmarshal(dummy_test_send_big_hid_report_messages, 300, false);
            TEST_ASSERT_TRUE(result);
        }
    } else {
        TEST_FAIL();
        return false;
    }
    return true;
}
bool dummy_test_send_big_hid_report_tx_handler(u8* buffer, size_t size)
{
    return ProtoBuff.explicit_unmarshal(buffer, size, true, dummy_test_send_big_hid_report_packet_parser);
}

void test_send_big_hid_report(void)
{
    bool result = ProtoBuff.add_handler(HIDReport_fields, &dummy_test_send_big_hid_report_parse_hidreport);
    UnionMessage message = UnionMessage_init_zero;
    message.has_hid_report = true;
    strncpy(message.hid_report.data, "test1test2test3test4test5test6test7test", strlen("test1test2test3test4test5test6test7test") + 1);
    uint8_t my_buffer[100] = { 0 };
    int size = ProtoBuff.marshal(&message, UnionMessage_fields, my_buffer, 100, true);

    PacketSerial.register_tx_handler(&dummy_test_send_big_hid_report_tx_handler);
    bool success = PacketSerial.send((char*)my_buffer, size);
    TEST_ASSERT_TRUE(success);
}