#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include "board.h"

static int pinmux_w53_init(const struct device *port)
{
	ARG_UNUSED(port);

	board_enable_vdd_ext(1);

	return 0;
}

SYS_INIT(pinmux_w53_init, PRE_KERNEL_1, 0);
