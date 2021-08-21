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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ws2801.h"

static void get_next(struct led *led)
{
	led->r += 10;
	led->g += 10;
	led->b += 10;
}

int app(struct ws *ws)
{
	int err, i;
	struct led first, next;

	srand(time(NULL));
	first.r = rand();
	first.g = rand();
	first.b = rand();
	for (;;) {
		next = first;
		for (i = 0; i < ws->num_leds; i++) {
			err = ws_set_led(ws, i, &next);
			if (err)
				return err;
			get_next(&next);
		}

		ws_commit(ws);
		usleep(30000);
		get_next(&first);
	}

	return err;
}
