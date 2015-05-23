/* $Id$ */
/* Copyright (c) 2014 Pierre Pronchery <khorben@defora.org> */
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



#include <System.h>
#include "Desktop.h"
#include "../config.h"

#ifndef PREFIX
# define PREFIX		"/usr/local"
#endif
#ifndef LIBDIR
# define LIBDIR		PREFIX "/lib"
#endif


/* Widget */
/* private */
/* types */
struct _DesktopWidget
{
	GtkWidget * widget;
};


/* public */
/* functions */
/* desktop_widget_new */
DesktopWidget * desktop_widget_new(char const * name)
{
	DesktopWidget * ret;

	if((ret = object_new(sizeof(*ret))) == NULL)
		return NULL;
	/* FIXME really implement */
	ret->widget = gtk_label_new(name);
	return ret;
}


/* desktop_widget_delete */
void desktop_widget_delete(DesktopWidget * widget)
{
	gtk_widget_destroy(widget->widget);
	object_delete(widget);
}


/* accessors */
/* desktop_widget_get_widget */
GtkWidget * desktop_widget_get_widget(DesktopWidget * widget)
{
	/* FIXME really implement */
	return widget->widget;
}
