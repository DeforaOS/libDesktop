/* $Id$ */
/* Copyright (c) 2015 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Desktop libDesktop */
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */



#include <stdio.h>
#include <System.h>
#include "Desktop.h"

#define PROGNAME	"widget"


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
	int ret = 0;
	DesktopWidget * widget;
	GtkWidget * test;

	if((widget = desktop_widget_new(name)) == NULL)
		_error(name, 1);
	else
	{
		test = desktop_widget_get_widget(widget);
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
