#include "PacketDAO.h"

char test_array[] = {0x00,0x11,0x12,0x13,0x14};

void char_test(char *packet){
    for(int i=0;i<5;i++){
        packet[i] = test_array[i];
    }
}
