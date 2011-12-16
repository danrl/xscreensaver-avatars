/* avatars, Copyright (c) 2011 Dan Luedtke <mail@danrl.de>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#include "screenhack.h"
#include <stdio.h>
#include "images/avatars_penguin.xpm"
#include "images/avatars_danrl.xpm"
#include "images/avatars_yourlogo.xpm"

#define AVATARS_FADE_STEPS	50	/* min=1, max=100 */

struct state {
	int next_state;
	int show;
	int fade;
	int blank;
	int xpos;
	int ypos;
	int width;
	int height;
	XWindowAttributes wa;
	GC gc;
	XImage *image;
	XImage *op[AVATARS_FADE_STEPS];
};

static void
avatars_precalc_fade (Display *display, struct state *st) {
	int x, y, k;
	unsigned long p, a, r, g, b;

	for (k = 0; k < AVATARS_FADE_STEPS; k++) {
		a = (1000 / AVATARS_FADE_STEPS) * k;
		for (x = 0; x < st->width; x++) {
			for (y = 0; y < st->height; y++) {
				/* alpha blending to black background */
				p = XGetPixel(st->op[k], x, y);
				r = (p >> 16) & 0xff;
				g = (p >> 8) & 0xff;
				b = (p >> 0) & 0xff;
				r = (r * a) / 1000;
				g = (g * a) / 1000;
				b = (b * a) / 1000;
				p = (r << 16) | (g << 8) | b;
				XPutPixel(st->op[k], x, y, p);
			}
		}
	}
}

static void *
avatars_init(Display *display, Window window) {
	struct state *st = (struct state *) calloc (1, sizeof(*st));
	int k;
	char **src;

	/* starting state is fade in */
	st->next_state = 0;

	/* get and check parameters */
	switch (get_integer_resource(display, "image", "Integer")) {
	case 1:
		src = avatars_danrl_xpm;
		break;
	case 2:
		src = avatars_yourlogo_xpm;
		break;
	default:
		src = avatars_penguin_xpm;
		break;
	}
	st->show = get_integer_resource(display, "show", "Integer");
	if (st->show < 1)
		st->show = 1;
	st->fade = get_integer_resource(display, "blank", "Integer");
	if (st->blank < 1)
		st->blank = 1;
	st->fade = get_integer_resource(display, "fade", "Integer");
	if (st->fade < 1)
		st->fade = 1;

	/* allocate images */
	XpmCreateImageFromData(display, src, &st->image, NULL, NULL);
	for (k = 0; k < AVATARS_FADE_STEPS; k++) {
		XpmCreateImageFromData(display, src, &st->op[k], NULL, NULL);
	}
	st->width = (st->image)->width;
	st->height = (st->image)->height;

	/* create GC */
	XGetWindowAttributes(display, window, &st->wa);
	st->gc = XCreateGC(display, window, 0, NULL);

	/* random starting position */
	st->xpos = random()%(st->wa.width - st->width);
	st->ypos = random()%(st->wa.height - st->height);

	avatars_precalc_fade(display, st);

	return st;
}

static unsigned long
avatars_draw (Display *display, Window window, void *closure) {
	struct state *st = (struct state *) closure;
	long delay;
	int k;

	if (st->next_state < AVATARS_FADE_STEPS) {
		/* fade in */
		k = st->next_state;
		XPutImage(display, window, st->gc, st->op[k], 0, 0,
			  st->xpos, st->ypos, st->width, st->height);
		st->next_state++;
		delay = (1000000 / AVATARS_FADE_STEPS) * st->fade;
	} else if (st->next_state == AVATARS_FADE_STEPS) {
		/* show avatar */
		XPutImage(display, window, st->gc, st->image, 0, 0,
			  st->xpos, st->ypos, st->width, st->height);
		st->next_state++;
		delay = 1000000 * st->show;
	} else if (st->next_state > AVATARS_FADE_STEPS &&
		   st->next_state < 2*AVATARS_FADE_STEPS) {
		/* fade out */
		k = AVATARS_FADE_STEPS - (st->next_state % AVATARS_FADE_STEPS);
		XPutImage(display, window, st->gc, st->op[k], 0, 0,
			  st->xpos, st->ypos, st->width, st->height);
		st->next_state++;
		delay = (1000000 / AVATARS_FADE_STEPS) * st->fade;
	} else {
		/* blank screen */
		XClearWindow(display, window);
		st->xpos = random()%(st->wa.width - st->width);
		st->ypos = random()%(st->wa.height - st->height);
		st->next_state = 0;
		delay = 1000000 * st->blank;
	}

	return delay;
}

static void
avatars_reshape(Display *display, Window window, void *closure, 
				unsigned int width, unsigned int height) {
	struct state *st = (struct state *) closure;

	st->wa.width = width;
	st->wa.height = height;
	st->next_state = 999;
}

static Bool
avatars_event(Display *display, Window window, void *closure,
				XEvent *event) {
	return False;
}

static void
avatars_free (Display *display, Window window, void *closure) {
	struct state *st = (struct state *) closure;
	int k;

	XDestroyImage(st->image);
		for (k = 0; k < AVATARS_FADE_STEPS; k++) {
		XDestroyImage(st->op[k]);
	}
	free(st);
}

static const char *avatars_defaults [] = {
	".background:		black",
	".foreground:		white",
	"*show:			6",
	"*fade:			1",
	"*blank:		2",
	"*image:		0",
	0
};

static XrmOptionDescRec avatars_options [] = {
	{ "-show",		".show",	XrmoptionSepArg, 0 },
	{ "-fade",		".fade",	XrmoptionSepArg, 0 },
	{ "-blank",		".blank",	XrmoptionSepArg, 0 },
	{ "-image",		".image",	XrmoptionSepArg, 0 },
	{ 0, 0, 0, 0 }
};

XSCREENSAVER_MODULE ("Avatars", avatars)
