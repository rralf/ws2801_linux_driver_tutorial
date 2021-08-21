/*
 * ws2801 - WS2801 LED driver running in Linux userspace
 *
 * Copyright (c) - Ralf Ramsauer, 2021
 *
 * Authors:
 *   Ralf Ramsauer <ralf.ramsauer@oth-regensburg.de>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ws2801.h"

int app(struct ws *ws)
{
	int err, i;
	const struct led led = {
		.r = 255,
		.g = 255,
		.b = 255,
	};

	for (i = 0; ; i++) {
		ws_clear(ws);

		err = ws_set_led(ws, i % ws->num_leds, &led);
		if (err) {
			fprintf(stderr, "set led\n");
			break;
		}
		err = ws_commit(ws);
		if (err) {
			fprintf(stderr, "commit\n");
			break;
		}
		usleep(10000);
	}

	return err;
}
