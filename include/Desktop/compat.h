/* $Id$ */
/* Copyright (c) 2011-2018 Pierre Pronchery <khorben@defora.org> */
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



#ifndef LIBDESKTOP_DESKTOP_COMPAT_H
# define LIBDESKTOP_DESKTOP_COMPAT_H


/* Compatibility */
/* constants */
# if !GTK_CHECK_VERSION(2, 22, 0)
#  define GDK_KEY_0		GDK_0
#  define GDK_KEY_1		GDK_1
#  define GDK_KEY_2		GDK_2
#  define GDK_KEY_3		GDK_3
#  define GDK_KEY_4		GDK_4
#  define GDK_KEY_5		GDK_5
#  define GDK_KEY_6		GDK_6
#  define GDK_KEY_7		GDK_7
#  define GDK_KEY_8		GDK_8
#  define GDK_KEY_9		GDK_9
#  define GDK_KEY_A		GDK_A
#  define GDK_KEY_B		GDK_B
#  define GDK_KEY_C		GDK_C
#  define GDK_KEY_D		GDK_D
#  define GDK_KEY_E		GDK_E
#  define GDK_KEY_F		GDK_F
#  define GDK_KEY_G		GDK_G
#  define GDK_KEY_H		GDK_H
#  define GDK_KEY_I		GDK_I
#  define GDK_KEY_J		GDK_J
#  define GDK_KEY_K		GDK_K
#  define GDK_KEY_L		GDK_L
#  define GDK_KEY_M		GDK_M
#  define GDK_KEY_N		GDK_N
#  define GDK_KEY_O		GDK_O
#  define GDK_KEY_P		GDK_P
#  define GDK_KEY_Q		GDK_Q
#  define GDK_KEY_R		GDK_R
#  define GDK_KEY_S		GDK_S
#  define GDK_KEY_T		GDK_T
#  define GDK_KEY_U		GDK_U
#  define GDK_KEY_V		GDK_V
#  define GDK_KEY_W		GDK_W
#  define GDK_KEY_X		GDK_X
#  define GDK_KEY_Y		GDK_Y
#  define GDK_KEY_Z		GDK_Z
#  define GDK_KEY_Alt_L		GDK_Alt_L
#  define GDK_KEY_Alt_R		GDK_Alt_R
#  define GDK_KEY_asterisk	GDK_asterisk
#  define GDK_KEY_Back		GDK_Back
#  define GDK_KEY_BackSpace	GDK_BackSpace
#  define GDK_KEY_Control_L	GDK_Control_L
#  define GDK_KEY_Control_R	GDK_Control_R
#  define GDK_KEY_Delete	GDK_Delete
#  define GDK_KEY_downarrow	GDK_downarrow
#  define GDK_KEY_Escape	GDK_Escape
#  define GDK_KEY_F1		GDK_F1
#  define GDK_KEY_F2		GDK_F2
#  define GDK_KEY_F3		GDK_F3
#  define GDK_KEY_F4		GDK_F4
#  define GDK_KEY_F5		GDK_F5
#  define GDK_KEY_F6		GDK_F6
#  define GDK_KEY_F7		GDK_F7
#  define GDK_KEY_F8		GDK_F8
#  define GDK_KEY_F9		GDK_F9
#  define GDK_KEY_F10		GDK_F10
#  define GDK_KEY_F11		GDK_F11
#  define GDK_KEY_F12		GDK_F12
#  define GDK_KEY_Forward	GDK_Forward
#  define GDK_KEY_Home		GDK_Home
#  define GDK_KEY_Left		GDK_Left
#  define GDK_KEY_minus		GDK_minus
#  define GDK_KEY_Page_Down	GDK_Page_Down
#  define GDK_KEY_Page_Up	GDK_Page_Up
#  define GDK_KEY_plus		GDK_plus
#  define GDK_KEY_Return	GDK_Return
#  define GDK_KEY_Right		GDK_Right
#  define GDK_KEY_slash		GDK_slash
#  define GDK_KEY_Up		GDK_Up
#  define GDK_KEY_uparrow	GDK_uparrow
# endif

/* Gtk+ 3.0 */
# if !GTK_CHECK_VERSION(3, 0, 0)
/* types */
typedef struct _GdkRGBA
{
	gdouble red;
	gdouble green;
	gdouble blue;
	gdouble alpha;
} GdkRGBA;

typedef enum _GtkStateFlags
{
	GTK_STATE_FLAG_NORMAL		= 0x00,
	GTK_STATE_FLAG_ACTIVE		= 0x01,
	GTK_STATE_FLAG_PRELIGHT		= 0x02,
	GTK_STATE_FLAG_SELECTED		= 0x04,
	GTK_STATE_FLAG_INSENSITIVE	= 0x08
} GtkStateFlags;


/* functions */
void gdk_window_clear(GdkWindow * window);

GtkWidget * gtk_box_new(GtkOrientation orientation, gint spacing);

GtkWidget * gtk_button_box_new(GtkOrientation orientation);

#  if !GTK_CHECK_VERSION(2, 14, 0)
GtkWidget * gtk_dialog_get_content_area(GtkDialog * dialog);
#  endif

GtkWidget * gtk_paned_new(GtkOrientation orientation);

GtkWidget * gtk_scale_new(GtkOrientation orientation,
		GtkAdjustment * adjustment);
GtkWidget * gtk_scale_new_with_range(GtkOrientation orientation,
		gdouble min, gdouble max, gdouble step);

GtkWidget * gtk_separator_new(GtkOrientation orientation);

#  if !GTK_CHECK_VERSION(2, 18, 0)
GtkAllocation gtk_widget_get_allocation(GtkWidget * widget);
#  endif

#  if !GTK_CHECK_VERSION(2, 14, 0)
GdkWindow * gtk_widget_get_window(GtkWidget * widget);
#  endif

#  if !GTK_CHECK_VERSION(2, 12, 0)
void gtk_widget_set_tooltip_text(GtkWidget * widget, const gchar * text);
#  endif

void gtk_widget_override_color(GtkWidget * widget, GtkStateFlags state,
		const GdkRGBA * color);
void gtk_widget_override_font(GtkWidget * widget,
		const PangoFontDescription * desc);
# endif

#endif /* !LIBDESKTOP_DESKTOP_COMPAT_H */
