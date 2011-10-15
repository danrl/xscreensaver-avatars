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
#include "images/avatars_getadrink.xpm"

struct state {
	int delay;
	XWindowAttributes wa;
	GC gc;
	XImage *image;
};

static void *
avatars_init(Display *display, Window window) {
	struct state *st = (struct state *) calloc (1, sizeof(*st));
	int selected_image = get_integer_resource(display, "image", "Integer");
	st->delay = get_integer_resource(display, "delay", "Integer");
	XGetWindowAttributes(display, window, &st->wa);
	st->gc = XCreateGC(display, window, 0, NULL);
	if(selected_image==1) {
		XpmCreateImageFromData(display, avatars_danrl_xpm, &st->image, NULL, NULL);
	}
	else if(selected_image==2) {
		XpmCreateImageFromData(display, avatars_getadrink_xpm, &st->image, NULL, NULL);
	}
	else {
		XpmCreateImageFromData(display, avatars_penguin_xpm, &st->image, NULL, NULL);
	}

	return st;
}

static unsigned long
avatars_draw (Display *display, Window window, void *closure) {
	struct state *st = (struct state *) closure;

	XClearWindow(display, window);
	XPutImage(display, window, st->gc, st->image, 0, 0,
		random()%(st->wa.width - (st->image)->width),
		random()%(st->wa.height - (st->image)->height),
		(st->image)->width,
		(st->image)->height);
	return 1000000 * st->delay;
}

static void
avatars_reshape(Display *display, Window window, void *closure, 
				unsigned int width, unsigned int height) {
	struct state *st = (struct state *) closure;

	st->wa.width = width;
	st->wa.height = height;
}

static Bool
avatars_event(Display *display, Window window, void *closure,
				XEvent *event) {
	return False;
}

static void
avatars_free (Display *display, Window window, void *closure) {
	struct state *st = (struct state *) closure;
	free(st);
}

static const char *avatars_defaults [] = {
	".background:		black",
	".foreground:		white",
	"*delay:			6",
	"*image:			0",
	0
};

static XrmOptionDescRec avatars_options [] = {
	{ "-delay",			".delay",	XrmoptionSepArg, 0 },
	{ "-image",			".image",	XrmoptionSepArg, 0 },
	{ 0, 0, 0, 0 }
};

XSCREENSAVER_MODULE ("Avatars", avatars)
