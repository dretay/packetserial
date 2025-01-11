#include "PacketDAO.h"

u8 packet_buffer[10][32];
int packet_cnt = 0;

u8* get_packet_buffer(int index){    
    int size=32;
    char hex_str[(size * 2) + 1];
    string2hexstring((char*)packet_buffer[index], hex_str);
    printf("(get_packet_buffer) %s", hex_str);
    return packet_buffer[index];    
}
bool tx_handler(u8* buffer, size_t size){
  
    for(int i=0;i<size;i++){
        packet_buffer[0][i] = buffer[i];
    }    
    return true;
}
void clean_buffer(){
    packet_cnt=0;
    for(int i=0;i<10;i++){
        memset(packet_buffer[i],0,32);
    }
}
void init(){
    PacketSerial.register_tx_handler(&tx_handler);
}
int send_msg(UnionMessage *message){
    uint8_t my_buffer[300] = { 0 };
    int size = ProtoBuff.marshal(message, UnionMessage_fields, my_buffer, 300, true);
    bool success = PacketSerial.send((char*)my_buffer, size);
    if(success){
        return packet_cnt;
    }
    return -1;
}
int generate_neopixel_pulse(){
    clean_buffer();    
    
    UnionMessage message = UnionMessage_init_zero;
    message.has_neopixel = true;
    message.neopixel.pattern = NeoPixelMsg_Pattern_PULSE;
    message.neopixel.start_color.green = 229;
    message.neopixel.start_color.red = 42;
    message.neopixel.start_color.blue = 62;

    return send_msg(&message);

}
// int generate_speak(char* speech, int speech_size){   
// clean_buffer();
//     UnionMessage message = UnionMessage_init_zero;
//     message.has_speak = true;
//     strncpy(message.speak.data, speech, speech_size);
//     uint8_t my_buffer[300] = { 0 };
//     int size = ProtoBuff.marshal(&message, UnionMessage_fields, my_buffer, 300, true);

//     PacketSerial.register_tx_handler(&tx_handler);
//     bool success = PacketSerial.send((char*)my_buffer, size);
//     if(success){
//         return packet_cnt;
//     }
//     return -1;
// }