/* $Id$ */
/* Copyright (c) 2011-2024 Pierre Pronchery <khorben@defora.org> */
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
/* TODO:
 * - proxy for the libApp framework instead */



#include <stdlib.h>
#ifdef DEBUG
# include <stdio.h>
#endif
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#if defined(GDK_WINDOWING_X11)
# if GTK_CHECK_VERSION(3, 0, 0)
#  include <gtk/gtkx.h>
# else
#  include <gdk/gdkx.h>
# endif
#else
# include <sys/socket.h>
# include <sys/un.h>
# include <unistd.h>
#endif
#include <System.h>
#include "Desktop.h"


/* Message */
/* private */
/* types */
typedef struct _MessageCallback
{
	GtkWidget * window;
#if defined(GDK_WINDOWING_X11)
# if GTK_CHECK_VERSION(3, 0, 0)
	Atom atom;
# else
	GtkWidget * widget;
	Window xwindow;
# endif
#else
	int socket;
	GIOChannel * channel;
	guint id;
#endif
	DesktopMessageCallback callback;
	void * data;
} MessageCallback;


/* variables */
static MessageCallback ** _callbacks = NULL;
static size_t _callbacks_cnt = 0;


/* prototypes */
/* callbacks */
#if defined(GDK_WINDOWING_X11)
static GdkFilterReturn _desktop_message_on_callback(GdkXEvent * xevent,
		GdkEvent * event, gpointer data);
#else
static gboolean _desktop_message_on_connect(GIOChannel * channel,
		GIOCondition condition, gpointer data);
#endif


/* public */
/* functions */
/* desktop_message_register */
int desktop_message_register(GtkWidget * window, char const * destination,
		DesktopMessageCallback callback, void * data)
{
	MessageCallback ** p;
	MessageCallback * mc;
#if defined(GDK_WINDOWING_X11)
	GdkWindow * gwindow;
# if !GTK_CHECK_VERSION(3, 0, 0)
	GdkAtom atom;
# endif
#else
	(void) window;
#endif

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%p, \"%s\", %p, %p)\n", __func__, window,
			destination, callback, data);
#endif
	if((p = realloc(_callbacks, sizeof(*p) * (_callbacks_cnt + 1))) == NULL)
		return -1;
	_callbacks = p;
	if((mc = object_new(sizeof(*mc))) == NULL)
		return -1;
	_callbacks[_callbacks_cnt++] = mc;
	mc->callback = callback;
	mc->data = data;
#if defined(GDK_WINDOWING_X11)
# if GTK_CHECK_VERSION(3, 0, 0)
	mc->atom = XInternAtom(gdk_x11_get_default_xdisplay(), destination,
			FALSE);
	gwindow = (window != NULL) ? gtk_widget_get_window(window) : NULL;
	gdk_window_add_filter(gwindow, _desktop_message_on_callback, mc);
# else
	if((mc->window = window) == NULL)
	{
		mc->widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_widget_realize(mc->widget);
	}
	else
		mc->widget = window;
	gwindow = gtk_widget_get_window(mc->widget);
	mc->xwindow = GDK_WINDOW_XWINDOW(gwindow);
	atom = gdk_atom_intern(destination, FALSE);
	gdk_add_client_message_filter(atom, _desktop_message_on_callback, mc);
# endif
#else
	struct sockaddr_un addr;

	if((p = realloc(_callbacks, sizeof(*p) * (_callbacks_cnt + 1))) == NULL)
		return -error_set_code(1, "%s", strerror(errno));
	_callbacks = p;
	if((mc = object_new(sizeof(*mc))) == NULL)
		return -1;
	_callbacks[_callbacks_cnt] = mc;
	mc->callback = callback;
	mc->data = data;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/%s-%s",
			g_get_tmp_dir(), gdk_get_display(), destination);
	addr.sun_len = sizeof(addr) - sizeof(addr.sun_path)
		+ strlen(addr.sun_path) + 1;
	if((mc->socket = socket(addr.sun_family, SOCK_STREAM, 0)) < 0)
		return -error_set_code(1, "%s: %s: %s", "socket",
				addr.sun_path);
	if(bind(mc->socket, (struct sockaddr *)&addr, sizeof(addr)) != 0)
	{
		error_set_code(1, "%s: %s: %s", "bind", addr.sun_path,
				strerror(errno));
		close(mc->socket);
		unlink(addr.sun_path);
		return -1;
	}
	if(listen(mc->socket, 5) != 0)
	{
		error_set_code(1, "%s: %s: %s", "listen", addr.sun_path,
				strerror(errno));
		close(mc->socket);
		unlink(addr.sun_path);
		return -1;
	}
	mc->channel = g_io_channel_unix_new(mc->socket);
	g_io_channel_set_encoding(mc->channel, NULL, NULL);
	mc->id = g_io_add_watch(mc->channel, G_IO_IN,
			_desktop_message_on_connect, NULL);
	_callbacks_cnt++;
#endif
	return 0;
}


/* desktop_message_send */
int desktop_message_send(char const * destination, uint32_t value1,
		uint32_t value2, uint32_t value3)
{
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%s, 0x%x, 0x%x, 0x%x)\n", __func__,
			destination, value1, value2, value3);
#endif
#if defined(GDK_WINDOWING_X11)
# if GTK_CHECK_VERSION(3, 0, 0)
	GdkDisplay * display;
	Display * xdisplay;
	XEvent xev;
	XClientMessageEvent * xcme = &xev.xclient;

	display = gdk_display_get_default();
	xdisplay = gdk_x11_display_get_xdisplay(display);
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xcme->serial = 0;
	xcme->send_event = True;
	xcme->message_type = XInternAtom(xdisplay, destination, FALSE);
	xcme->format = 32;
	xcme->data.l[0] = value1;
	xcme->data.l[1] = value2;
	xcme->data.l[2] = value3;
	gdk_x11_display_error_trap_push(display);
	XSendEvent(xdisplay, gdk_x11_get_default_root_xwindow(), False,
			StructureNotifyMask | /* XXX check the mask */
			SubstructureNotifyMask | SubstructureRedirectMask,
			&xev);
	return (gdk_x11_display_error_trap_pop(display) == 0) ? 0 : -1;
# else
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
	return 0;
# endif
#else
	int fd;
	struct sockaddr_un addr;
	char buf[33];

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/%s-%s",
			g_get_tmp_dir(), gdk_get_display(), destination);
	addr.sun_len = sizeof(addr) - sizeof(addr.sun_path)
		+ strlen(addr.sun_path) + 1;
	if(access(addr.sun_path, W_OK) == 0)
		return -error_set_code(1, "%s: %s", addr.sun_path,
				strerror(errno));
	if((fd = socket(addr.sun_family, SOCK_STREAM, 0)) < 0)
		return -error_set_code(1, "%s: %s: %s", "socket", addr.sun_path,
				strerror(errno));
	if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
	{
		close(fd);
		return -error_set_code(1, "%s: %s: %s", "connect",
				addr.sun_path, strerror(errno));
	}
	snprintf(buf, sizeof(buf), "0x%x:0x%x:0x%x", value1, value2, value3);
	if(send(fd, buf, strlen(buf), 0) != (ssize_t)strlen(buf))
	{
		close(fd);
		return -error_set_code(1, "%s: %s: %s", "send", addr.sun_path,
				strerror(errno));
	}
	close(fd);
	return 0;
#endif
}


/* desktop_message_unregister */
void desktop_message_unregister(GtkWidget * window,
		DesktopMessageCallback callback, void * data)
{
	size_t i;
	MessageCallback ** p;
	MessageCallback * mc;
#if defined(GDK_WINDOWING_X11)
	GdkWindow * w;
#endif

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%p, %p)\n", __func__, callback, data);
#endif
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
#if defined(GDK_WINDOWING_X11)
# if GTK_CHECK_VERSION(3, 0, 0)
	w = (window != NULL) ? gtk_widget_get_window(window) : NULL;
# else
	w = gtk_widget_get_window(mc->widget);
# endif
	gdk_window_remove_filter(w, _desktop_message_on_callback, mc);
# if !GTK_CHECK_VERSION(3, 0, 0)
	if(mc->window == NULL)
		gtk_widget_destroy(mc->widget);
# endif
#else
	if(mc->id > 0)
		g_source_remove(mc->id);
	if(mc->channel != NULL)
		g_io_channel_unref(mc->channel);
	if(mc->socket >= 0)
		close(mc->socket);
#endif
	object_delete(mc);
	p = &_callbacks[i];
	memmove(p, p + 1, sizeof(*p) * (_callbacks_cnt - i - 1));
	if((p = realloc(_callbacks, sizeof(*p) * (--_callbacks_cnt))) != NULL
			|| _callbacks_cnt == 0)
		_callbacks = p;
}


/* private */
/* callbacks */
#if defined(GDK_WINDOWING_X11)
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
	(void) event;

	if(xev->type != ClientMessage)
		return GDK_FILTER_CONTINUE;
	xcme = &xev->xclient;
# if GTK_CHECK_VERSION(3, 0, 0)
#  ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%lu %lu %p)\n", __func__, xcme->serial,
			xcme->window, (void *)mc);
#  endif
	if(mc->atom != xcme->message_type)
		return GDK_FILTER_CONTINUE;
# else
#  ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%lu %lu %p) %lu\n", __func__, xcme->serial,
			xcme->window, mc, (void *)mc->xwindow);
#  endif
	if(mc->xwindow != xcme->window)
		return GDK_FILTER_CONTINUE;
# endif
# ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() %p(%p)\n", __func__, (void *)mc->callback,
			(void *)mc->data);
# endif
	value1 = xcme->data.l[0];
	value2 = xcme->data.l[1];
	value3 = xcme->data.l[2];
	if(mc->callback(mc->data, value1, value2, value3) == 0)
		return GDK_FILTER_CONTINUE;
	desktop_message_unregister(mc->window, mc->callback, mc->data);
	return GDK_FILTER_REMOVE;
}
#else
static gboolean _desktop_message_on_connect(GIOChannel * channel,
		GIOCondition condition, gpointer data)
{
	size_t i;
	MessageCallback * mc;
	int fd;
	char buf[33];
	ssize_t len;
	uint32_t value1;
	uint32_t value2;
	uint32_t value3;
	(void) data;

# ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
# endif
	if(condition != G_IO_IN)
		return FALSE;
	for(i = 0; i < _callbacks_cnt; i++)
		if(_callbacks[i]->channel == channel)
			break;
	if(i == _callbacks_cnt)
		return FALSE;
	mc = _callbacks[i];
	fd = accept(mc->socket, NULL, NULL);
	len = recv(fd, buf, sizeof(buf) - 1, 0);
	close(fd);
	if(len > 0 && (size_t)len < sizeof(buf))
	{
		buf[len] = '\0';
		if(sscanf(buf, "0x%x:0x%x:0x%x", &value1, &value2, &value3) == 3
				&& mc->callback(mc->data, value1, value2,
					value3) != 0)
			desktop_message_unregister(mc->window, mc->callback,
					mc->data);
	}
	return TRUE;
}
#endif
