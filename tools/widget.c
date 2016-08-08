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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <System.h>
#include <gtk/gtk.h>
#include "Desktop.h"

#define PROGNAME	"widget"


/* Widget */
/* private */
/* types */
typedef struct _WidgetPrefs
{
	String const * title;
	unsigned int width;
	unsigned int height;
} WidgetPrefs;


/* prototypes */
static int _widget(WidgetPrefs * prefs, int namec, char ** namev);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* widget */
static gboolean _widget_on_closex(gpointer data);

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
					(prefs->height != 0)
					? prefs->height : -1);;
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
	gtk_widget_destroy(window);
	return ret;
}

static gboolean _widget_on_closex(gpointer data)
{
	gtk_main_quit();
	return TRUE;
}


/* error */
static int _error(char const * message, int ret)
{
	fprintf(stderr, PROGNAME ": %s%s%s\n", (message != NULL) ? message : "",
			(message != NULL) ? ": " : "", error_get(NULL));
	return ret;
}


/* usage */
static int _usage(void)
{
	fputs("Usage: " PROGNAME " [-h height][-t title][-w width] widget...\n",
			stderr);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int o;
	WidgetPrefs prefs;
	char * p;

	memset(&prefs, 0, sizeof(prefs));
	gtk_init(&argc, &argv);
	while((o = getopt(argc, argv, "h:t:w:")) != -1)
		switch(o)
		{
			case 'h':
				prefs.height = strtoul(optarg, &p, 0);
				if(optarg[0] == '\0' || *p != '\0')
					return _usage();
				break;
			case 't':
				prefs.title = optarg;
				break;
			case 'w':
				prefs.width = strtoul(optarg, &p, 0);
				if(optarg[0] == '\0' || *p != '\0')
					return _usage();
				break;
			default:
				return _usage();
		}
	if(optind == argc)
		return _usage();
	return (_widget(&prefs, argc - optind, &argv[optind]) == 0) ? 0 : 2;
}
