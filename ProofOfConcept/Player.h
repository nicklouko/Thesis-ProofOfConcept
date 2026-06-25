#pragma once

class Player
{
public:
    float xpos; // 0x0000
    float ypos; // 0x0004
    float zpos; // 0x0008
    char pad_000C[36]; // 0x000C
    float N000001F0; // 0x0030
    float N00000271; // 0x0034
    float N000001F1; // 0x0038
    float Yaw; // 0x003C
    float Pitch; // 0x0040
    char pad_0044[51]; // 0x0044 - padding to 0x0077
    bool isDead; // 0x0077
    char pad_0078[256]; // 0x0078 - continuing padding to 0x0178
    int32_t Health; // 0x0178
    char pad_017C[508]; // 0x017C — from 0x017C to 0x037C
    char team[4]; // 0x0378 — fixed-size ASCII string
}; // Size: 0x0408