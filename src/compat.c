/* $Id$ */
/* Copyright (c) 2012-2016 Pierre Pronchery <khorben@defora.org> */
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
/* Gtk+ 3 */
#if GTK_CHECK_VERSION(3, 0, 0)
/* gdk_window_clear */
void gdk_window_clear(GdkWindow * window)
{
	Display * display;
	Window wid;

	display = gdk_x11_get_default_xdisplay();
	wid = gdk_x11_window_get_xid(window);
	gdk_error_trap_push();
	XClearWindow(display, wid);
	gdk_error_trap_pop();
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


/* gtk_button_box_new */
GtkWidget * gtk_button_box_new(GtkOrientation orientation)
{
	switch(orientation)
	{
		case GTK_ORIENTATION_HORIZONTAL:
			return gtk_hbutton_box_new();
		case GTK_ORIENTATION_VERTICAL:
		default:
			return gtk_vbutton_box_new();
	}
}


/* gtk_paned_new */
GtkWidget * gtk_paned_new(GtkOrientation orientation)
{
	switch(orientation)
	{
		case GTK_ORIENTATION_HORIZONTAL:
			return gtk_hpaned_new();
		case GTK_ORIENTATION_VERTICAL:
		default:
			return gtk_vpaned_new();
	}
}


/* gtk_scale_new */
GtkWidget * gtk_scale_new(GtkOrientation orientation,
		GtkAdjustment * adjustment)
{
	switch(orientation)
	{
		case GTK_ORIENTATION_HORIZONTAL:
			return gtk_hscale_new(adjustment);
		case GTK_ORIENTATION_VERTICAL:
		default:
			return gtk_vscale_new(adjustment);
	}
}


/* gtk_scale_new_with_range */
GtkWidget * gtk_scale_new_with_range(GtkOrientation orientation,
		gdouble min, gdouble max, gdouble step)
{
	switch(orientation)
	{
		case GTK_ORIENTATION_HORIZONTAL:
			return gtk_hscale_new_with_range(min, max, step);
		case GTK_ORIENTATION_VERTICAL:
		default:
			return gtk_vscale_new_with_range(min, max, step);
	}
}


/* gtk_separator_new */
GtkWidget * gtk_separator_new(GtkOrientation orientation)
{
	switch(orientation)
	{
		case GTK_ORIENTATION_HORIZONTAL:
			return gtk_hseparator_new();
		case GTK_ORIENTATION_VERTICAL:
		default:
			return gtk_vseparator_new();
	}
}


/* gtk_widget_override_font */
void gtk_widget_override_font(GtkWidget * widget,
		const PangoFontDescription * desc)
{
	gtk_widget_modify_font(widget, desc);
}
#endif
