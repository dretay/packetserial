## PacketSerial: send Protocol Buffer-encoded packets of data over UART ##

### Overview ###

Project encapsulates logic needed to break down a "large" message into smaller packets that can be transmitted between uControllers.

### Compiling ###
(WIN)
You need USB support to be added to WSL: https://learn.microsoft.com/en-us/windows/wsl/connect-usb#attach-a-usb-device.
(LINUX)
apt install ruby valgrind cppcheck python3-pip protobuf-compiler
pip3 install pyinstaller
cd lib/nanopb-0.3.9.3/tools
DEST=/home/parallels/src/packetserial/lib/nanopb-0.3.9.3 ./make_linux_package.sh

(OSX)
brew install ruby 
### Main Features ###
- Calculates a CRC8 for each packet
- Thoroughly tested through Unity-based unit test suite
- Integration with Jupyter notebooks through SWIG to support integration testing
- Supports arbitrarily-sized payloads thanks to COBS-encoded messages and built-in support for Protocol Buffers through nanopb.


### Summary ###
To use the library you simply need to register the appropriate handlers. See tests for more examples but the basic idea is as follows:
```c
void report_parser(pb_istream_t* stream, const pb_field_t* type)
{
    HIDReport report;
    if (ProtoBuff.decode_union(stream, type, &report)) {
        //do stuff with decoded message
    } else {
        TEST_FAIL_MESSAGE("Failed to parse report");
    }
}
bool tx_handler(u8* buffer, size_t size)
{
    bool retval = true;
    for (int i = 0; i < size; i++) {
        //the idea is whatever native UART handler is on your uController you would call
        //process() inside the handler to pass control back to the library
        retval = retval && PacketSerial.process((const char*)&buffer[i], 1);
    }
    return retval;
}
void test_byte_at_a_time_receive_report(void)
{
    //for each "type" of message you need to define a handler to parse the stream into an object
    //and handle the resulting object that has been transmitted
    ProtoBuff.add_handler(HIDReport_fields, &report_parser);

    //we also need to register a tx handler to deal with the underlying UART transmission
    PacketSerial.register_tx_handler(&tx_handler);

    //in this case we're defining a dummy message.
    UnionMessage message = UnionMessage_init_zero;
    message.has_hid_report = true;
    strncpy(message.hid_report.data, "test1test2test3test4test5test6test7testtest1test2test3test4test5test6test7test", strlen("test1test2test3test4test5test6test7testtest1test2test3test4test5test6test7test") + 1);

    //we encode the resulting PB object into a byte array
    uint8_t my_buffer[300] = { 0 };
    int size = ProtoBuff.marshal(&message, UnionMessage_fields, my_buffer, 300, true);

    //finally we send the data off on its way. In this example the data will be broken into several packets,
    //sent to a receiver, then reconstructed and parsed on the receiving end.
    PacketSerial.send((char*)my_buffer, size);
}
```
