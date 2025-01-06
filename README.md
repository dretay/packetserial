- [Overview](#overview)
- [Compiling and Testing](#compiling-and-testing)
  - [Compiling](#compiling)
  - [Testing](#testing)
- [Main Features](#main-features)
- [Usage Example](#usage-example)



# Overview
Sometimes you have a project where you want to send data back and forth between microcontrollers (such as an ESP32 and a STM32). You can run into several challenges such as maintaining consistent models between the devices, encoding / endian-ness issues, and framing (espically if you're using DMA). 

# Compiling and Testing
## Compiling
Since the ProtoBuf requires that the models be compiled, you can't just import the project, but rather must generate the models and then include them. Rather than requiring you to install a bunch of dependencies (and deal with really annoying issues like the fact that OSX doesn't support Valgrind), I've included a container to do the compilation. Running `make dockerbuild` will build the container, install the dependencies, run tests, and generate a `release` folder with all the relevant source code.

## Testing
Once you've developed your models and built a client, you may want to test the project from your PC rather than on a second microcontroller. To help with that I've included a Jupyter notebook that you can use to send messages to the microcontroller. When you invoke `make jupyter` the build target will dynamically build a SWIG interface to the C code and expose it to Python. You can then invoke the C code directly through Python to send serial messages over something like a GreatFET. 
 - **NOTE:** for Jupyter to work correctly you need to have a properly setup CONDA environment. Something like the following should work:
 ``` sh
$ conda create -n packetserial python=3.11
$ conda activate packetserial
$ conda install pip jupyter conda-forge::swig
$ pip install greatfet
$ apt install build-essential
 ```  

# Main Features
- Calculates a CRC8 for each packet
- Thoroughly tested through Unity-based unit test suite
- Integration with Jupyter notebooks through SWIG to support integration testing
- Supports arbitrarily-sized payloads thanks to COBS-encoded messages and built-in support for Protocol Buffers through nanopb.


# Usage Example
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
