/* $Id$ */
/* Copyright (c) 2017 Pierre Pronchery <khorben@defora.org> */
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



#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Desktop.h"


/* private */
/* constants */
#define EXTENSION	".desktop"
#define SECTION		"Desktop Entry"


/* public */
/* functions */
/* mimehandler_new */
MimeHandler * mimehandler_new(void)
{
	return config_new();
}


/* mimehandler_new_load */
MimeHandler * mimehandler_new_load(String const * name)
{
	MimeHandler * handler;

	if((handler = mimehandler_new()) == NULL)
		return NULL;
	if(mimehandler_load_by_name(handler, name) != 0)
	{
		mimehandler_delete(handler);
		return NULL;
	}
	return handler;
}


/* mimehandler_delete */
void mimehandler_delete(MimeHandler * handler)
{
	config_delete(handler);
}


/* accessors */
/* mimehandler_can_display */
int mimehandler_can_display(MimeHandler * handler)
{
	String const * p;

	if((p = config_get(handler, SECTION, "NoDisplay")) == NULL)
		return 1;
	return (string_compare(p, "true") == 0) ? 1 : 0;
}


/* mimehandler_can_execute */
static int _can_execute_access(String const * path, int mode);
static int _can_execute_access_path(String const * path,
		String const * filename, int mode);

int mimehandler_can_execute(MimeHandler * handler)
{
	String const * p;

	if(mimehandler_get_type(handler) != MIME_HANDLER_TYPE_APPLICATION)
		return 0;
	if((p = config_get(handler, SECTION, "TryExec")) == NULL)
		return 1;
	return _can_execute_access(p, X_OK);
}

static int _can_execute_access(String const * path, int mode)
{
	int ret = -1;
	String const * p;
	String * q;
	String * last;

	if(path[0] == '/')
		return access(path, mode);
	if((p = getenv("PATH")) == NULL)
		return 0;
	if((q = string_new(p)) == NULL)
		return 0;
	for(p = strtok_r(q, ":", &last); p != NULL;
			p = strtok_r(NULL, ":", &last))
		ret = _can_execute_access_path(p, path, mode);
	string_delete(q);
	return ret;
}

static int _can_execute_access_path(String const * path,
		String const * filename, int mode)
{
	int ret;
	String * p;

	if((p = string_new_append(path, "/", filename, NULL)) == NULL)
		return -1;
	ret = access(p, mode);
	string_delete(p);
	return ret;
}


/* mimehandler_get_name */
String const * mimehandler_get_name(MimeHandler * handler)
{
	return config_get(handler, SECTION, "Name");
}


/* mimehandler_get_program */
String const * mimehandler_get_program(MimeHandler * handler)
{
	switch(mimehandler_get_type(handler))
	{
		case MIME_HANDLER_TYPE_APPLICATION:
			/* XXX may be a format string */
			return config_get(handler, SECTION, "Exec");
		case MIME_HANDLER_TYPE_DIRECTORY:
		case MIME_HANDLER_TYPE_UNKNOWN:
		case MIME_HANDLER_TYPE_URL:
			return NULL;
	}
	return NULL;
}


/* mimehandler_get_type */
MimeHandlerType mimehandler_get_type(MimeHandler * handler)
{
	String const * type;
	struct
	{
		String const * name;
		MimeHandlerType type;
	} types[] =
	{
		{ "Application",MIME_HANDLER_TYPE_APPLICATION	},
		{ "Directory",	MIME_HANDLER_TYPE_DIRECTORY	},
		{ "URL",	MIME_HANDLER_TYPE_URL		}
	};
	size_t i;

	if((type = config_get(handler, SECTION, "Type")) == NULL)
		return MIME_HANDLER_TYPE_UNKNOWN;
	for(i = 0; i < sizeof(types) / sizeof(*types); i++)
		if(string_compare(types[i].name, type) == 0)
			return types[i].type;
	return MIME_HANDLER_TYPE_UNKNOWN;
}


/* mimehandler_is_hidden */
int mimehandler_is_hidden(MimeHandler * handler)
{
	String const * p;

	if((p = config_get(handler, SECTION, "Hidden")) == NULL)
		return 0;
	return (string_compare(p, "true") == 0) ? 1 : 0;
}


/* useful */
/* mimehandler_load_by_name */
static int _load_by_name_path(MimeHandler * handler, String const * name,
		String const * path);

int mimehandler_load_by_name(MimeHandler * handler, String const * name)
{
	int ret;
	String const fallback[] = ".local/share";
	String const * path;
	String const * homedir;
	String * p;
	String const * q;
	String * last;

	/* use $XDG_DATA_HOME if set and not empty */
	if((path = getenv("XDG_DATA_HOME")) != NULL && strlen(path) > 0
			&& _load_by_name_path(handler, name, path) == 0)
		return 0;
	/* fallback to "$HOME/.local/share" */
	if((homedir = getenv("HOME")) == NULL)
		homedir = g_get_home_dir();
	if((p = string_new_append(homedir, "/", fallback, NULL)) == NULL)
		return -1;
	ret = _load_by_name_path(handler, name, p);
	string_delete(p);
	if(ret == 0)
		return ret;
	/* read through every XDG application folder */
	if((path = getenv("XDG_DATA_DIRS")) == NULL || strlen(path) == 0)
		path = "/usr/local/share:/usr/share";
	if((p = string_new(path)) == NULL)
		return -1;
	for(q = strtok_r(p, ":", &last); q != NULL;
			q = strtok_r(NULL, ":", &last))
		if((ret = _load_by_name_path(handler, name, q)) == 0)
			break;
	string_delete(p);
	return ret;
}

static int _load_by_name_path(MimeHandler * handler, String const * name,
		String const * path)
{
	int ret;
	String const applications[] = "/applications/";
	String * pathname;

	if((pathname = string_new_append(path, applications, name, EXTENSION,
					NULL)) == NULL)
		return -1;
	ret = (config_reset(handler) == 0
			&& config_load(handler, pathname) == 0
			&& mimehandler_get_type(handler)
				!= MIME_HANDLER_TYPE_UNKNOWN
			&& mimehandler_get_name(handler) != NULL
			&& mimehandler_is_hidden(handler) == 0)
		? 0 : -1;
	string_delete(pathname);
	return ret;
}