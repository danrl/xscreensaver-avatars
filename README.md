# xscreensaver-avatars

avatars - a simple, cpu-friendly screensaver


## About

The Avatars program draws a selected image at a random position
on the screen. After a defined delay the image disappears to show
up at a new position.


## Author

Written by Dan Luedtke <mail@danrl.de>, based on code by Jamie
Zawinski <jwz@jwz.org>.


## Install

-  **Download xscreensaver** 

	Download xscreensaver sources by Jamie Zawinski from http://www.jwz.org/xscreensaver/ and extract the source files.

	    $ wget http://www.jwz.org/xscreensaver/xscreensaver-5.15.tar.gz
	    $ tar -xvzf xscreensaver-5.15.tar.gz


-  **Download xscreensaver-avatars** 

	Get the latest xscreensaver-avatars sources and merge them with your xscreensaver hacks folder.

	    $ git clone git://github.com/danrl/xscreensaver-avatars.git
	    $ cp -r xscreensaver-avatars/* xscreensaver-5.15/hacks/


-  **Edit ./hacks/Makefile.in** 
	-  add `avatars.c` to variable `SRCS`
	-  add `avatars.o` to variable `OBJS`
	-  add `avatars` to variable `EXES`
	-  add the following lines where the others build-targets are:

		    avatars: avatars.o $(HACK_OBJS) $(XPM)
		    	$(CC_HACK) -o $@ $@.o $(HACK_OBJS) $(XPM) $(XPM_LIBS) $(HACK_LIBS)


-  **Configure, compile, and install!** 

	    $ ./configure
	    $ make distdepend
	    $ make
	    # make install


## License

Copyright © 2011 by Dan Luedtke. Permission to use, copy, modify, dis‐
tribute, and sell this software and its documentation for any purpose
is hereby granted without fee, provided that the above copyright notice
appear in all copies and that both that copyright notice and this per‐
mission notice appear in supporting documentation. No representations
are made about the suitability of this software for any purpose. It is
provided "as is" without express or implied warranty.
