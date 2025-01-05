#include "PacketDAO.h"

char packet_buffer[10][32];
int packet_cnt = 0;

void get_packet_buffer(int index, char* dest){
    int size=32;
    char hex_str[(size * 2) + 1];
    string2hexstring((char*)packet_buffer[index], hex_str);
    printf("(get_packet_buffer) %s", hex_str);


    for(int i=0;i<32;i++){
        dest[i] = packet_buffer[index][i];
    }
    
}
bool tx_handler(u8* buffer, size_t size){
  
    for(int i=0;i<size;i++){
        packet_buffer[0][i] = buffer[i];
    }    
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
    uint8_t my_buffer[300] = { 0 };
    int size = ProtoBuff.marshal(&message, UnionMessage_fields, my_buffer, 300, true);

    PacketSerial.register_tx_handler(&tx_handler);
    bool success = PacketSerial.send((char*)my_buffer, size);
    if(success){
        return packet_cnt;
    }
    return -1;
}