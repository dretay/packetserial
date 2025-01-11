#pragma once
#include "PacketSerial.h"
#include "ProtoBuff.h"
#include <stdio.h>

int generate_speak(char* speech, int speech_size);
u8* get_packet_buffer(int index);
int generate_neopixel_pulse();