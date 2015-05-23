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



#ifndef LIBDESKTOP_DESKTOP_WIDGET_H
# define LIBDESKTOP_DESKTOP_WIDGET_H


/* Widget */
/* public */
/* types */
typedef struct _DesktopWidget DesktopWidget;

typedef struct _DesktopWidgetPlugin DesktopWidgetPlugin;

typedef struct _DesktopWidgetDefinition
{
	char const * name;
	char const * icon;
	char const * description;
	DesktopWidgetPlugin * (*init)(char const * name);
	void (*destroy)(DesktopWidgetPlugin * widget);
	GtkWidget * (*get_widget)(DesktopWidgetPlugin * widget);
} DesktopWidgetDefinition;


/* functions */
DesktopWidget * desktop_widget_new(char const * name);
void desktop_widget_delete(DesktopWidget * widget);

/* accessors */
GtkWidget * desktop_widget_get_widget(DesktopWidget * widget);

#endif /* !LIBDESKTOP_DESKTOP_WIDGET_H */
