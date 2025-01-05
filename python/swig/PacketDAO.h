#pragma once
#include "PacketSerial.h"
#include "ProtoBuff.h"
#include <stdio.h>

int generate_speak(char* speech, int speech_size);
void get_packet_buffer(int index, char* dest);
