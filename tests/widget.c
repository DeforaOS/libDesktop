/* $Id$ */
/* Copyright (c) 2015 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Desktop libDesktop */
/* All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */



#include <stdio.h>
#include <System.h>
#include "Desktop.h"

#ifndef PROGNAME
# define PROGNAME	"widget"
#endif


/* Widget */
/* private */
/* prototypes */
static int _widget(char const * name);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* widget */
static int _widget(char const * name)
{
	int ret;
	DesktopWidget * widget;
	GtkWidget * test;

	if((widget = desktop_widget_new(name)) == NULL)
		ret = -_error(name, 1);
	else
	{
		test = desktop_widget_get_widget(widget);
		ret = desktop_widget_set_property(widget, NULL);
		desktop_widget_delete(widget);
	}
	return ret;
}


/* error */
static int _error(char const * message, int ret)
{
	fprintf(stderr, PROGNAME ": %s%s%s\n", (message != NULL) ? message : "",
			(message != NULL) ? ": " : "", error_get());
	return ret;
}


/* usage */
static int _usage(void)
{
	fputs("Usage: " PROGNAME " widget...\n", stderr);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int ret = 0;
	int o;

	gtk_init(&argc, &argv);
	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			default:
				return _usage();
		}
	if(optind == argc)
		return _usage();
	for(o = optind; o < argc; o++)
		ret |= _widget(argv[o]);
	return ret;
}
