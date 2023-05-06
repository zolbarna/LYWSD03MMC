#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_MHO_C401
/* Based on source: https://github.com/znanev/ATC_MiThermometer */
#include "app.h"
#include "lcd.h"
#include "epd.h"
#include "drivers/8258/pm.h"
#include "drivers/8258/timer.h"

#define DEF_EPD_REFRESH_CNT	32

RAM uint8_t stage_lcd;
RAM uint8_t flg_lcd_init;
RAM uint8_t lcd_refresh_cnt;
RAM uint8_t epd_updated;
//----------------------------------
// LUTV, LUT_KK and LUT_KW values taken from the actual device with a
// logic analyzer
//----------------------------------
const uint8_t T_LUTV_init[15] = {0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KK_init[15] = {0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KW_init[15] = {0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KK_update[15] = {0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KW_update[15] = {0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x81, 0x81, 0x01};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const uint8_t top_left[22] = {16, 7, 15, 4, 14, 1, 14, 7, 12, 5, 12, 4, 13, 3, 15, 7, 15, 6, 15, 5, 14, 0};
const uint8_t top_middle[22] = {15, 0, 15, 1, 14, 6, 13, 0, 13, 5, 13, 4, 14, 5, 14, 4, 15, 3, 15, 2, 14, 3};
const uint8_t top_right[22] = {13, 1, 13, 7, 12, 1, 12, 7, 11, 5, 11, 2, 12, 6, 12, 0, 13, 6, 13, 2, 12, 2};
const uint8_t bottom_left[22] = {9, 1, 9, 7, 8, 5, 1, 1, 0, 3, 1, 4, 9, 4, 10, 0, 10, 6, 10, 3, 8, 2};
const uint8_t bottom_right[22] = {7, 7, 6, 5, 2, 0, 2, 3, 0, 2, 1, 7, 2, 6, 7, 4, 7, 1, 8, 6, 6, 2};

// These values closely reproduce times captured with logic analyser
//#define delay_SPI_end_cycle() pm_wait_us(3) // 1.5 us
//#define delay_EPD_SCL_pulse() pm_wait_us(3) // 1.5 us
#define delay_SPI_end_cycle() sleep_us(2)
#define delay_EPD_SCL_pulse() sleep_us(2)

/*
Now define how each digit maps to the segments:
          1
 10 :-----------
    |           |
  9 |           | 2
    |     11    |
  8 :-----------: 3
    |           |
  7 |           | 4
    |     5     |
  6 :----------- 
*/

const uint8_t digits[16][11] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0},  // 0
    {2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0},   // 1
    {1, 2, 3, 5, 6, 7, 8, 10, 11, 0, 0}, // 2
    {1, 2, 3, 4, 5, 6, 10, 11, 0, 0, 0}, // 3
    {2, 3, 4, 8, 9, 10, 11, 0, 0, 0, 0}, // 4
    {1, 3, 4, 5, 6, 8, 9, 10, 11, 0, 0}, // 5
    {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0}, // 6
    {1, 2, 3, 4, 10, 0, 0, 0, 0, 0, 0},  // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // 8
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 0}, // 9
    {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 0}, // A
    {3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0}, // b
    {5, 6, 7, 8, 11, 0, 0, 0, 0, 0, 0},  // c
    {2, 3, 4, 5, 6, 7, 8, 11, 0, 0, 0},  // d
    {1, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0}, // E
    {1, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}  // F
};

/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_attribute_ram_code_ void show_temp_symbol(uint8_t symbol) {
	if (symbol & 0x20)
		display_buff[16] |= BIT(5); // "Г", "%", "( )", "."
	else
		display_buff[16] &= ~BIT(5); // "Г", "%", "( )", "."
	if (symbol & 0x40)
		display_buff[16] |= BIT(6); //"-"
	else
		display_buff[16] &= ~BIT(6); //"-"
	if (symbol & 0x80)
		display_buff[14] |= BIT(2); // "_"
	else
		display_buff[14] &= ~BIT(2); // "_"
}
/* 0 = "   " off,
 * 1 = " o "
 * 2 = "o^o"
 * 3 = "o-o"
 * 4 = "oVo"
 * 5 = "vVv" happy
 * 6 = "^-^" sad
 * 7 = "oOo" */
_attribute_ram_code_ void show_smiley(uint8_t state){
	// off
	display_buff[3] = 0;
	display_buff[4] = 0;
	display_buff[5] &= BIT(0); // "*"
	switch(state & 7) {
		case 1:
			display_buff[3] |= BIT(2);
			display_buff[4] |= BIT(4);
		break;
		case 2:
			display_buff[4] |= BIT(1) | BIT(4);
			display_buff[5] |= BIT(6);
		break;
		case 3:
			display_buff[4] |= BIT(1) | BIT(7);
			display_buff[5] |= BIT(6);
		break;
		case 4:
			display_buff[3] |= BIT(2);
			display_buff[4] |= BIT(1);
			display_buff[5] |= BIT(6);
		break;
		case 5:
			display_buff[3] |= BIT(2);
			display_buff[4] |= BIT(1);
		break;
		case 6:
			display_buff[4] |= BIT(7);
			display_buff[5] |= BIT(6);
		break;
		case 7:
			display_buff[3] |= BIT(2);
			display_buff[4] |= BIT(1) | BIT(4);
			display_buff[5] |= BIT(6);
		break;
	}
}

_attribute_ram_code_ void show_battery_symbol(bool state){
	if (state)
		display_buff[16] |= BIT(4);
	else
		display_buff[16] &= ~BIT(4);
}

_attribute_ram_code_ void show_ble_symbol(bool state){
	if (state)
		display_buff[5] |= BIT(0); // "*"
	else
		display_buff[5] &= ~BIT(0);
}

// 223 us
_attribute_ram_code_ __attribute__((optimize("-Os"))) static void transmit(uint8_t cd, uint8_t data_to_send) {
    gpio_write(EPD_SCL, LOW);
    // enable SPI
    gpio_write(EPD_CSB, LOW);
    delay_EPD_SCL_pulse();

    // send the first bit, this indicates if the following is a command or data
    if (cd != 0)
        gpio_write(EPD_SDA, HIGH);
    else
        gpio_write(EPD_SDA, LOW);
    delay_EPD_SCL_pulse();
    gpio_write(EPD_SCL, HIGH);
    delay_EPD_SCL_pulse();

    // send 8 bits
    for (int i = 0; i < 8; i++) {
        // start the clock cycle
        gpio_write(EPD_SCL, LOW);
        // set the MOSI according to the data
        if (data_to_send & 0x80)
            gpio_write(EPD_SDA, HIGH);
        else
            gpio_write(EPD_SDA, LOW);
        // prepare for the next bit
        data_to_send = (data_to_send << 1);
        delay_EPD_SCL_pulse();
        // the data is read at rising clock (halfway the time MOSI is set)
        gpio_write(EPD_SCL, HIGH);
        delay_EPD_SCL_pulse();
    }

    // finish by ending the clock cycle and disabling SPI
    gpio_write(EPD_SCL, LOW);
    delay_SPI_end_cycle();
    gpio_write(EPD_CSB, HIGH);
    delay_SPI_end_cycle();
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) static void epd_set_digit(uint8_t *buf, uint8_t digit, const uint8_t *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < 11; i++) {
        // get the segment needed to display the digit 'digit',
        // this is stored in the array 'digits'
        int segment = digits[digit][i] - 1;
        // segment = -1 indicates that there are no more segments to display
        if (segment >= 0) {
            segment_byte = segments[2 * segment];
            segment_bit = segments[1 + 2 * segment];
            buf[segment_byte] |= (1 << segment_bit);
        }
        else
            // there are no more segments to be displayed
            break;
    }
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number){
	display_buff[11] = 0;
	display_buff[12] = 0;
	display_buff[13] = 0;
	display_buff[14] &= BIT(2); //"_"
	display_buff[15] = 0;
	display_buff[16] &= BIT(4) | BIT(5) | BIT(6);
	if (number > 19995) {
		// "Hi"
   		display_buff[12] = BIT(4);
   		display_buff[13] = BIT(0) | BIT(3);
   		display_buff[14] = BIT(0) | BIT(1) | BIT(6) | BIT(7);
   		display_buff[15] = BIT(4) | BIT(5) | BIT(6) | BIT(7);
	} else if (number < -995) {
		// "Lo"
   		display_buff[12] = BIT(4) | BIT(5);
   		display_buff[13] = BIT(0) | BIT(3) | BIT(4) | BIT(5);
   		display_buff[14] = BIT(3) | BIT(4) | BIT(5);
   		display_buff[15] = BIT(5) | BIT(6) | BIT(7);
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			display_buff[16] &= ~BIT(5); // no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[14] = BIT(0); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[16] |= BIT(5); // point
			if (number < 0){
				number = -number;
				display_buff[14] = BIT(0); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[12] |= BIT(3); // "1" 1000..1999
		if (number > 99) epd_set_digit(display_buff, number / 100 % 10, top_left);
		if (number > 9) epd_set_digit(display_buff, number / 10 % 10, top_middle);
		else epd_set_digit(display_buff, 0, top_middle);
		epd_set_digit(display_buff, number % 10, top_right);
	}
}

/* -9 .. 99 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void show_small_number(int16_t number, bool percent){
	display_buff[0] = 0;
	display_buff[1] = 0;
	display_buff[2] = 0;
	display_buff[6] = 0;
	display_buff[7] = 0;
	display_buff[8] = 0;
	display_buff[9] = 0;
	display_buff[10] = 0;
	if (percent)
		display_buff[16] |= BIT(5); // "%" TODO 'C', '.', '(  )' ?
	if (number > 99) {
		// "Hi"
		display_buff[1] |= BIT(1) | BIT(4);
		display_buff[2] |= BIT(0) | BIT(3);
		display_buff[8] |= BIT(2) | BIT(5);
		display_buff[9] |= BIT(4) | BIT(7);
		display_buff[10] |= BIT(0) | BIT(3) | BIT(6);
	} else if (number < -9) {
		//"Lo"
		display_buff[0] |= BIT(2) | BIT(3);
		display_buff[1] |= BIT(4) | BIT(7);
		display_buff[2] |= BIT(6) | BIT(3);
		display_buff[6] |= BIT(2);
		display_buff[9] |= BIT(4);
		display_buff[10] |= BIT(0) | BIT(3) | BIT(6);
	} else {
		if (number < 0) {
			number = -number;
			display_buff[8] |= BIT(2); // "-"
		}
		if (number > 9) epd_set_digit(display_buff, number / 10 % 10, bottom_left);
		epd_set_digit(display_buff, number % 10, bottom_right);
	}
}

void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
    pm_wait_us(110);
	lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
    stage_lcd = 1;
    epd_updated = 0;
    flg_lcd_init = 3;
    gpio_write(EPD_RST, HIGH);
}

_attribute_ram_code_ void update_lcd(void){
	if(cfg.flg2.screen_off) {
		stage_lcd = 0;
		return;
	}
	if (!stage_lcd) {
		if (memcmp(display_cmp_buff, display_buff, sizeof(display_buff))) {
			memcpy(display_cmp_buff, display_buff, sizeof(display_buff));
			lcd_flg.b.send_notify = lcd_flg.b.notify_on; // set flag LCD for send notify
			if (lcd_refresh_cnt) {
				lcd_refresh_cnt--;
				flg_lcd_init = 0;
				stage_lcd = 1;
			} else {
				init_lcd(); // pulse RST_N low for 110 microseconds
			}
		}
	}
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) int task_lcd(void) {
	if(cfg.flg2.screen_off) {
		stage_lcd = 0;
		return stage_lcd;
	}
	if (gpio_read(EPD_BUSY)) {
		switch (stage_lcd) {
		case 1: // Update/Init lcd, stage 1
			if (flg_lcd_init)
				flg_lcd_init--;
			// send Charge Pump ON command
			transmit(0, POWER_ON);
			// wait ~30 ms for the display to become ready to receive new
			stage_lcd = 2;
			break;
		case 2: // Update/Init lcd, stage 2
			if (epd_updated == 0) {
				// send next blocks ~25 ms
				transmit(0, PANEL_SETTING);
				transmit(1, 0x0B);
				transmit(0, POWER_SETTING);
				transmit(1, 0x46);
				transmit(1, 0x46);
				transmit(0, POWER_OFF_SEQUENCE_SETTING);
				if (flg_lcd_init)
					transmit(1, 0x00);
				else
					transmit(1, 0x06);
				// Frame Rate Control
				transmit(0, PLL_CONTROL);
				if (flg_lcd_init)
					transmit(1, 0x03); // transmit(1, 0x02);
				else {
					transmit(1, 0x07); // transmit(1, 0x03);
					// NOTE: Original firmware makes partial refresh on update, but not when initialising the screen.
					transmit(0, PARTIAL_DISPLAY_REFRESH);
					transmit(1, 0x00);
					transmit(1, 0x87);
					transmit(1, 0x01);
				}
				// send the e-paper voltage settings (waves)
				transmit(0, LUT_FOR_VCOM);
				for (int i = 0; i < 15; i++)
					transmit(1, T_LUTV_init[i]);

				if (flg_lcd_init) {
					transmit(0, LUT_CMD_0x23);
					if (flg_lcd_init == 1) { // pass 2
						for (int i = 0; i < 15; i++)
							transmit(1, T_LUT_KW_update[i]);
						transmit(0, LUT_CMD_0x26);
						for (int i = 0; i < 15; i++)
							transmit(1, T_LUT_KK_update[i]);
						// start an initialization sequence (white - all 0x00)
						transmit(0, DATA_START_TRANSMISSION_1);
						for (int i = 0; i < 18; i++)
							transmit(1, 0);
						transmit(0, DATA_START_TRANSMISSION_2);
						for (int i = 0; i < 18; i++)
							transmit(1, 0);
					} else { // pass 1
						for (int i = 0; i < 15; i++)
							transmit(1, T_LUT_KK_init[i]);
						transmit(0, LUT_CMD_0x26);
						for (int i = 0; i < 15; i++)
							transmit(1, T_LUT_KW_init[i]);
						// start an initialization sequence (black - all 0xFF)
						transmit(0, DATA_START_TRANSMISSION_1);
						for (int i = 0; i < 18; i++)
							transmit(1, 0xff);
						transmit(0, DATA_START_TRANSMISSION_2);
						for (int i = 0; i < 18; i++)
							transmit(1, 0xff);
					}
				} else {
					transmit(0, LUT_CMD_0x23);
					for (int i = 0; i < 15; i++)
						transmit(1, T_LUTV_init[i]);

					transmit(0, LUT_CMD_0x24);
					for (int i = 0; i < 15; i++)
						transmit(1, T_LUT_KK_update[i]);

					transmit(0, LUT_CMD_0x25);
					for (int i = 0; i < 15; i++)
						transmit(1, T_LUT_KW_update[i]);

					transmit(0, LUT_CMD_0x26);
					for (int i = 0; i < 15; i++)
						transmit(1, T_LUTV_init[i]);
					// send the actual data
					transmit(0, DATA_START_TRANSMISSION_1);
					for (int i = 0; i < 18; i++)
						transmit(1, display_buff[i]);
				}
			} else {
				// send the actual data
				transmit(0, DATA_START_TRANSMISSION_1);
				for (int i = 0; i < 18; i++)
					transmit(1, display_buff[i]);
			}
			stage_lcd = 3;
			// Refresh
			transmit(0, DISPLAY_REFRESH);
			// wait ~1256 ms for the display to become ready to receive new
			break;
		case 3: // Update/Init lcd, stage 3
			// send Charge Pump OFF command
			transmit(0, POWER_OFF);
			transmit(1, 0x03);
			if (flg_lcd_init)
				// wait ~20 ms for the display to become ready to receive new
				stage_lcd = 1;
			else {
				epd_updated = 1;
				stage_lcd = 0;
			}
			break;
		default:
			stage_lcd = 0;
		}
	}
	return stage_lcd;
}

#if	USE_CLOCK
_attribute_ram_code_ void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;
	memset(display_buff, 0, sizeof(display_buff));
	epd_set_digit(display_buff, min / 10 % 10, bottom_left);
	epd_set_digit(display_buff, min % 10, bottom_right);
	epd_set_digit(display_buff, hrs / 10 % 10, top_left);
	epd_set_digit(display_buff, hrs % 10, top_middle);
}
#endif // USE_CLOCK

#endif // DEVICE_TYPE == DEVICE_MHO_C401
