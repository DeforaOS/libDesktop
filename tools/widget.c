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
#include <gtk/gtk.h>
#include "Desktop.h"

#define PROGNAME	"widget"


/* Widget */
/* private */
/* prototypes */
static int _widget(int namec, char ** namev);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* widget */
static gboolean _widget_on_closex(gpointer data);

static int _widget(int namec, char ** namev)
{
	int ret = 0;
	DesktopWidget * widget;
	GtkWidget * window;
	GtkWidget * box;
	GtkWidget * test;
	int i;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
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
	return (_widget(argc - optind, &argv[optind]) == 0) ? 0 : 2;
}
