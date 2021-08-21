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

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ws2801.h"

int ws_commit(struct ws *ws)
{
	ssize_t i;
	unsigned int size;

	size = ws->num_leds * sizeof(struct led);
	i = write(ws->fd, ws->buf, size);
	if (i != size)
		return -EINVAL;
	
	return 0;
}

void ws_clear(struct ws *ws)
{
	memset(ws->buf, 0, sizeof(struct led) * ws->num_leds);
}

int ws_set_led(struct ws *ws, unsigned int no, const struct led *led)
{
	if (no >= ws->num_leds)
		return -ERANGE;

	ws->buf[no] = *led;

	return 0;
}

void ws_full_on(struct ws *ws, const struct led *led)
{
	unsigned int i;

	for (i = 0; i < ws->num_leds; i++)
		ws->buf[i] = *led;
}

static void __attribute__((noreturn)) usage(int exit_code)
{
	FILE *s;

	if (exit_code)
		s = stderr;
	else
		s = stdout;

	fprintf(s, "Usage: { -k DEVICE_NAME }\n"
		   "       [ -h ]\n");

	exit(exit_code);
}

static int ws_open(const char *device, struct ws *ws)
{
	int err;

	ws->fd = open(device, O_RDWR);
	if (ws->fd == -1)
		return -errno;

	err = ioctl(ws->fd, 0, &ws->num_leds);
	if (err) {
		close(ws->fd);
		return -EINVAL;
	}

	ws->buf = malloc(sizeof(struct led) * ws->num_leds);
	if (!ws->buf) {
		close(ws->fd);
		return -ENOMEM;
	}

	ws_clear(ws);

	return 0;
}

static void ws_close(struct ws *ws)
{
	if (ws->buf)
		free(ws->buf);

	if (ws->fd != -1)
		close(ws->fd);
}

int main(int argc, char **argv)
{
	const char *device_name = NULL;
	struct ws ws;
	int option, err;

	option = 0;

	while ((option = getopt(argc, argv, "k:h")) != -1) {
		switch (option) {
			case 'k':
				device_name = optarg;
				break;
			default:
				usage(-1);
		}
	}

	if (!device_name)
		usage(-1);

	err = ws_open(device_name, &ws);
	if (err) {
		fprintf(stderr, "initialising ws2801: %s\n", strerror(-err));
		return err;
	}

	err = app(&ws);

	ws_close(&ws);

	return err;
}
