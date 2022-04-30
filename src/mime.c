/* $Id$ */
/* Copyright (c) 2011-2022 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS libDesktop */
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



#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <errno.h>
#include <libintl.h>
#include <System.h>
#include "Desktop.h"
#include "mimehandler.h"
#include "../config.h"

/* constants */
#ifndef PREFIX
# define PREFIX		"/usr/local"
#endif
#ifndef DATADIR
# define DATADIR	PREFIX "/share"
#endif


/* Mime */
/* private */
/* types */
typedef struct _MimeType
{
	char * type;
	char ** globs;
	size_t globs_cnt;
	GdkPixbuf * icon_24;
	GdkPixbuf * icon_48;
	GdkPixbuf * icon_96;
} MimeType;

struct _Mime
{
	GtkIconTheme * theme;
	MimeType * types;
	size_t types_cnt;
	Config * config;
};


/* constants */
#define MIME_CONFIG_FILE ".mime"


/* prototypes */
static char * _mime_get_config_filename(void);
static GdkPixbuf * _mime_icons_size(Mime * mime, char const * type, int size);


/* public */
/* functions */
/* mime_new */
static void _new_config(Mime * mime);

Mime * mime_new(GtkIconTheme * theme)
{
	Mime * mime;
	char * globs2[] =
	{
		DATADIR "/mime/globs2",
		"/usr/share/mime/globs2",
		"/usr/local/share/mime/globs2",
		"/usr/pkg/share/mime/globs2",
		NULL
	};
	char * globs[] =
	{
		DATADIR "/mime/globs",
		"/usr/share/mime/globs",
		"/usr/local/share/mime/globs",
		"/usr/pkg/share/mime/globs",
		NULL
	};
	int priority = 1;
	char ** g;
	FILE * fp = NULL;
	char buf[256];
	size_t len;
	char * p;
	char * glob;
	MimeType * type;
	size_t i;
	char ** q;

	if((mime = object_new(sizeof(*mime))) == NULL)
		return NULL;
	if(theme == NULL)
		theme = gtk_icon_theme_get_default();
	mime->theme = theme;
	for(g = globs2; *g != NULL; g++)
		if((fp = fopen(*g, "r")) != NULL)
			break;
	if(fp == NULL)
		for(g = globs, priority = 0; *g != NULL; g++)
			if((fp = fopen(*g, "r")) != NULL)
				break;
	if(fp == NULL)
	{
		error_set_code(1, "%s", "Could not load MIME globs");
		object_delete(mime);
		return NULL;
	}
	mime->types = NULL;
	mime->types_cnt = 0;
	_new_config(mime);
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		errno = EINVAL;
		len = strlen(buf);
		if(buf[--len] != '\n')
			break;
		if(buf[0] == '#')
			continue;
		buf[len] = '\0';
		if(priority)
		{
			/* parse the priority */
			errno = 0;
			strtoul(buf, &p, 0);
			if(errno != 0 || *(p++) != ':')
				continue;
		}
		else
			p = buf;
		glob = strchr(p, ':');
		*(glob++) = '\0';
		for(i = 0; i < mime->types_cnt; i++)
			if(strcmp(mime->types[i].type, p) == 0)
				break;
		if(i < mime->types_cnt)
			type = &mime->types[i];
		else if((type = realloc(mime->types, sizeof(*type)
						* (mime->types_cnt + 1)))
				== NULL)
			break;
		else
		{
			mime->types = type;
			type = &type[mime->types_cnt];
			type->type = strdup(p);
			type->globs = NULL;
			type->globs_cnt = 0;
		}
		if((q = realloc(type->globs, sizeof(*type->globs)
						* (type->globs_cnt + 1)))
				!= NULL)
		{
			type->globs = q;
			type->globs[type->globs_cnt] = strdup(glob);
		}
		if(type->type == NULL
				|| type->globs == NULL
				|| type->globs[type->globs_cnt] == NULL)
		{
			free(type->type);
			free(type->globs);
			break;
		}
		if(type->globs_cnt++ == 0)
			mime->types_cnt++;
		type->icon_24 = NULL;
		type->icon_48 = NULL;
		type->icon_96 = NULL;
#if 0
		type->open = mime->config != NULL
			? config_get(mime->config, p, "open") : NULL;
		type->edit = mime->config != NULL
			? config_get(mime->config, p, "edit") : NULL;
#endif
	}
	if(!feof(fp))
	{
		perror(*g);
		mime_delete(mime);
		mime = NULL;
	}
	fclose(fp);
	return mime;
}

static void _new_config(Mime * mime)
{
	char * filename;

	if((mime->config = config_new()) == NULL)
		return;
	if((filename = _mime_get_config_filename()) == NULL)
		return;
	config_load(mime->config, filename);
	string_delete(filename);
}


/* mime_delete */
void mime_delete(Mime * mime)
{
	size_t i;
	size_t j;

	for(i = 0; i < mime->types_cnt; i++)
	{
		free(mime->types[i].type);
		for(j = 0; j < mime->types[i].globs_cnt; j++)
			free(mime->types[i].globs[j]);
		free(mime->types[i].globs);
		if(mime->types[i].icon_24 != NULL)
			g_object_unref(mime->types[i].icon_24);
		if(mime->types[i].icon_48 != NULL)
			g_object_unref(mime->types[i].icon_48);
		if(mime->types[i].icon_96 != NULL)
			g_object_unref(mime->types[i].icon_96);
	}
	free(mime->types);
	if(mime->config != NULL)
		config_delete(mime->config);
	object_delete(mime);
}


/* accessors */
/* mime_get_handler */
static MimeHandler * _get_handler_executable(char const * type,
		char const * program);

MimeHandler * mime_get_handler(Mime * mime, char const * type,
		char const * action)
{
	MimeHandler * handler;
	String const * program;
	String * p;
	String * q;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(\"%s\", \"%s\")\n", __func__, type, action);
#endif
	if(type == NULL || action == NULL)
	{
		error_set_code(-EINVAL, "%s", strerror(EINVAL));
		return NULL;
	}
	if((program = config_get(mime->config, type, action)) != NULL)
	{
		if(program[0] == '/')
			/* open with a specific executable */
			return _get_handler_executable(type, program);
		if((handler = mimehandler_new_load_by_name(program)) != NULL)
			return handler;
	}
	/* generic fallback */
	if((p = string_find(type, "/")) == NULL || *(++p) == '\0'
			|| (p = string_new(type)) == NULL)
		/* XXX the error may not be reported */
		return NULL;
	q = string_find(p, "/");
	q[1] = '*';
	q[2] = '\0';
	program = config_get(mime->config, p, action);
	string_delete(p);
	if(program == NULL)
		return NULL;
	if(program[0] == '/')
		/* open with a specific executable */
		return _get_handler_executable(type, program);
	return mimehandler_new_load_by_name(program);
}

static MimeHandler * _get_handler_executable(char const * type,
		char const * program)
{
	MimeHandler * handler;
	String * p;

	if((handler = mimehandler_new()) == NULL)
		return NULL;
	if((p = string_new_append(program, " %f", NULL)) == NULL
			|| mimehandler_set(handler, "Type", "Application") != 0
			|| mimehandler_set(handler, "Name", program) != 0
			|| mimehandler_set(handler, "MimeType", type) != 0
			|| mimehandler_set(handler, "Exec", p) != 0)
	{
		string_delete(p);
		mimehandler_delete(handler);
		return NULL;
	}
	string_delete(p);
	return handler;
}


/* mime_set_handler */
int mime_set_handler(Mime * mime, char const * type, char const * action,
		char const * handler)
{
	if(handler != NULL && strcmp(handler, "") == 0)
		handler = NULL;
	return config_set(mime->config, type, action, handler);
}


/* useful */
/* mime_type */
char const * mime_type(Mime * mime, char const * path)
{
	char const * p;
	size_t i;
	size_t j;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(\"%s\")\n", __func__, path);
#endif
	p = strrchr(path, '/');
	p = (p != NULL) ? p + 1 : path;
	for(i = 0; i < mime->types_cnt; i++)
	{
		for(j = 0; j < mime->types[i].globs_cnt; j++)
			if(fnmatch(mime->types[i].globs[j], p, FNM_NOESCAPE)
					== 0)
				break;
		if(j < mime->types[i].globs_cnt)
			break;
	}
#ifdef FNM_CASEFOLD
	if(i < mime->types_cnt)
		return mime->types[i].type;
	for(i = 0; i < mime->types_cnt; i++)
		for(j = 0; j < mime->types[i].globs_cnt; j++)
			if(fnmatch(mime->types[i].globs[j], p,
						FNM_NOESCAPE | FNM_CASEFOLD)
					== 0)
				return mime->types[i].type;
#endif
	return NULL;
}


/* mime_action */
int mime_action(Mime * mime, char const * action, char const * path)
{
	char const * type;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(\"%s\", \"%s\")\n", __func__, action, path);
#endif
	if((type = mime_type(mime, path)) == NULL)
		return 1;
	return mime_action_type(mime, action, path, type);
}


/* mime_action_type */
int mime_action_type(Mime * mime, char const * action, char const * path,
		char const * type)
{
	int ret;
	MimeHandler * handler;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(\"%s\", \"%s\", \"%s\")\n", __func__, action,
			path, type);
#endif
	if((handler = mime_get_handler(mime, type, action)) == NULL)
		return -1;
	ret = mimehandler_open(handler, path);
	mimehandler_delete(handler);
	return ret;
}


/* mime_foreach */
void mime_foreach(Mime * mime, MimeForeachCallback callback, void * data)
{
	size_t i;

	for(i = 0; i < mime->types_cnt; i++)
	{
		if(mime->types[i].icon_24 == NULL)
			mime->types[i].icon_24 = _mime_icons_size(mime,
					mime->types[i].type, 24);
		if(mime->types[i].icon_48 == NULL)
			mime->types[i].icon_48 = _mime_icons_size(mime,
					mime->types[i].type, 48);
		if(mime->types[i].icon_96 == NULL)
			mime->types[i].icon_96 = _mime_icons_size(mime,
					mime->types[i].type, 96);
		callback(data, mime->types[i].type, mime->types[i].icon_24,
				mime->types[i].icon_48, mime->types[i].icon_96);
	}
}


/* mime_icons */
void mime_icons(Mime * mime, char const * type, ...)
{
	size_t i;
	va_list arg;
	int size;
	GdkPixbuf ** icon;

	if(type == NULL)
		i = mime->types_cnt;
	else
		for(i = 0; i < mime->types_cnt; i++)
			if(strcmp(type, mime->types[i].type) == 0)
				break;
	va_start(arg, type);
	while((size = va_arg(arg, int)) > 0)
	{
		icon = va_arg(arg, GdkPixbuf **);
		if(i >= mime->types_cnt)
			*icon = _mime_icons_size(mime, type, size);
		else if(size == 24)
		{
			if(mime->types[i].icon_24 == NULL)
				mime->types[i].icon_24 = _mime_icons_size(mime,
						type, size);
			*icon = mime->types[i].icon_24;
			g_object_ref(*icon);
		}
		else if(size == 48)
		{
			if(mime->types[i].icon_48 == NULL)
				mime->types[i].icon_48 = _mime_icons_size(mime,
						type, size);
			*icon = mime->types[i].icon_48;
			g_object_ref(*icon);
		}
		else if(size == 96)
		{
			if(mime->types[i].icon_96 == NULL)
				mime->types[i].icon_96 = _mime_icons_size(mime,
						type, size);
			*icon = mime->types[i].icon_96;
			g_object_ref(*icon);
		}
		else
			*icon = _mime_icons_size(mime, type, size);
	}
	va_end(arg);
}


/* mime_save */
int mime_save(Mime * mime)
{
	int ret;
	char * filename;

	if((filename = _mime_get_config_filename()) == NULL)
		return -1;
	ret = config_save(mime->config, filename);
	string_delete(filename);
	return ret;
}


/* private */
/* functions */
/* mime_get_config_filename */
static char * _mime_get_config_filename(void)
{
	char const * homedir;

	if((homedir = getenv("HOME")) == NULL
			&& (homedir = g_get_home_dir()) == NULL)
		return NULL; /* XXX set error */
	return string_new_append(homedir, "/", MIME_CONFIG_FILE, NULL);
}


/* mime_icons_size */
static GdkPixbuf * _icons_size_fallback_file(Mime * mime, int size, int flags);
static GdkPixbuf * _icons_size_fallback_folder(Mime * mime, int size,
		int flags);

static GdkPixbuf * _mime_icons_size(Mime * mime, char const * type, int size)
{
	GdkPixbuf * ret;
	static char buf[256] = "gnome-mime-";
	char * p;
	GtkIconLookupFlags flags = GTK_ICON_LOOKUP_USE_BUILTIN
#if GTK_CHECK_VERSION(2, 12, 0)
		| GTK_ICON_LOOKUP_GENERIC_FALLBACK
#endif
		;
	char const mountpoint[] = "inode/mountpoint";
	char const folder[] = "inode/directory";

	if(type == NULL)
		return _icons_size_fallback_file(mime, size, flags);
	/* check if it is a mountpoint */
	if(strcmp(type, mountpoint) == 0)
	{
		if((ret = gtk_icon_theme_load_icon(mime->theme, "mount-point",
						size, flags, NULL)) != NULL)
			return ret;
		/* fallback to a folder icon */
		return _icons_size_fallback_folder(mime, size, flags);
	}
	/* check if it is a folder */
	if(strcmp(type, folder) == 0)
		return _icons_size_fallback_folder(mime, size, flags);
	/* try to guess an icon */
	strncpy(&buf[11], type, sizeof(buf) - 11);
	buf[sizeof(buf) - 1] = '\0';
	for(; (p = strchr(&buf[11], '/')) != NULL; *p = '-');
	if((ret = gtk_icon_theme_load_icon(mime->theme, buf, size,
					flags, NULL)) != NULL)
		return ret;
	return _icons_size_fallback_file(mime, size, flags);
}

static GdkPixbuf * _icons_size_fallback_file(Mime * mime, int size, int flags)
{
	GdkPixbuf * ret;
	char const * fallbacks[] =
	{
		"gnome-fs-regular",
#if GTK_CHECK_VERSION(2, 6, 0)
		GTK_STOCK_FILE,
#endif
		GTK_STOCK_MISSING_IMAGE
	};
	size_t i;

	/* fallback to a regular icon */
	for(i = 0; i < sizeof(fallbacks) / sizeof(*fallbacks); i++)
		if((ret = gtk_icon_theme_load_icon(mime->theme, fallbacks[i],
						size, flags, NULL)) != NULL)
			return ret;
	return NULL;
}

static GdkPixbuf * _icons_size_fallback_folder(Mime * mime, int size, int flags)
{
	GdkPixbuf * ret;
	char const * fallbacks[] =
	{
		"gnome-fs-directory",
#if GTK_CHECK_VERSION(2, 6, 0)
		GTK_STOCK_DIRECTORY,
#endif
		GTK_STOCK_MISSING_IMAGE
	};
	size_t i;

	/* fallback to a folder icon */
	for(i = 0; i < sizeof(fallbacks) / sizeof(*fallbacks); i++)
		if((ret = gtk_icon_theme_load_icon(mime->theme, fallbacks[i],
						size, flags, NULL)) != NULL)
			return ret;
	/* fallback to a regular icon */
	return _icons_size_fallback_file(mime, size, flags);
}
