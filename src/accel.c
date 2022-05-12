#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/sensor.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include "state.h"
#include "disp.h"
#include "accel.h"

__UNUSED static const struct device* sensor;
__UNUSED static const struct device* sensor_bus;
extern struct g_state state;

void acc_app_init(void);

int accel_get_mg(int32_t accel[3])
{
	return 0;
}

int accel_high_latency(bool high)
{
	struct sensor_value freq;
	if(high) {
		freq.val1 = 1;
	} else {
		freq.val1 = 50;
	}

	return 0;
}

int accel_init(void)
{
	return 0;
}

/* Imported from ledwatch project
 * TODO: Improve upstream driver someday */
static uint8_t acc_read_reg(uint8_t reg_addr)
{
	return 0;
}

static void acc_write_reg(uint8_t reg_addr, uint8_t value)
{
}

static void acc_hpf_config(uint8_t config)
{
}

static void acc_int1_sources(uint8_t sources)
{
}

static void acc_int2_sources(uint8_t sources)
{
}

static void acc_click_set(uint8_t sources, uint16_t threshold,
			  uint16_t limit_ms, uint16_t latency_ms,
			  uint16_t window_ms)
{
	//----Config----
	uint8_t oldcfg = acc_read_reg(0x38);
	sources &= 0x3F; // 6 bits
	sources |= oldcfg;
	acc_write_reg(0x38, sources); // Write tap_cfg


	//----Threshold----
	uint8_t fs = acc_read_reg(0x23);
	fs >>= 4;   // Data in MSB
	fs &= 0x03; // 2 bits

	switch(fs) {
		case ACC_FS_2G:
			threshold /= 16;
			break;
		case ACC_FS_4G:
		threshold /= 31;
			break;
		case ACC_FS_8G:
			threshold /= 63;
			break;
		case ACC_FS_16G:
			threshold /= 125;
			break;
		default:
			return;
	}

	threshold &= 0x7F; // 7 bits
	acc_write_reg(0x3A, threshold); // Write tap_thr


	//----Time parameters----
	float factor_ms;
	uint8_t rate = acc_read_reg(0x20);  // Read cfg_reg_1
	rate >>= 4;   // Data is in MSB
	rate &= 0x0F; // 4 bits

	switch(rate) {
		case ACC_RATE_1:
			factor_ms = 1000;
			break;
		case ACC_RATE_10:
			factor_ms = 100;
			break;
		case ACC_RATE_25:
			factor_ms = 40;
			break;
		case ACC_RATE_50:
			factor_ms = 20;
			break;
		case ACC_RATE_100:
			factor_ms = 10;
			break;
		case ACC_RATE_200:
			factor_ms = 5;
			break;
		case ACC_RATE_400:
			factor_ms = 2.5;
			break;
		case ACC_RATE_1250:
			factor_ms = 0.8;
			break;
		case ACC_RATE_1600_LP:
			factor_ms = 0.625;
			break;
		// case ACC_RATE_5000_LP:
		// 	factor_ms = 5;
		// 	break;
		default:
			return;
			break;
	}
	limit_ms /= factor_ms;
	latency_ms /= factor_ms;
	window_ms /= factor_ms;

	limit_ms &= 0x7F; // 7 bits
	latency_ms &= 0xFF; // 8 bits
	window_ms &= 0xFF; // 8 bits

	acc_write_reg(0x3B, limit_ms); // Write tap_limit
	acc_write_reg(0x3C, latency_ms); // Write tap_latency
	acc_write_reg(0x3D, window_ms); // Write tap_limit
}

/* App-level */
static void acc_enable_click(void)
{
	acc_click_set(TAP_ZD, // Enable Z double-tap, X and Y single-tap
		      800, // Tap-threshold is 0.3 G
		      200, // Tap detected if acc_value decreases within 180ms
		      80, // Pause 80ms before starting double-tap detection
		      100); // User has to tap again withing 100 ms to register a double-tap
	// Enable only Z-axis double-click
	acc_write_reg(0x38, TAP_ZD);
}

__UNUSED static struct gpio_callback acc_cb_data;
__UNUSED static void acc_callback(const struct device *dev,
			 struct gpio_callback *cb,
			 uint32_t pins)
{
	if(state.motion_wake)
		k_wakeup(state.main_tid); /* Wake from sleep */
}

void acc_app_init(void)
{
	/* Enable accelerometer motion app wakeup */
	state.motion_wake = 1;

	// Enable hpf on click only, fc = 1Hz @ fs=50Hz
	acc_hpf_config(HP_MODE_NORMAL | HPCLICK);
	acc_enable_click(); // Enable doubleclick detection on Z-axis

	// Tap-detection interrupt on output 1
	acc_int1_sources(I1_CLICK);
	// Orientation detection interrupt on output 2
	acc_int2_sources(I2_CLICK | I2_IG1 | I2_HLACTIVE);
}
