/* $Id$ */
/* Copyright (c) 2009-2012 Pierre Pronchery <khorben@defora.org> */
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



#ifndef LIBDESKTOP_DESKTOP_ABOUT_H
# define LIBDESKTOP_DESKTOP_ABOUT_H


/* About */
GtkWidget * desktop_about_dialog_new(void);
void desktop_about_dialog_set_artists(GtkWidget * about,
		char const * artists[]);
void desktop_about_dialog_set_authors(GtkWidget * about,
		char const * authors[]);
void desktop_about_dialog_set_comments(GtkWidget * about,
		char const * comments);
void desktop_about_dialog_set_copyright(GtkWidget * about,
		char const * copyright);
void desktop_about_dialog_set_license(GtkWidget * about, char const * license);
void desktop_about_dialog_set_logo_icon_name(GtkWidget * about,
		char const * icon);
/* XXX desktop_about_dialog_set_name() is deprecated */
void desktop_about_dialog_set_name(GtkWidget * about, char const * name);
void desktop_about_dialog_set_program_name(GtkWidget * about,
		char const * name);
void desktop_about_dialog_set_translator_credits(GtkWidget * about,
		char const * translator);
void desktop_about_dialog_set_version(GtkWidget * about, char const * version);
void desktop_about_dialog_set_website(GtkWidget * about, char const * website);

#endif /* !LIBDESKTOP_DESKTOP_ABOUT_H */
