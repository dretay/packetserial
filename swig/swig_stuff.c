#include "swig_stuff.h"

char mystring[10] = "hello";
char* get_a_string(void)
{
    return mystring;
}
bool tx_handler(u8* buffer, size_t size)
{
    printf("%s", buffer);
    return true;
}
void test_receive_report(void)
{
    UnionMessage message = UnionMessage_init_zero;
    message.has_hid_report = true;
    strncpy(message.hid_report.data, "test1test2test3test4test5test6test7testtest1test2test3test4test5test6test7test", strlen("test1test2test3test4test5test6test7testtest1test2test3test4test5test6test7test") + 1);
    uint8_t my_buffer[300] = { 0 };
    int size = ProtoBuff.marshal(&message, UnionMessage_fields, my_buffer, 300, true);

    PacketSerial.register_tx_handler(&tx_handler);
    bool success = PacketSerial.send((char*)my_buffer, size);
}

void hello()
{
    printf("Hello!\n");
}