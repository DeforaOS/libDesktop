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
#include <stdio.h>
#include "../src/mimehandler.c"


/* main */
int main(void)
{
	MimeHandler * handler;
	char * path;
	String ** types;

	if((path = getcwd(NULL, 0)) == NULL)
		return 2;
	if(setenv("XDG_DATA_HOME", path, 1) != 0)
	{
		free(path);
		return 3;
	}
	free(path);
	if((handler = mimehandler_new_load("Root")) == NULL)
		return 4;
	if(mimehandler_get_type(handler) != MIME_HANDLER_TYPE_DIRECTORY)
	{
		mimehandler_delete(handler);
		return 5;
	}
	if(mimehandler_get_types(handler) != NULL)
	{
		mimehandler_delete(handler);
		return 6;
	}
	mimehandler_delete(handler);
	if((handler = mimehandler_new_open("applications/Widget.desktop"))
			== NULL)
		return 7;
	if((types = mimehandler_get_types(handler)) == NULL
			|| types[0] != NULL)
	{
		mimehandler_delete(handler);
		return 8;
	}
	free(types);
	mimehandler_delete(handler);
	return 0;
}
