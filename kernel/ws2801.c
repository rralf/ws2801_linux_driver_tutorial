/*
 * ws2801 - WS2801 LED driver running in Linux kernelspace
 *
 * Copyright (c) - Ralf Ramsauer, 2021
 *
 * Authors:
 *   Ralf Ramsauer <ralf.ramsauer@oth-regensburg.de>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>

#define DRIVER_NAME	"ws2801"

#define DEFAULT_NUM_LEDS	20
#define BYTES_PER_LED		3

struct ws2801 {
	const struct device *dev;

	const char *name;
	unsigned int num_leds;

	struct gpio_desc *clk;
	struct gpio_desc *data;

	struct mutex mutex;
	struct miscdevice misc_dev;
};

static inline void ws2801_send_byte(struct ws2801 *ws, unsigned char byte)
{
	unsigned char mask;

	for (mask = 0x80; mask; mask >>= 1) {
		gpiod_set_value(ws->clk, 0);
		gpiod_set_value(ws->data, (byte & mask) ? 1 : 0);
		gpiod_set_value(ws->clk, 1);
		udelay(1);
	}
}

static ssize_t ws2801_write(struct file *fp, const char __user *ubuf,
			    size_t cnt, loff_t *ppos)
{
	struct ws2801 *ws = container_of(fp->private_data, struct ws2801, misc_dev);
	unsigned char led[3];
	unsigned long copied;
	size_t i;

	if (cnt % BYTES_PER_LED != 0) {
		dev_warn(ws->dev, "Incorrect range %zu\n", cnt);
		return -ERANGE;
	}

	if (cnt / BYTES_PER_LED != 1) {
		dev_warn(ws->dev, "Only one LED is supported\n");
		return -EINVAL;
	}

	copied = copy_from_user(&led, ubuf, 3);
	if (copied) {
		dev_err(ws->dev, "Unable to copy from user\n");
		return -EINVAL;
	}

	mutex_lock(&ws->mutex);
	preempt_disable();
	for (i = 0; i < 3; i++)
		ws2801_send_byte(ws, led[i]);
	gpiod_set_value(ws->clk, 0);
	udelay(1000);
	preempt_enable();
	mutex_unlock(&ws->mutex);

	return cnt;
}

static const struct file_operations ws2801_fops = {
	.owner = THIS_MODULE,
	.write = &ws2801_write,
};

static int ws2801_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ws2801 *ws;
	int err;

	ws = devm_kzalloc(dev, sizeof(*ws), GFP_KERNEL);
	if (!ws)
		return -ENOMEM;
	platform_set_drvdata(pdev, ws);

	ws->dev = dev;
	ws->name = dev->of_node->name;

	err = of_property_read_u32(dev->of_node, "num-leds", &ws->num_leds);
	if (err) {
		dev_warn(dev,
			 "defaulting to " __stringify(DEFAULT_NUM_LEDS) " LEDs");
		ws->num_leds = DEFAULT_NUM_LEDS;
	} else
		dev_info(dev, "using %u LEDs\n", ws->num_leds);


	ws->clk = devm_gpiod_get(dev, "clk", GPIOD_OUT_LOW);
	if (IS_ERR(ws->clk)) {
		dev_err(dev, "error getting clk: %ld", PTR_ERR(ws->clk));
		return PTR_ERR(ws->clk);
	}

	ws->data = devm_gpiod_get(dev, "data", GPIOD_OUT_LOW);
	if (IS_ERR(ws->data)) {
		dev_err(dev, "error getting data: %ld", PTR_ERR(ws->data));
		return PTR_ERR(ws->data);
	}

	ws->misc_dev.minor = MISC_DYNAMIC_MINOR;
	ws->misc_dev.name = ws->name;
	ws->misc_dev.fops = &ws2801_fops;

	mutex_init(&ws->mutex);

	err = misc_register(&ws->misc_dev);

	return err;
}

static int ws2801_remove(struct platform_device *pdev)
{
	struct ws2801 *ws = platform_get_drvdata(pdev);

	misc_deregister(&ws->misc_dev);

	return 0;
}

static const struct of_device_id of_ws2801_match[] = {
	{
		.compatible = DRIVER_NAME,
	},
	{},
};

MODULE_DEVICE_TABLE(of, of_ws2801_match);

static struct platform_driver ws2801_driver = {
	.probe = ws2801_probe,
	.remove = ws2801_remove,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = of_ws2801_match,
	},
};

static int __init ws2801_module_init(void)
{
	int err;

	err = platform_driver_register(&ws2801_driver);

	return err;
}

static void __exit ws2801_module_exit(void)
{
	platform_driver_unregister(&ws2801_driver);
}

module_init(ws2801_module_init);
module_exit(ws2801_module_exit);

MODULE_AUTHOR("Ralf Ramsauer <ralf.ramsauer@oth-regensburg.de>");
MODULE_LICENSE("GPL");
