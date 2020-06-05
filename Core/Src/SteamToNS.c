#include "SteamToNS.h"
#include "usart.h"

#define SMOOTH_LENGTH 10
#define AMP_RATIO 2
#define DEADZONE_X 0x0c7
#define DEADZONE_Y 0x112
#define Y_X_RATIO 1
#define DRIFT_TIME 50
#define DRIFT_LIMIT 29000*29000

__ALIGN_BEGIN static NSCON_Report_TypeDef ns __ALIGN_END = {.array = {0x30,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

typedef union {
	struct {
		int16_t x;
		int16_t y;
	};
	int16_t arr[2];
	uint32_t u32;
} i16tuple;

void buffer_putn(char* buf, int num, uint8_t len){
	if(num < 0){
		*buf = '-';
		num = 0 - num;
	} else {
		*buf = ' ';
	}
	buf = buf + len;
	for(uint8_t i = 0; i < len; i++){
		*buf = 0x30 + (num % 10);
		num /= 10;
		buf--;
	}
}

i16tuple steamcon_pad_to_mouse(i16tuple in, uint8_t valid){
	i16tuple retval = {.x=0, .y=0};
	static i16tuple last = {.x=0, .y=0};
	static uint8_t last_valid = 0;
	if(valid && last_valid){
		retval.x = in.x - last.x;
		retval.y = in.y - last.y;
	}
	last = in;
	last_valid = valid;
	return retval;
}

i16tuple steamcon_mouse_smoother(i16tuple this_f1, uint8_t valid){
	static i16tuple retval = {.x=0, .y=0};
	
	static i16tuple last_f1 = {.x=0, .y=0};
	i16tuple this_f2;
	this_f2.x = this_f1.x - last_f1.x;
	this_f2.y = this_f1.y - last_f1.y;
	last_f1 = this_f1;
	
	static i16tuple history_f2[SMOOTH_LENGTH] = {0};
	static uint16_t history_f2_ptr = 0;
	history_f2[history_f2_ptr] = this_f2;
	history_f2_ptr++;
	if(history_f2_ptr >= SMOOTH_LENGTH) history_f2_ptr = 0;
	int32_t sumx = 0;
	int32_t sumy = 0;
	for(int i = 0; i < SMOOTH_LENGTH; i++){
		sumx += history_f2[i].x;
		sumy += history_f2[i].y;
	}
	sumx /= SMOOTH_LENGTH;
	sumy /= SMOOTH_LENGTH;
	
	retval.x += sumx;
	retval.y += sumy;
	
	static uint16_t counter = 0;
	if(valid){
		counter = 0;
	} else {
		if(counter <= SMOOTH_LENGTH){
			counter++;
		} else {
			retval.x = 0;
			retval.y = 0;
		}
	}
	
	return retval;
}

i16tuple steamcon_mouse_edge_drift(i16tuple in){
	i16tuple retval = {.x=0, .y=0};
	static uint16_t counter = 0;
	if(in.x*in.x+in.y*in.y >= DRIFT_LIMIT){
		if(counter < DRIFT_TIME){
			counter++;
		} else {
			retval.x = in.x;
			retval.y = in.y;
		}
	} else counter = 0;
	return retval;
}

NSCON_Report_TypeDef* steamtons(STEAM_Report_TypeDef* steam){
	ns.Y       = steam->X;
	ns.X       = steam->Y;
	ns.B       = steam->A;
	ns.A       = steam->B;
	ns.R1      = steam->R1;
	ns.R2      = steam->R2;
	ns.Minus   = steam->ML;
	ns.Plus    = steam->MR;
	ns.RStick  = steam->RP2;
	ns.LStick  = steam->LS1;
	ns.Home    = steam->MM;
	//ns.Share   = steam->;
	ns.D       = steam->D;
	ns.U       = steam->U;
	ns.R       = steam->R;
	ns.L       = steam->L;
	ns.L1      = steam->L1;
	ns.L2      = steam->L2;
	
	if(!steam->LPorLS){
		int16_t LX = (steam->LX >> 4) + 0x800;
		int16_t LY = (steam->LY >> 4) + 0x800;
		if(LX > 0xfff) LX = 0xfff;
		if(LY > 0xfff) LY = 0xfff;
		if(LX < 0x001) LX = 0x001;
		if(LY < 0x001) LY = 0x001;
		ns.LX = LX;
		ns.LY = LY;
	}
	
	i16tuple pad_atm = {.x=steam->RX, .y=steam->RY};
	i16tuple mouse_atm = steamcon_pad_to_mouse(pad_atm, steam->RP1|steam->RP2);
	i16tuple smooth_stick = steamcon_mouse_smoother(mouse_atm, steam->RP1|steam->RP2);
	i16tuple edge_drift = steamcon_mouse_edge_drift(pad_atm);
	static int32_t RX_acc = 0;
	static int32_t RY_acc = 0;
	RX_acc += smooth_stick.x * AMP_RATIO;
	RY_acc += smooth_stick.y * AMP_RATIO * Y_X_RATIO;
	int16_t RX;
	int16_t RY;
	if(edge_drift.x) RX = edge_drift.x >> 4;
	else {
		if(RX_acc >= 0xfff-0x800-DEADZONE_X){
			RX_acc -= 0xfff-0x800-DEADZONE_X;
			RX = 0xfff-0x800-DEADZONE_X;
		} else if(RX_acc <= 0x001-0x800-DEADZONE_X) {
			RX_acc -= 0x001-0x800-DEADZONE_X;
			RX = 0x001-0x800-DEADZONE_X;
		} else {
			RX = RX_acc;
			RX_acc = 0;
		}
	}
	if(edge_drift.y) RY = edge_drift.y >> 4;
	else {
		if(RY_acc >= 0xfff-0x800-DEADZONE_Y){
			RY_acc -= 0xfff-0x800-DEADZONE_Y;
			RY = 0xfff-0x800-DEADZONE_Y;
		} else if(RY_acc <= 0x001-0x800-DEADZONE_Y) {
			RY_acc -= 0x001-0x800-DEADZONE_Y;
			RY = 0x001-0x800-DEADZONE_Y;
		} else {
			RY = RY_acc;
			RY_acc = 0;
		}
	}
	if     (RX > 0) RX += DEADZONE_X;
	else if(RX < 0) RX -= DEADZONE_X;
	if     (RY > 0) RY += DEADZONE_Y;
	else if(RY < 0) RY -= DEADZONE_Y;
	RX += 0x800;
	RY += 0x800;
	if(RX > 0xfff) RX = 0xfff;
	if(RY > 0xfff) RY = 0xfff;
	if(RX < 0x001) RX = 0x001;
	if(RY < 0x001) RY = 0x001;
	ns.RX = RX;
	ns.RY = RY;
	
	for(int i = 0; i < 3; i++){
		ns.imu[i].acc_y = (  steam->acc_y) / 4;
		ns.imu[i].acc_x = (0-steam->acc_x) / 4;
		ns.imu[i].acc_z = (  steam->acc_z) / 4;
		ns.imu[i].ang_roll  = (0-steam->ang_roll)  * 1;
		ns.imu[i].ang_pitch = (0-steam->ang_pitch) * 1;
		ns.imu[i].ang_yaw   = (  steam->ang_yaw)   * 1;
	}
	
	return &ns;
	/*
	HAL_UART_Transmit_DMA(&huart1, ns.array, sizeof(NSCON_Report_TypeDef));
	
	static char send_buf[] = "-00000, -00000, -00000\n";
	buffer_putn(send_buf+0, mouse_atm.x, 5);
	buffer_putn(send_buf+8, smooth_stick.x, 5);
	buffer_putn(send_buf+16, RX, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)send_buf, sizeof(send_buf)-1);
	*/
}
