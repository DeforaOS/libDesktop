/* $Id$ */
/* Copyright (c) 2010 Pierre Pronchery <khorben@defora.org> */
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



#include <string.h>
#include <libintl.h>
#include "Desktop.h"
#define _(string) gettext(string)


/* Toolbar */
/* desktop_toolbar_create */
GtkWidget * desktop_toolbar_create(DesktopToolbar * toolbar,
		gpointer data, GtkAccelGroup * accel)
{
	GtkWidget * ret;
	size_t i;
	DesktopToolbar * p;
	GtkWidget * widget;

	ret = gtk_toolbar_new();
	for(i = 0; toolbar[i].name != NULL; i++)
	{
		p = &toolbar[i];
		if(p->name[0] == '\0')
		{
			p->widget = gtk_separator_tool_item_new();
			gtk_toolbar_insert(GTK_TOOLBAR(ret), p->widget, -1);
			continue;
		}
#if !GTK_CHECK_VERSION(3, 10, 0)
		else if(strncmp(p->stock, "gtk-", 4) == 0) /* stock icon */
			p->widget = gtk_tool_button_new_from_stock(p->stock);
#endif
		else if(p->stock != NULL) /* icon name */
		{
			widget = gtk_image_new_from_icon_name(p->stock,
					GTK_ICON_SIZE_LARGE_TOOLBAR);
			p->widget = gtk_tool_button_new(widget, p->name);
		}
		else
			p->widget = gtk_tool_button_new(NULL, _(p->name));
		if(p->callback != NULL)
			g_signal_connect_swapped(G_OBJECT(p->widget), "clicked",
					G_CALLBACK(p->callback), data);
		else
			gtk_widget_set_sensitive(GTK_WIDGET(p->widget), FALSE);
		if(accel != NULL && p->accel != 0)
			gtk_widget_add_accelerator(GTK_WIDGET(p->widget),
					"clicked", accel, p->accel, p->modifier,
					GTK_ACCEL_VISIBLE);
#if 0 /* GTK_CHECK_VERSION(2, 12, 0) */
		/* XXX the default is returned instead of the configuration */
		if(gtk_toolbar_get_style(GTK_TOOLBAR(ret)) == GTK_TOOLBAR_ICONS)
			gtk_tool_item_set_tooltip_text(p->widget, p->name);
#endif
		gtk_toolbar_insert(GTK_TOOLBAR(ret), p->widget, -1);
	}
	return ret;
}
