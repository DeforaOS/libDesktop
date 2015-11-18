/* $Id$ */
/* Copyright (c) 2014-2015 Pierre Pronchery <khorben@defora.org> */
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
	widget->definition = NULL;
	widget->dplugin = NULL;
	if((widget->plugin = plugin_new(LIBDIR, "Desktop", "widget",
					name)) == NULL
			|| (widget->definition = plugin_lookup(widget->plugin,
					"widget")) == NULL
			|| widget->definition->init == NULL
			|| widget->definition->destroy == NULL
			|| widget->definition->get_widget == NULL
			|| (widget->dplugin = widget->definition->init(name))
			== NULL)
	{
		desktop_widget_delete(widget);
		return NULL;
	}
	return widget;
}


/* desktop_widget_delete */
void desktop_widget_delete(DesktopWidget * widget)
{
	if(widget->definition != NULL && widget->dplugin != NULL)
		widget->definition->destroy(widget->dplugin);
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


/* desktop_widget_set_property */
int desktop_widget_set_property(DesktopWidget * widget, ...)
{
	int ret;
	va_list ap;

	if(widget->definition->set_property == NULL)
		return -error_set_code(1, "%s: %s", widget->definition->name,
				"Properties not supported");
	va_start(ap, widget);
	ret = widget->definition->set_property(widget->dplugin, ap);
	va_end(ap);
	return ret;
}
