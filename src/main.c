#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <string.h>
#include <drivers/display.h>
#include <lvgl.h>
#include "disp.h"
#include "cts.h"
#include "calendar.h"
#include "clock.h"
#include "board.h"
#include "accel.h"
#include "state.h"
#include "screen.h"
#include "ble.h"

struct g_state state;

void display_test()
{
	uint32_t count = 0U;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = device_get_binding(CONFIG_LV_Z_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		printk("device not found.  Aborting test.");
		return;
	}

	hello_world_label = lv_label_create(lv_scr_act());

	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

	count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
		}
		lv_task_handler();
		k_sleep(K_MSEC(10));
		++count;
	}
}

void main(void)
{
	/* Reset global state */
	memset(&state, 0, sizeof(state));
	/* Store main thread ID */
	state.main_tid = k_current_get();

	/* Init clock module */
	clock_time_init();
	time_struct_t new_time = {12, 0, 0};
	clock_set_time(new_time);
	struct date_time new_date = {0, 0, 0, 0, 0, 0};
	cal_set_date(&new_date);


	board_gpio_setup();
	/* FIXME: will block in here */
	display_test();

	/* board_enable_vdd_ext(1); */
	ble_init();

	/* Advertise and sleep */
	ble_adv(1);
	for(;;)
	{
		__NOP();
		k_msleep(1000);
	}
	/* state.pgm_state = PGM_STATE_CLOCK; */

	/* while(1) */
	/* { */
	/* 	main_state_loop(); */
	/* } */
}
