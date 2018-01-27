/* $Id$ */
/* Copyright (c) 2017 Pierre Pronchery <khorben@defora.org> */
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



#ifndef LIBDESKTOP_DESKTOP_MIMEHANDLER_H
# define LIBDESKTOP_DESKTOP_MIMEHANDLER_H

# include <System/config.h>
# include <System/string.h>


/* MimeHandler */
/* types */
typedef struct _MimeHandler MimeHandler;

typedef enum _MimeHandlerType
{
	MIMEHANDLER_TYPE_UNKNOWN = 0,
	MIMEHANDLER_TYPE_APPLICATION,
	MIMEHANDLER_TYPE_LINK,
	MIMEHANDLER_TYPE_DIRECTORY
} MimeHandlerType;


/* functions */
MimeHandler * mimehandler_new(void);
MimeHandler * mimehandler_new_load(String const * filename);
MimeHandler * mimehandler_new_load_by_name(String const * name);
void mimehandler_delete(MimeHandler * handler);

/* accessors */
int mimehandler_can_display(MimeHandler * handler);
int mimehandler_can_execute(MimeHandler * handler);
int mimehandler_can_open(MimeHandler * handler);

String const ** mimehandler_get_categories(MimeHandler * handler);
String const * mimehandler_get_comment(MimeHandler * handler, int translate);
String const * mimehandler_get_environment(MimeHandler * handler);
String const * mimehandler_get_filename(MimeHandler * handler);
String const * mimehandler_get_generic_name(MimeHandler * handler,
		int translate);
String const * mimehandler_get_icon(MimeHandler * handler, int translate);
String const * mimehandler_get_name(MimeHandler * handler, int translate);
String const * mimehandler_get_path(MimeHandler * handler);
String const * mimehandler_get_program(MimeHandler * handler);
MimeHandlerType mimehandler_get_type(MimeHandler * handler);
String const ** mimehandler_get_types(MimeHandler * handler);
String const * mimehandler_get_url(MimeHandler * handler);

int mimehandler_is_deleted(MimeHandler * handler);

int mimehandler_set_environment(MimeHandler * handler,
		String const * environment);

/* useful */
int mimehandler_open(MimeHandler * handler, String const * filename);

int mimehandler_load(MimeHandler * handler, String const * filename);
int mimehandler_load_by_name(MimeHandler * handler, String const * name);
int mimehandler_save(MimeHandler * handler);

#endif /* !LIBDESKTOP_DESKTOP_MIMEHANDLER_H */
