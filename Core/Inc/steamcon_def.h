#ifndef __steamcon_def_H
#define __steamcon_def_H
#include <stdint.h>

typedef union {
	struct {
		uint8_t x01;
		uint8_t x00;
		uint8_t type;
		uint8_t size;
		uint32_t seq;
		struct __attribute__((packed)) {
			//array[8]
			uint8_t R2		:1;
			uint8_t L2		:1;
			uint8_t R1		:1;
			uint8_t L1		:1;
			uint8_t Y		:1;
			uint8_t B		:1;
			uint8_t X		:1;
			uint8_t A		:1;
			//array[9]
			uint8_t U		:1;
			uint8_t R		:1;
			uint8_t L		:1;
			uint8_t D		:1;
			uint8_t ML		:1;
			uint8_t MM		:1;
			uint8_t MR		:1;
			uint8_t L3		:1;
			//array[10]
			union {
				struct __attribute__((packed)) {
					uint8_t R3		:1;
					uint8_t LP1to2	:1;
					uint8_t RP2		:1;
					uint8_t LPorLS	:1;
					uint8_t RP1		:1;
					uint8_t unused1	:1;
					uint8_t LS1		:1;
					uint8_t LPandLS	:1;
				};
				struct __attribute__((packed)) {
					uint8_t unsued2	:1;
					uint8_t LP1		:1;	//second,=(LPandLS||LPorLS)&&!LP1to2
					uint8_t unsued3	:5;
					uint8_t LP2		:1;	//first, =(LPandLS||LPorLS)&& LP1to2
				};
			};
		};
		uint8_t LT_8;
		uint8_t RT_8;
		uint8_t unused4[3];
		int16_t LX;
		int16_t LY;	//LPorLS ? Pad : Stick
		int16_t RX;
		int16_t RY;
		uint16_t LT_16;
		uint16_t RT_16;
		int16_t acc_x;     //left -> postive
		int16_t acc_y;     //tail -> postive
		int16_t acc_z;     //down -> postive
		int16_t ang_pitch; //up   -> postive
		int16_t ang_roll;  //left -> postive
		int16_t ang_yaw;   //ccw  -> postive
		int16_t Qr;
		int16_t Qi;
		int16_t Qj;
		int16_t Qk;
		uint8_t padding[16];
	};
	uint8_t array[64];
} STEAM_Report_TypeDef;

#endif /* __steamcon_def_H */
