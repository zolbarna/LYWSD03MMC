#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"
/*
 *  LYWSD03MMC LCD buffer:  byte.bit

         --5.4--         --4.4--            --3.4--          BLE
  |    |         |     |         |        |         |        2.4
  |   5.5       5.0   4.5       4.0      3.5       3.0  
  |    |         |     |         |        |         |      o 2.5
 5.3     --5.1--         --4.1--            --3.1--          +--- 2.5
  |    |         |     |         |        |         |     2.5|
  |   5.6       5.2   4.6       4.2      3.6       3.2       ---- 2.6
  |    |         |     |         |        |         |     2.5|
         --5.7--         --4.7--     *      --3.7--          ---- 2.7
                                    4.3
                                        --1.4--         --0.4--
                                      |         |     |         |
          2.0      2.0               1.5       1.0   0.5       0.0
          / \      / \                |         |     |         |
    2.2(  ___  2.1 ___  )2.2            --1.1--         --0.1--
          2.1  / \ 2.1                |         |     |         |
               ___                   1.6       1.2   0.6       0.2     %
               2.0                    |         |     |         |     0.3
                                        --1.7--         --0.7--
                           BAT 1.3
*/


RAM uint8_t lcd_i2c_addr;

#define lcd_send_i2c_byte(a)  send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_buf(lcd_i2c_addr, (uint8_t *) b, a)

/* t,H,h,L,o,i  0xe2,0x67,0x66,0xe0,0xC6,0x40 */
#define LCD_SYM_H	0x67	// "H"
#define LCD_SYM_i	0x40	// "i"
#define LCD_SYM_L	0xE0	// "L"
#define LCD_SYM_o	0xC6	// "o"

#define LCD_SYM_BLE	0x10	// connect
#define LCD_SYM_BAT	0x08	// battery

const uint8_t lcd_init_cmd_b14[] =	{0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
								//	{0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6};
const uint8_t lcd_init_clr_b14[] =	{0x80,0x40,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0};

/* Test cmd ():
 * 0400007ceaa49cacbcf0fcc804ffffffff,
 * 0400007c0449
 * 0400007cf3c8 - blink
 */
const uint8_t lcd_init_b19[]	=	{
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xa4, // Display control (DISCTL): Normal mode, FRAME flip, Power save mode 1
//		0x9c, // Address set (ADSET): 0x1C ?
		0xac, // Display control (DISCTL): Power save mode 1, FRAME flip, Power save mode 1
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xf0, // Blink control (BLKCTL): Off
		0xfc, // All pixel control (APCTL): Normal
		0xc8, // Mode Set (MODE SET): Display ON, 1/3 Bias
		0x00, // Set Address 0
		// Clear 18 bytes RAM BU9792AFUV
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00
};

typedef struct __attribute__((packed)) _dma_uart_buf_t {
	volatile uint32_t dma_len;
	uint32_t head;
	uint8_t start;
	uint8_t data[6];
	uint8_t chk;
	uint8_t end;
} dma_uart_buf_t;

RAM dma_uart_buf_t utxb;

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const uint8_t display_numbers[] = {0xf5,0x05,0xd3,0x97,0x27,0xb6,0xf6,0x15,0xf7,0xb7, 0x77,0xe6,0xf0,0xc7,0xf2,0x72};

static _attribute_ram_code_ uint8_t reverse(uint8_t revByte) {
   revByte = (revByte & 0xF0) >> 4 | (revByte & 0x0F) << 4;
   revByte = (revByte & 0xCC) >> 2 | (revByte & 0x33) << 2;
   revByte = (revByte & 0xAA) >> 1 | (revByte & 0x55) << 1;
   return revByte;
}

// UART 38400 BAUD
#define UART_BAUD 38400
#if CLOCK_SYS_CLOCK_HZ == 16000000
#define uartCLKdiv 51 // 16000000/(7+1)/(51+1)=38461.538...
#define bwpc 7
#elif CLOCK_SYS_CLOCK_HZ == 24000000
#define uartCLKdiv 124 // 24000000/(4+1)/(124+1)=38400
#define bwpc 4
#elif CLOCK_SYS_CLOCK_HZ == 32000000
#define uartCLKdiv 103 // 32000000/(7+1)/(103+1)=38461.538...
#define bwpc 7
#elif CLOCK_SYS_CLOCK_HZ == 48000000
#define uartCLKdiv 124 // 48000000/(9+1)/(124+1)=38400
#define bwpc 9
#endif

_attribute_ram_code_
void lcd_send_uart(void) {
	// init uart
	reg_clk_en0 |= FLD_CLK0_UART_EN;
	///reg_clk_en1 |= FLD_CLK1_DMA_EN;
	uart_reset();
	// reg_uart_clk_div/reg_uart_ctrl0
	REG_ADDR32(0x094) = MASK_VAL(FLD_UART_CLK_DIV, uartCLKdiv, FLD_UART_CLK_DIV_EN, 1)
		|	((MASK_VAL(FLD_UART_BPWC, bwpc)	| (FLD_UART_TX_DMA_EN)) << 16) // set bit width, enable UART DMA mode
			| ((MASK_VAL(FLD_UART_CTRL1_STOP_BIT, 0)) << 24) // 00: 1 bit, 01: 1.5bit 1x: 2bits;
		;
	// reg_dma1_addr/reg_dma1_ctrl
	REG_ADDR32(0xC04) = (unsigned short)((u32)(&utxb)) //set tx buffer address
		| 	(((sizeof(utxb)+15)>>4) << 16); //set tx buffer size
	///reg_dma1_addrHi = 0x04; (in sdk init?)
	reg_dma_chn_en |= FLD_DMA_CHN_UART_TX;
	///reg_dma_chn_irq_msk |= FLD_DMA_IRQ_UART_TX;

	// GPIO_PD7 set TX UART pin
	REG_ADDR8(0x5AF) = (REG_ADDR8(0x5AF) &  (~(BIT(7)|BIT(6)))) | BIT(7);
	BM_CLR(reg_gpio_func(UART_TX_PD7), UART_TX_PD7 & 0xff);
	/// gpio_set_input_en(UART_TX_PD7, 1); ???

	// start send DMA
	reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX; // start tx
	// wait send (3.35 ms), sleep?
	pm_wait_us(3330); // 13 bytes * 10 bits / 38400 baud = 0.0033854 sec = 3.4 ms power ~3 mA
	//while (reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX); ?
	while (!(reg_uart_status1 & FLD_UART_TX_DONE));
	// set low/off power UART
	reg_uart_clk_div = 0;
}


_attribute_ram_code_
void send_to_lcd(void){
	unsigned int buff_index;
	uint8_t * p = display_buff;
	if(cfg.flg2.screen_off)
		return;
	if (lcd_i2c_addr) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		else {
			gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_10K);
			gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_10K);
		}
		reg_i2c_id = lcd_i2c_addr;
		if (lcd_i2c_addr == (B14_I2C_ADDR << 1)) {
			// B1.4, B1.7, B2.0
			reg_i2c_adr_dat = 0x4080;
			reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr = 0xC0;
			for(buff_index = 0; buff_index < sizeof(display_buff); buff_index++) {
				reg_i2c_do = *p++;
				reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
				while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			}
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
		} else { // (lcd_i2c_addr == (B19_I2C_ADDR << 1))
			// B1.9 BU9792AFUV
			reg_i2c_adr = 0x04;	// addr: 4
			reg_i2c_do = reverse(*p++);
			reg_i2c_di = reverse(*p++);
			reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_START;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr_dat = 0;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr = reverse(*p++);
			reg_i2c_do = reverse(*p++);
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr_dat = 0;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr = reverse(*p++);
			reg_i2c_do = reverse(*p);
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
		}
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	} else {
		// B1.6 (UART LCD)
		utxb.data[5] = *p++;
		utxb.data[4] = *p++;
		utxb.data[3] = *p++;
		utxb.data[2] = *p++;
		utxb.data[1] = *p++;
		utxb.data[0] = *p;
		utxb.chk = utxb.data[0]^utxb.data[1]^utxb.data[2]^utxb.data[3]^utxb.data[4]^utxb.data[5];
		utxb.dma_len = sizeof(utxb) - sizeof(utxb.dma_len);
		lcd_send_uart();
	}
}

//const uint8_t lcd_init_cmd[] = {0xea,0xf0, 0xc0, 0xbc}; // sleep all 9.4 uA

void init_lcd(void){
	lcd_i2c_addr = (uint8_t) scan_i2c_addr(B14_I2C_ADDR << 1);
	if (lcd_i2c_addr) { // B1.4, B1.7, B2.0
// 		GPIO_PB6 set in app_config.h!
//		gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K); // LCD on low temp needs this, its an unknown pin going to the LCD controller chip
		if(!cfg.flg2.screen_off) {
			pm_wait_ms(50);
			lcd_send_i2c_buf((uint8_t *) lcd_init_cmd_b14, sizeof(lcd_init_cmd_b14));
			lcd_send_i2c_buf((uint8_t *) lcd_init_clr_b14, sizeof(lcd_init_clr_b14));
		}
		return;
	}
	lcd_i2c_addr = (uint8_t) scan_i2c_addr(B19_I2C_ADDR << 1);
	if (lcd_i2c_addr) { // B1.9
		if(cfg.flg2.screen_off) {
			lcd_send_i2c_byte(0xEA); // BU9792AFUV reset
		} else {
			lcd_send_i2c_buf((uint8_t *) lcd_init_b19, sizeof(lcd_init_b19));
			lcd_send_i2c_buf((uint8_t *) lcd_init_b19, sizeof(lcd_init_b19));
		}
		return;
	}
	// B1.6 (UART), lcd_i2c_addr = 0
	// utxb.dma_len = 0;
	// utxb.head = 0;
	utxb.start = 0xAA;
	// utxb.data = {0};
	utxb.end = 0x55;
}

/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_attribute_ram_code_
void show_temp_symbol(uint8_t symbol) {
	display_buff[2] &= ~0xE0;
	display_buff[2] |= symbol & 0xE0;
}

/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */
_attribute_ram_code_
void show_smiley(uint8_t state){
	display_buff[2] &= ~0x07;
	display_buff[2] |= state & 0x07;
}

_attribute_ram_code_
void show_ble_symbol(bool state){
	if (state)
		display_buff[2] |= LCD_SYM_BLE;
	else 
		display_buff[2] &= ~LCD_SYM_BLE;
}

_attribute_ram_code_
void show_battery_symbol(bool state){
	if (state)
		display_buff[1] |= LCD_SYM_BAT;
	else 
		display_buff[1] &= ~LCD_SYM_BAT;
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number){
//	display_buff[4] = point?0x08:0x00;
	if (number > 19995) {
   		display_buff[3] = 0;
   		display_buff[4] = LCD_SYM_i; // "i"
   		display_buff[5] = LCD_SYM_H; // "H"
	} else if (number < -995) {
   		display_buff[3] = 0;
   		display_buff[4] = LCD_SYM_o; // "o"
   		display_buff[5] = LCD_SYM_L; // "L"
	} else {
		display_buff[5] = 0;
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			display_buff[4] = 0; // no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[5] = 2; // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[4] = 0x08; // point,
			if (number < 0){
				number = -number;
				display_buff[5] = 2; // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[5] |= 0x08; // "1" 1000..1999
		if (number > 99) display_buff[5] |= display_numbers[number / 100 % 10];
		if (number > 9) display_buff[4] |= display_numbers[number / 10 % 10];
		else display_buff[4] |= 0xF5; // "0"
	    display_buff[3] = display_numbers[number %10];
	}
}

/* -9 .. 99 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_small_number(int16_t number, bool percent){
	display_buff[1] = display_buff[1] & 0x08; // and battery
	display_buff[0] = percent?0x08:0x00;
	if (number > 99) {
		display_buff[0] |= LCD_SYM_i; // "i"
		display_buff[1] |= LCD_SYM_H; // "H"
	} else if (number < -9) {
		display_buff[0] |= LCD_SYM_o; // "o"
		display_buff[1] |= LCD_SYM_L; // "L"
	} else {
		if (number < 0) {
			number = -number;
			display_buff[1] = 2; // "-"
		}
		if (number > 9) display_buff[1] |= display_numbers[number / 10 % 10];
		display_buff[0] |= display_numbers[number %10];
	}
}

void show_ota_screen(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	display_buff[2] = BIT(4); // "ble"
	display_buff[3] = BIT(7); // "_"
	display_buff[4] = BIT(7); // "_"
	display_buff[5] = BIT(7); // "_"
	send_to_lcd();
	if(lcd_i2c_addr == B19_I2C_ADDR << 1)
		lcd_send_i2c_byte(0xf2);
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

#if	USE_CLOCK
_attribute_ram_code_
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;
	display_buff[0] = display_numbers[min % 10];
	display_buff[1] = display_numbers[min / 10 % 10];
	display_buff[2] = 0;
	display_buff[3] = display_numbers[hrs % 10];
	display_buff[4] = display_numbers[hrs / 10 % 10];
	display_buff[5] = 0;
}
#endif // USE_CLOCK

#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC
