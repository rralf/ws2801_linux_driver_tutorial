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

struct led {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} __attribute__((packed));

struct ws {
	int fd;
	unsigned int num_leds;
	struct led *buf;
};

void ws_clear(struct ws *ws);
int ws_commit(struct ws *ws);
int ws_set_led(struct ws *ws, unsigned int no, const struct led *led);
void ws_full_on(struct ws *ws, const struct led *led);

int app(struct ws *ws);
