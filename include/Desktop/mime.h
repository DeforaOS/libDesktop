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



#ifndef LIBDESKTOP_DESKTOP_MIME_H
# define LIBDESKTOP_DESKTOP_MIME_H

# include <gtk/gtk.h>
# include "mimehandler.h"


/* Mime */
/* types */
typedef struct _Mime Mime;

typedef void (*MimeForeachCallback)(void * data, char const * name,
		GdkPixbuf * icon_24, GdkPixbuf * icon_48, GdkPixbuf * icon_96);


/* functions */
Mime * mime_new(GtkIconTheme * theme);
void mime_delete(Mime * mime);

/* accessors */
MimeHandler * mime_get_handler(Mime * mime, char const * type,
		char const * action);
int mime_set_handler(Mime * mime, char const * type, char const * action,
		char const * handler);
void mime_set_theme(Mime * mime, GtkIconTheme * theme);

/* useful */
char const * mime_type(Mime * mime, char const * path);
int mime_action(Mime * mime, char const * action, char const * path);
int mime_action_type(Mime * mime, char const * action, char const * path,
		char const * type);

void mime_foreach(Mime * mime, MimeForeachCallback callback, void * data);

void mime_icons(Mime * mime, char const * type, ...);

int mime_save(Mime * mime);

#endif /* !LIBDESKTOP_DESKTOP_MIME_H */
