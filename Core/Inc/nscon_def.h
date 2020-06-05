#ifndef __nscon_def_H
#define __nscon_def_H
#include <stdint.h>

#define NSCON_REPORT_TYPE_REPORT 0x30
#define NSCON_REPORT_TYPE_HANDSHAKE 0x21

typedef union {
	struct __attribute__((packed)){
		uint8_t type;
		uint8_t seq;
		struct __attribute__((packed)) {
			//array[2]
			uint8_t myth0		:4; // 0x01
			uint8_t battery		:4; // 0~9
			//array[3]
			uint8_t Y			:1;
			uint8_t X			:1;
			uint8_t B			:1;
			uint8_t A			:1;
			uint8_t unused1		:2;
			uint8_t R1			:1;
			uint8_t R2			:1;
			//array[4]
			uint8_t Minus		:1;
			uint8_t Plus		:1;
			uint8_t RStick		:1;
			uint8_t LStick		:1;
			uint8_t Home		:1;
			uint8_t Share		:1;
			uint8_t unused2		:2; // =2
			//array[5]
			uint8_t D			:1;
			uint8_t U			:1;
			uint8_t R			:1;
			uint8_t L			:1;
			uint8_t unused3		:2;
			uint8_t L1			:1;
			uint8_t L2			:1;
			//array[6]~array[11]
			uint16_t LX			:12;
			uint16_t LY			:12;
			uint16_t RX			:12;
			uint16_t RY			:12;
		};
		uint8_t myth1;				//0x09~0x0b
		//array[13]
		union {
			struct {
				struct {
					int16_t acc_y;    //tail  -> postive
					int16_t acc_x;    //right -> postive
					int16_t acc_z;    //down  -> postive
					int16_t ang_roll; //right -> postive
					int16_t ang_pitch;//down  -> postive
					int16_t ang_yaw;  //ccw   -> postive
				} imu[3];
				uint8_t padding[15];
			};
			uint8_t extra_data[51];
		};
	};
	uint8_t array[64];
} NSCON_Report_TypeDef;

#endif /* __nscon_def_H */
