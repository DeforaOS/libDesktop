/* $Id$ */
/* Copyright (c) 2011-2013 Pierre Pronchery <khorben@defora.org> */
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
/* TODO:
 * - proxy for the libApp framework instead */



#include <stdlib.h>
#ifdef DEBUG
# include <stdio.h>
#endif
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#if !GTK_CHECK_VERSION(3, 0, 0)
# include <gdk/gdkx.h>
#endif
#include <System.h>
#include "Desktop.h"


/* Message */
#if !GTK_CHECK_VERSION(3, 0, 0)
/* private */
/* types */
typedef struct _MessageCallback
{
	GtkWidget * window;
	GtkWidget * widget;
	Window xwindow;
	DesktopMessageCallback callback;
	void * data;
} MessageCallback;


/* variables */
static MessageCallback ** _callbacks = NULL;
static size_t _callbacks_cnt = 0;
#endif


/* prototypes */
/* callbacks */
#if !GTK_CHECK_VERSION(3, 0, 0)
static GdkFilterReturn _desktop_message_on_callback(GdkXEvent * xevent,
		GdkEvent * event, gpointer data);
#endif


/* public */
/* functions */
/* desktop_message_register */
int desktop_message_register(GtkWidget * window, char const * destination,
		DesktopMessageCallback callback, void * data)
{
#if !GTK_CHECK_VERSION(3, 0, 0)
	MessageCallback ** p;
	MessageCallback * mc;
	GdkWindow * gwindow;
	GdkAtom atom;

# ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%p, \"%s\", %p, %p)\n", __func__, window,
			destination, callback, data);
# endif
	if((p = realloc(_callbacks, sizeof(*p) * (_callbacks_cnt + 1))) == NULL)
		return -1;
	_callbacks = p;
	if((mc = object_new(sizeof(*mc))) == NULL)
		return -1;
	_callbacks[_callbacks_cnt++] = mc;
	if((mc->window = window) == NULL)
	{
		mc->widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_widget_realize(mc->widget);
	}
	else
		mc->widget = window;
	gwindow = gtk_widget_get_window(mc->widget);
	mc->xwindow = GDK_WINDOW_XWINDOW(gwindow);
	mc->callback = callback;
	mc->data = data;
	atom = gdk_atom_intern(destination, FALSE);
	gdk_add_client_message_filter(atom, _desktop_message_on_callback, mc);
#endif
	return 0;
}


/* desktop_message_send */
int desktop_message_send(char const * destination, uint32_t value1,
		uint32_t value2, uint32_t value3)
{
#if !GTK_CHECK_VERSION(3, 0, 0)
	GdkAtom atom;
	GdkEvent event;
	GdkEventClient * client = &event.client;

	atom = gdk_atom_intern(destination, FALSE);
	memset(&event, 0, sizeof(event));
	client->type = GDK_CLIENT_EVENT;
	client->window = NULL;
	client->send_event = TRUE;
	client->message_type = atom;
	client->data_format = 32;
	client->data.l[0] = value1;
	client->data.l[1] = value2;
	client->data.l[2] = value3;
	gdk_event_send_clientmessage_toall(&event);
#endif
	return 0;
}


/* desktop_message_unregister */
void desktop_message_unregister(GtkWidget * window,
		DesktopMessageCallback callback, void * data)
{
#if !GTK_CHECK_VERSION(3, 0, 0)
	size_t i;
	MessageCallback ** p;
	MessageCallback * mc;
	GdkWindow * w;

# ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%p, %p)\n", __func__, callback, data);
# endif
	for(i = 0; i < _callbacks_cnt; i++)
	{
		mc = _callbacks[i];
		if(mc->window == window
				&& mc->callback == callback
				&& mc->data == data)
			break;
	}
	if(i == _callbacks_cnt)
		return;
	w = gtk_widget_get_window(mc->widget);
	gdk_window_remove_filter(w, _desktop_message_on_callback, mc);
	if(mc->window == NULL)
		gtk_widget_destroy(mc->widget);
	object_delete(mc);
	p = &_callbacks[i];
	memmove(p, p + 1, sizeof(*p) * (_callbacks_cnt - i - 1));
	if((p = realloc(_callbacks, sizeof(*p) * (--_callbacks_cnt))) != NULL
			|| _callbacks_cnt == 0)
		_callbacks = p;
#endif
}


/* private */
/* callbacks */
#if !GTK_CHECK_VERSION(3, 0, 0)
/* desktop_message_on_callback */
static GdkFilterReturn _desktop_message_on_callback(GdkXEvent * xevent,
		GdkEvent * event, gpointer data)
{
	MessageCallback * mc = data;
	XEvent * xev = xevent;
	XClientMessageEvent * xcme;
	uint32_t value1;
	uint32_t value2;
	uint32_t value3;

	if(xev->type != ClientMessage)
		return GDK_FILTER_CONTINUE;
	xcme = &xev->xclient;
# ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%lu %lu %p) %lu\n", __func__, xcme->serial,
			xcme->window, mc, mc->xwindow);
# endif
	if(mc->xwindow != xcme->window)
		return GDK_FILTER_CONTINUE;
# ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() %p(%p)\n", __func__, mc->callback,
			mc->data);
# endif
	value1 = xcme->data.l[0];
	value2 = xcme->data.l[1];
	value3 = xcme->data.l[2];
	if(mc->callback(mc->data, value1, value2, value3) == 0)
		return GDK_FILTER_CONTINUE;
	desktop_message_unregister(mc->window, mc->callback, mc->data);
	return GDK_FILTER_REMOVE;
}
#endif
