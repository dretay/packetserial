#include "PacketDAO.h"

char packet_buffer[10][32];
int packet_cnt = 0;

// char* get_packet_buffer(int index){
//     return packet_buffer[index];
// }
bool tx_handler(u8* buffer, size_t size){
    memcpy(packet_buffer[packet_cnt++], buffer, size);
    return true;
}
int generate_speak(char* speech, int speech_size){
    packet_cnt=0;
    for(int i=0;i<10;i++){
        memset(packet_buffer[i],0,32);
    }

    UnionMessage message = UnionMessage_init_zero;
    message.has_speak = true;
    strncpy(message.speak.data, speech, speech_size);
    // uint8_t my_buffer[300] = { 0 };
    // int size = marshal(&message, UnionMessage_fields, my_buffer, 300, true);
    // protobuff_test();

    // PacketSerial.register_tx_handler(&tx_handler);
    // bool success = PacketSerial.send((char*)my_buffer, size);
    // if(success){
    //     return packet_cnt;
    // }
    return -1;
}