/* $Id$ */
/* Copyright (c) 2012-2015 Pierre Pronchery <khorben@defora.org> */
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



#include <gdk/gdkx.h>
#include "Desktop.h"


/* Compat */
#if GTK_CHECK_VERSION(3, 0, 0)
/* gdk_window_clear */
void gdk_window_clear(GdkWindow * window)
{
	Display * display;
	Window wid;

	display = gdk_x11_get_default_xdisplay();
	wid = gdk_x11_window_get_xid(window);
	XClearWindow(display, wid);
}
#endif


#if !GTK_CHECK_VERSION(3, 0, 0)
/* gtk_box_new */
GtkWidget * gtk_box_new(GtkOrientation orientation, gint spacing)
{
	switch(orientation)
	{
		case GTK_ORIENTATION_HORIZONTAL:
			return gtk_hbox_new(FALSE, spacing);
		case GTK_ORIENTATION_VERTICAL:
		default:
			return gtk_vbox_new(FALSE, spacing);
	}
}
#endif
