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



#ifndef LIBDESKTOP_DESKTOP_WIDGET_H
# define LIBDESKTOP_DESKTOP_WIDGET_H

# include <stdarg.h>


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
	int (*set_property)(DesktopWidgetPlugin * widget, va_list ap);
} DesktopWidgetDefinition;


/* functions */
DesktopWidget * desktop_widget_new(char const * name);
void desktop_widget_delete(DesktopWidget * widget);

/* accessors */
GtkWidget * desktop_widget_get_widget(DesktopWidget * widget);

int desktop_widget_set_property(DesktopWidget * widget, ...);

#endif /* !LIBDESKTOP_DESKTOP_WIDGET_H */
