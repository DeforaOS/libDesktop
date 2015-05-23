/* $Id$ */
/* Copyright (c) 2014-2015 Pierre Pronchery <khorben@defora.org> */
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
	Plugin * plugin;
	DesktopWidgetDefinition * definition;
	DesktopWidgetPlugin * dplugin;
};


/* public */
/* functions */
/* desktop_widget_new */
DesktopWidget * desktop_widget_new(char const * name)
{
	DesktopWidget * widget;

	if((widget = object_new(sizeof(*widget))) == NULL)
		return NULL;
	if((widget->plugin = plugin_new(LIBDIR, "Desktop", "widget", name))
			== NULL || (widget->definition = plugin_lookup(
					widget->plugin, "widget")) == NULL)
	{
		desktop_widget_delete(widget);
		return NULL;
	}
	return widget;
}


/* desktop_widget_delete */
void desktop_widget_delete(DesktopWidget * widget)
{
	if(widget->definition != NULL && widget->plugin != NULL)
		widget->definition->destroy(widget->plugin);
	if(widget->plugin != NULL)
		plugin_delete(widget->plugin);
	object_delete(widget);
}


/* accessors */
/* desktop_widget_get_widget */
GtkWidget * desktop_widget_get_widget(DesktopWidget * widget)
{
	return widget->definition->get_widget(widget->dplugin);
}
