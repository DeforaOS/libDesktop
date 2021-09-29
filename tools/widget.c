/* $Id$ */
/* Copyright (c) 2015-2020 Pierre Pronchery <khorben@defora.org> */
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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <System.h>
#include <gtk/gtk.h>
#include "Desktop.h"

#define PROGNAME_WIDGET	"widget"


/* Widget */
/* private */
/* types */
typedef struct _WidgetPrefs
{
	String const * title;
	int width;
	int height;
} WidgetPrefs;


/* prototypes */
static int _widget(WidgetPrefs * prefs, int namec, char ** namev);

static int _error(char const * message, int ret);
static int _usage(char const * error);


/* functions */
/* widget */
static gboolean _widget_on_closex(GtkWidget * widget);

static int _widget(WidgetPrefs * prefs, int namec, char ** namev)
{
	int ret = 0;
	DesktopWidget * widget;
	GtkWidget * window;
	GtkWidget * box;
	GtkWidget * test;
	int i;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	if(prefs != NULL)
	{
		if(prefs->title != NULL)
			gtk_window_set_title(GTK_WINDOW(window), prefs->title);
		if(prefs->width > 0 || prefs->height > 0)
			gtk_window_set_default_size(GTK_WINDOW(window),
					(prefs->width != 0) ? prefs->width : -1,
					(prefs->height != 0) ? prefs->height : -1);
	}
	g_signal_connect(window, "delete-event", G_CALLBACK(_widget_on_closex),
			NULL);
#if GTK_CHECK_VERSION(3, 0, 0)
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
#else
	box = gtk_hbox_new(FALSE, 4);
#endif
	for(i = 0; i < namec; i++)
	{
		if((widget = desktop_widget_new(namev[i])) == NULL)
		{
			ret |= _error(namev[i], 1);
			continue;
		}
		test = desktop_widget_get_widget(widget);
		gtk_box_pack_start(GTK_BOX(box), test, TRUE, TRUE, 0);
	}
	gtk_container_add(GTK_CONTAINER(window), box);
	gtk_widget_show_all(window);
	gtk_main();
	return ret;
}

static gboolean _widget_on_closex(GtkWidget * widget)
{
	gtk_widget_destroy(widget);
	gtk_main_quit();
	return TRUE;
}


/* error */
static int _error(char const * message, int ret)
{
	fprintf(stderr, PROGNAME_WIDGET ": %s%s%s\n",
			(message != NULL) ? message : "",
			(message != NULL) ? ": " : "", error_get(NULL));
	return ret;
}


/* usage */
static int _usage(char const * error)
{
	if(error != NULL)
		fprintf(stderr, "%s: %s\n", PROGNAME_WIDGET, error);
	fputs("Usage: " PROGNAME_WIDGET " [-h height][-t title][-w width]"
			" widget...\n", stderr);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int o;
	WidgetPrefs prefs;
	char * p;

	memset(&prefs, 0, sizeof(prefs));
#if GTK_CHECK_VERSION(4, 0, 0)
	gtk_init(&argc, &argv);
#else
	gtk_init(&argc, &argv);
#endif
	while((o = getopt(argc, argv, "h:t:w:")) != -1)
		switch(o)
		{
			case 'h':
				errno = 0;
				prefs.height = strtol(optarg, &p, 0);
				if(optarg[0] == '\0' || *p != '\0')
					return _usage(NULL);
				else if(errno != 0)
					return _usage(strerror(errno));
				else if(prefs.height < 0)
					return _usage("width must be strictly"
							" positive");
				break;
			case 't':
				prefs.title = optarg;
				break;
			case 'w':
				errno = 0;
				prefs.width = strtol(optarg, &p, 0);
				if(optarg[0] == '\0' || *p != '\0')
					return _usage(NULL);
				else if(errno != 0)
					return _usage(strerror(errno));
				else if(prefs.width < 0)
					return _usage("height must be strictly"
							" positive");
				break;
			default:
				return _usage(NULL);
		}
	if(optind == argc)
		return _usage(NULL);
	return (_widget(&prefs, argc - optind, &argv[optind]) == 0) ? 0 : 2;
}
