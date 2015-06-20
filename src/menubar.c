/* $Id$ */
/* Copyright (c) 2011 Pierre Pronchery <khorben@defora.org> */
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


/* Menubar */
/* desktop_menubar_create */
static GtkWidget * _menubar_create_menu(DesktopMenu const * menu, gpointer data,
		GtkAccelGroup * accel);
static GtkWidget * _menubar_create_menu_from_image(DesktopMenu const * menu);

GtkWidget * desktop_menubar_create(DesktopMenubar const * menubar,
		gpointer data, GtkAccelGroup * accel)
{
	GtkWidget * ret;
	GtkWidget * menuitem;
	GtkWidget * menu;
	size_t i;

	ret = gtk_menu_bar_new();
	for(i = 0; menubar[i].name != NULL; i++)
	{
		menuitem = gtk_menu_item_new_with_mnemonic(_(menubar[i].name));
		menu = _menubar_create_menu(menubar[i].menu, data, accel);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(ret), menuitem);
	}
	return ret;
}

static GtkWidget * _menubar_create_menu(DesktopMenu const * menu, gpointer data,
		GtkAccelGroup * accel)
{
	GtkWidget * ret;
	size_t i;
	DesktopMenu const * p;
	GtkWidget * menuitem;

	ret = gtk_menu_new();
	for(i = 0; menu[i].name != NULL; i++)
	{
		p = &menu[i];
		if(p->name[0] == '\0')
			menuitem = gtk_separator_menu_item_new();
		else if(p->stock == NULL)
			menuitem = gtk_menu_item_new_with_mnemonic(_(p->name));
#if !GTK_CHECK_VERSION(3, 10, 0)
		else if(strncmp(p->stock, "gtk-", 4) == 0)
			menuitem = gtk_image_menu_item_new_from_stock(p->stock,
					NULL);
#endif
		else
			menuitem = _menubar_create_menu_from_image(p);
		if(p->callback != NULL)
			g_signal_connect_swapped(G_OBJECT(menuitem), "activate",
					G_CALLBACK(p->callback), data);
		else
			gtk_widget_set_sensitive(menuitem, FALSE);
		if(accel != NULL && p->accel != 0)
			gtk_widget_add_accelerator(menuitem, "activate", accel,
					p->accel, p->modifier,
					GTK_ACCEL_VISIBLE);
		gtk_menu_shell_append(GTK_MENU_SHELL(ret), menuitem);
	}
	return ret;
}

static GtkWidget * _menubar_create_menu_from_image(DesktopMenu const * menu)
{
	GtkWidget * ret;
	GtkWidget * image;

	ret = gtk_image_menu_item_new_with_mnemonic(_(menu->name));
	image = gtk_image_new_from_icon_name(menu->stock, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(ret), image);
	return ret;
}
