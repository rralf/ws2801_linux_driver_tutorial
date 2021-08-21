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

#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#define DRIVER_NAME	"ws2801"

static int ws2801_probe(struct platform_device *pdev)
{
	return -1;
}

static int ws2801_remove(struct platform_device *pdev)
{
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
