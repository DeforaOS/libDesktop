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
#ifdef DEBUG
# include <stdio.h>
#endif
#include <string.h>
#include <errno.h>
#include <System.h>
#include "Desktop.h"

#ifndef PREFIX
# define PREFIX		"/usr/local"
#endif
#ifndef BINDIR
# define BINDIR		PREFIX "/bin"
#endif
#ifndef DATADIR
# define DATADIR	PREFIX "/share"
#endif


/* private */
/* types */
struct _MimeHandler
{
	Config * config;
	String * filename;
	String ** categories;
	String ** types;
};


/* constants */
#define EXTENSION	".desktop"
#define SECTION		"Desktop Entry"


/* prototypes */
/* accessors */
static String const * _mimehandler_get_translation(MimeHandler * handler,
		String const * key);

/* useful */
static void _mimehandler_cache_invalidate(MimeHandler * handler);


/* public */
/* functions */
/* mimehandler_new */
MimeHandler * mimehandler_new(void)
{
	MimeHandler * handler;

	if((handler = object_new(sizeof(*handler))) == NULL)
		return NULL;
	handler->config = config_new();
	handler->filename = NULL;
	handler->categories = NULL;
	handler->types = NULL;
	if(handler->config == NULL)
	{
		mimehandler_delete(handler);
		return NULL;
	}
	return handler;
}


/* mimehandler_new_load */
MimeHandler * mimehandler_new_load(String const * filename)
{
	MimeHandler * handler;

	if((handler = mimehandler_new()) == NULL)
		return NULL;
	if(mimehandler_load(handler, filename) != 0)
	{
		mimehandler_delete(handler);
		return NULL;
	}
	return handler;
}


/* mimehandler_new_load_by_name */
MimeHandler * mimehandler_new_load_by_name(String const * name)
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
	_mimehandler_cache_invalidate(handler);
	config_delete(handler->config);
	string_delete(handler->filename);
	object_delete(handler);
}


/* accessors */
/* mimehandler_can_display */
int mimehandler_can_display(MimeHandler * handler)
{
	String const * p;

	if(mimehandler_is_deleted(handler))
		return 0;
	/* XXX allow setting a value for the desktop environment */
	if(config_get(handler->config, SECTION, "OnlyShowIn") != NULL)
		return 0;
	if((p = config_get(handler->config, SECTION, "NoDisplay")) != NULL
			&& string_compare(p, "true") == 0)
		return 0;
	return 1;
}


/* mimehandler_can_execute */
static int _can_execute_access(String const * path, int mode);
static int _can_execute_access_path(String const * path,
		String const * filename, int mode);

int mimehandler_can_execute(MimeHandler * handler)
{
	String const * p;

	if(mimehandler_get_type(handler) != MIMEHANDLER_TYPE_APPLICATION)
		return 0;
	if((p = config_get(handler->config, SECTION, "TryExec")) != NULL
			&& _can_execute_access(p, X_OK) == 0)
		return 0;
	return (mimehandler_get_program(handler) != NULL) ? 1 : 0;
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


/* mimehandler_can_open */
static int _can_open_application(MimeHandler * handler);

int mimehandler_can_open(MimeHandler * handler)
{
	switch(mimehandler_get_type(handler))
	{
		case MIMEHANDLER_TYPE_APPLICATION:
			return _can_open_application(handler);
		case MIMEHANDLER_TYPE_DIRECTORY:
			/* let errors be handled by the API user */
			return 1;
		case MIMEHANDLER_TYPE_LINK:
			return 1;
		case MIMEHANDLER_TYPE_UNKNOWN:
			return 0;
	}
	return 0;
}

static int _can_open_application(MimeHandler * handler)
{
	String const * program;
	String const * p;

	if(mimehandler_can_execute(handler) == 0)
		return 0;
	if((program = mimehandler_get_program(handler)) == NULL)
		/* XXX should not fail */
		return 0;
	for(p = string_find(program, "%"); p != NULL; p = string_find(p, "%"))
		switch(*(++p))
		{
			case 'f':
			case 'F':
			case 'u':
			case 'U':
				return 1;
		}
	return 0;
}


/* mimehandler_get_categories */
String const ** mimehandler_get_categories(MimeHandler * handler)
{
	String ** ret = NULL;
	size_t cnt = 0;
	size_t i;
	String const * p;
	String * q;
	String * last;
	String ** r;

	if(handler->categories != NULL)
		return (String const **)handler->categories;
	if((p = config_get(handler->config, SECTION, "Categories")) == NULL)
	{
		if((ret = malloc(sizeof(String *))) == NULL)
			return NULL;
		ret[0] = NULL;
		handler->categories = ret;
		return ret;
	}
	if((q = string_new(p)) == NULL)
		return NULL;
	for(p = strtok_r(q, ";", &last); p != NULL;
			p = strtok_r(NULL, ";", &last))
	{
		if(strlen(p) == 0)
			continue;
		if((r = realloc(ret, sizeof(*ret) * (cnt + 2))) != NULL)
		{
			ret = r;
			ret[cnt] = string_new(p);
		}
		if(r == NULL || ret[cnt] == NULL)
		{
			for(i = 0; i < cnt; i++)
				string_delete(ret[i]);
			free(ret);
			return NULL;
		}
		cnt++;
	}
	string_delete(q);
	if(ret != NULL)
		ret[cnt] = NULL;
	handler->categories = ret;
	return ret;
}


/* mimehandler_get_comment */
String const * mimehandler_get_comment(MimeHandler * handler, int translate)
{
	String const key[] = "Comment";

	if(translate)
		return _mimehandler_get_translation(handler, key);
	return config_get(handler->config, SECTION, key);
}


/* mimehandler_get_filename */
String const * mimehandler_get_filename(MimeHandler * handler)
{
	return handler->filename;
}


/* mimehandler_get_generic_name */
String const * mimehandler_get_generic_name(MimeHandler * handler,
		int translate)
{
	String const key[] = "GenericName";

	if(translate)
		return _mimehandler_get_translation(handler, key);
	return config_get(handler->config, SECTION, key);
}


/* mimehandler_get_icon */
String const * mimehandler_get_icon(MimeHandler * handler, int translate)
{
	String const key[] = "Icon";

	if(translate)
		return _mimehandler_get_translation(handler, key);
	return config_get(handler->config, SECTION, key);
}


/* mimehandler_get_name */
String const * mimehandler_get_name(MimeHandler * handler, int translate)
{
	String const key[] = "Name";

	if(translate)
		return _mimehandler_get_translation(handler, key);
	return config_get(handler->config, SECTION, key);
}


/* mimehandler_get_path */
String const * mimehandler_get_path(MimeHandler * handler)
{
	switch(mimehandler_get_type(handler))
	{
		case MIMEHANDLER_TYPE_APPLICATION:
		case MIMEHANDLER_TYPE_DIRECTORY:
			return config_get(handler->config, SECTION, "Path");
		default:
			return NULL;
	}
}


/* mimehandler_get_program */
String const * mimehandler_get_program(MimeHandler * handler)
{
	switch(mimehandler_get_type(handler))
	{
		case MIMEHANDLER_TYPE_APPLICATION:
			/* XXX may be a format string */
			return config_get(handler->config, SECTION, "Exec");
		case MIMEHANDLER_TYPE_DIRECTORY:
		case MIMEHANDLER_TYPE_UNKNOWN:
		case MIMEHANDLER_TYPE_LINK:
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
		{ "Application",MIMEHANDLER_TYPE_APPLICATION	},
		{ "Directory",	MIMEHANDLER_TYPE_DIRECTORY	},
		{ "Link",	MIMEHANDLER_TYPE_LINK		}
	};
	size_t i;

	if((type = config_get(handler->config, SECTION, "Type")) == NULL)
		return MIMEHANDLER_TYPE_UNKNOWN;
	for(i = 0; i < sizeof(types) / sizeof(*types); i++)
		if(string_compare(types[i].name, type) == 0)
			return types[i].type;
	return MIMEHANDLER_TYPE_UNKNOWN;
}


/* mimehandler_get_types */
String const ** mimehandler_get_types(MimeHandler * handler)
{
	String ** ret = NULL;
	size_t cnt = 0;
	size_t i;
	String const * p;
	String * q;
	String * last;
	String ** r;

	if(handler->types != NULL)
		return handler->types;
	if(mimehandler_get_type(handler) != MIMEHANDLER_TYPE_APPLICATION)
		return NULL;
	if((p = config_get(handler->config, SECTION, "MimeType")) == NULL)
	{
		if((ret = malloc(sizeof(String *))) == NULL)
			return NULL;
		ret[0] = NULL;
		handler->types = ret;
		return ret;
	}
	if((q = string_new(p)) == NULL)
		return NULL;
	for(p = strtok_r(q, ":", &last); p != NULL;
			p = strtok_r(NULL, ":", &last))
	{
		if(strlen(p) == 0)
			continue;
		if((r = realloc(ret, sizeof(*ret) * (cnt + 2))) != NULL)
		{
			ret = r;
			ret[cnt] = string_new(p);
		}
		if(r == NULL || ret[cnt] == NULL)
		{
			for(i = 0; i < cnt; i++)
				string_delete(ret[i]);
			free(ret);
			return NULL;
		}
		cnt++;
	}
	string_delete(q);
	if(ret != NULL)
		ret[cnt] = NULL;
	handler->types = ret;
	return ret;
}


/* mimehandler_get_url */
String const * mimehandler_get_url(MimeHandler * handler)
{
	if(mimehandler_get_type(handler) == MIMEHANDLER_TYPE_LINK)
		return config_get(handler->config, SECTION, "URL");
	return NULL;
}


/* mimehandler_is_deleted */
int mimehandler_is_deleted(MimeHandler * handler)
{
	String const * p;

	if((p = config_get(handler->config, SECTION, "Hidden")) != NULL
			&& string_compare(p, "true") == 0)
		return 1;
	switch(mimehandler_get_type(handler))
	{
		case MIMEHANDLER_TYPE_APPLICATION:
			if(mimehandler_can_execute(handler) == 0)
				return 1;
			break;
		default:
			break;
	}
	return 0;
}


/* useful */
/* mimehandler_load */
int mimehandler_load(MimeHandler * handler, String const * filename)
{
	Config * config;
	String * p;

	if((config = config_new()) == NULL)
		return -1;
	if(config_load(config, filename) != 0
			|| (p = string_new(filename)) == NULL)
	{
		config_delete(config);
		return -1;
	}
	config_delete(handler->config);
	handler->config = config;
	string_delete(handler->filename);
	handler->filename = p;
	_mimehandler_cache_invalidate(handler);
	return 0;
}


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

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(\"%s\")\n", __func__, name);
#endif
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
		path = "/usr/local/share:" DATADIR ":/usr/share";
	if((p = string_new(path)) == NULL)
		return -1;
	for(q = strtok_r(p, ":", &last); q != NULL;
			q = strtok_r(NULL, ":", &last))
		if((ret = _load_by_name_path(handler, name, q)) == 0)
			break;
	string_delete(p);
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() => %d\n", __func__, ret);
#endif
	return ret;
}

static int _load_by_name_path(MimeHandler * handler, String const * name,
		String const * path)
{
	int ret;
	String const applications[] = "/applications/";
	String * filename;

	if((filename = string_new_append(path, applications, name, EXTENSION,
					NULL)) == NULL)
		return -1;
	ret = mimehandler_load(handler, filename);
	string_delete(filename);
	return ret;
}


/* mimehandler_open */
static int _open_application(MimeHandler * handler, String const * filename);
static int _open_directory(MimeHandler * handler, String const * filename);
static int _open_url(MimeHandler * handler, String const * filename);

int mimehandler_open(MimeHandler * handler, String const * filename)
{
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(\"%s\")\n", __func__, filename);
#endif
	switch(mimehandler_get_type(handler))
	{
		case MIMEHANDLER_TYPE_APPLICATION:
			return _open_application(handler, filename);
		case MIMEHANDLER_TYPE_DIRECTORY:
			return _open_directory(handler, filename);
		case MIMEHANDLER_TYPE_LINK:
			return _open_url(handler, filename);
		case MIMEHANDLER_TYPE_UNKNOWN:
			/* XXX report error */
			return -1;
	}
	return error_set_code(-ENOSYS, "%s", strerror(ENOSYS));
}

static int _open_application(MimeHandler * handler, String const * filename)
{
	int ret = 0;
	String * program;
	String * p;
	String const * q;
	String const * name;
	String const * icon;
	size_t len;
	pid_t pid;
	GError * error = NULL;

	if((q = mimehandler_get_program(handler)) == NULL)
		return -1;
	if((program = string_new(q)) == NULL)
		return -1;
	for(p = string_find(program, "%"); p != NULL; p = string_find(p, "%"))
	{
		switch(p[1])
		{
			case 'c':
				/* XXX should not fail */
				if((name = mimehandler_get_name(handler, 1))
						== NULL)
				{
					/* ignore */
					memmove(p, &p[2], string_length(&p[1]));
					break;
				}
				*p = '\0';
				q = p;
				if((p = string_new_append(program, name, &q[2],
								NULL)) == NULL)
				{
					string_delete(program);
					return -1;
				}
				len = string_length(program)
					- string_length(&q[2]);
				string_delete(program);
				program = p;
				p += len;
				break;
			case 'f':
			case 'F':
				if(filename == NULL)
				{
					/* ignore */
					memmove(p, &p[2], string_length(&p[1]));
					break;
				}
				*p = '\0';
				q = p;
				/* FIXME escape filename */
				if((p = string_new_append(program, filename,
								&q[2], NULL))
						== NULL)
				{
					string_delete(program);
					return -1;
				}
				len = string_length(program)
					- string_length(&q[2]);
				string_delete(program);
				program = p;
				p += len;
				/* XXX avoid multiple inclusion */
				filename = NULL;
				break;
			case 'u':
			case 'U':
				if(filename == NULL)
				{
					/* ignore */
					memmove(p, &p[2], string_length(&p[1]));
					break;
				}
				*p = '\0';
				q = p;
				/* FIXME escape filename */
				if((p = string_new_append(program, "file:///",
								filename, &q[2],
								NULL)) == NULL)
				{
					string_delete(program);
					return -1;
				}
				len = string_length(program)
					- string_length(&q[2]);
				string_delete(program);
				program = p;
				p += len;
				/* XXX avoid multiple inclusion */
				filename = NULL;
				break;
			case 'i':
				if((icon = mimehandler_get_icon(handler, 1))
						== NULL)
				{
					/* ignore */
					memmove(p, &p[2], string_length(&p[1]));
					break;
				}
				*p = '\0';
				q = p;
				if((p = string_new_append(program, "--icon ",
								icon, &q[2],
								NULL)) == NULL)
				{
					string_delete(program);
					return -1;
				}
				len = string_length(program)
					- string_length(&q[2]);
				string_delete(program);
				program = p;
				p += len;
				break;
			case 'k':
				if((name = handler->filename) == NULL)
				{
					/* ignore */
					*(p++) = '"';
					*(p++) = '"';
					break;
				}
				*p = '\0';
				q = p;
				if((p = string_new_append(program, name, &q[2],
								NULL)) == NULL)
				{
					string_delete(program);
					return -1;
				}
				len = string_length(program)
					- string_length(&q[2]);
				string_delete(program);
				program = p;
				p += len;
				break;
			case '%':
				/* ignore */
				memmove(&p[1], &p[2], string_length(&p[1]));
				break;
			default:
				/* XXX skip */
				p++;
				break;
		}
	}
	if((q = mimehandler_get_path(handler)) == NULL)
	{
		/* execute the program directly */
		if(g_spawn_command_line_async(program, &error) != TRUE)
		{
			error_set_code(1, "%s: %s", program, error->message);
			g_error_free(error);
		}
	}
	else if((pid = fork()) == 0)
	{
		/* change the current working directory */
		if(chdir(q) != 0)
			error_set_code(-errno, "%s: %s: %s", program, q,
					strerror(errno));
		else if(g_spawn_command_line_async(program, &error) != TRUE)
		{
			error_set_code(1, "%s: %s", program, error->message);
			g_error_free(error);
		}
		exit(125);
	}
	else if(pid < 0)
	{
		error_set_code(-errno, "%s: %s", program, strerror(errno));
		ret = -1;
	}
	string_delete(program);
	return ret;
}

static int _open_directory(MimeHandler * handler, String const * filename)
{
	int ret = 0;
	String const * directory;
	/* XXX open with the default file manager instead */
	char * argv[] = { BINDIR "/browser", "--", NULL, NULL };
	const unsigned int flags = 0;
	GError * error = NULL;

	if(filename != NULL)
		return error_set_code(-EINVAL, "%s", strerror(EINVAL));
	/* XXX this may not be the correct key */
	if((directory = mimehandler_get_path(handler)) == NULL)
		/* XXX report an error? */
		return 0;
	if((argv[2] = string_new(directory)) == NULL)
		return -1;
	else if(g_spawn_async(NULL, argv, NULL, flags, NULL, NULL, NULL, &error)
			!= TRUE)
	{
		ret = -error_set_code(1, "%s: %s", directory, error->message);
		g_error_free(error);
	}
	string_delete(argv[2]);
	return ret;
}

static int _open_url(MimeHandler * handler, String const * filename)
{
	int ret = 0;
	String const * url;
	/* XXX open with the default web browser instead */
	char * argv[] = { BINDIR "/htmlapp", "--", NULL, NULL };
	unsigned int flags = 0;
	GError * error = NULL;

	if(filename != NULL)
		return error_set_code(-EINVAL, "%s", strerror(EINVAL));
	if((url = mimehandler_get_url(handler)) == NULL)
		/* XXX report an error? */
		return 0;
	if((argv[2] = string_new(url)) == NULL)
		return -1;
	else if(g_spawn_async(NULL, argv, NULL, flags, NULL, NULL, NULL, &error)
			!= TRUE)
	{
		ret = -error_set_code(1, "%s: %s", url, error->message);
		g_error_free(error);
	}
	string_delete(argv[2]);
	return ret;
}


/* private */
/* accessors */
/* mimehandler_get_translation */
static String const * _translation_strip_country(MimeHandler * handler,
		String * locale, String const * key);
static String const * _translation_strip_encoding(MimeHandler * handler,
		String * locale, String const * key);
static String const * _translation_strip_modifier(MimeHandler * handler,
		String * locale, String const * key);

static String const * _mimehandler_get_translation(MimeHandler * handler,
		String const * key)
{
	String const * ret;
	String * locale;

	if((ret = getenv("LC_MESSAGES")) != NULL
			|| (ret = getenv("LANG")) != NULL)
	{
		if((locale = string_new(ret)) == NULL)
			return NULL;
		if((ret = _translation_strip_encoding(handler, locale, key))
				== NULL
				&& (ret = _translation_strip_modifier(handler,
						locale, key)) == NULL)
			ret = _translation_strip_country(handler, locale, key);
		string_delete(locale);
		if(ret != NULL)
			return ret;
	}
	return config_get(handler->config, SECTION, key);
}

static String const * _translation_strip_country(MimeHandler * handler,
		String * locale, String const * key)
{
	String const * ret;
	String * p;

	if((p = string_find(locale, "_")) == NULL)
		return NULL;
	*p = '\0';
	if((p = string_new_append(key, "[", locale, "]", NULL)) == NULL)
		return NULL;
	ret = config_get(handler->config, SECTION, p);
	string_delete(p);
	return ret;
}

static String const * _translation_strip_encoding(MimeHandler * handler,
		String * locale, String const * key)
{
	String const * ret;
	String * p;

	/* FIXME really strip the encoding */
	if((p = string_new_append(key, "[", locale, "]", NULL)) == NULL)
		return NULL;
	ret = config_get(handler->config, SECTION, p);
	string_delete(p);
	return ret;
}

static String const * _translation_strip_modifier(MimeHandler * handler,
		String * locale, String const * key)
{
	String const * ret;
	String * p;

	if((p = string_find(locale, "@")) == NULL)
		return NULL;
	*p = '\0';
	if((p = string_new_append(key, "[", locale, "]", NULL)) == NULL)
		return NULL;
	ret = config_get(handler->config, SECTION, p);
	string_delete(p);
	return ret;
}


/* useful */
/* mimehandler_cache_invalidate */
static void _mimehandler_cache_invalidate(MimeHandler * handler)
{
	size_t i;

	if(handler->categories != NULL)
		for(i = 0; handler->categories[i] != NULL; i++)
			string_delete(handler->categories[i]);
	free(handler->categories);
	handler->categories = NULL;
	if(handler->types != NULL)
		for(i = 0; handler->types[i] != NULL; i++)
			string_delete(handler->types[i]);
	free(handler->types);
	handler->types = NULL;
}
