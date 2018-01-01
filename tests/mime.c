/* $Id$ */
/* Copyright (c) 2018 Pierre Pronchery <khorben@defora.org> */
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



#include <stdio.h>
#include <string.h>
#include "Desktop/mime.h"


/* main */
int main(void)
{
	int ret = 0;
	Mime * mime;
	struct
	{
		char const * filename;
		char const * type;
	} tests[] = {
		{ "Makefile", "text/x-makefile" },
		{ "../include/Desktop/mime.h", "text/x-chdr" },
		{ "mime.c", "text/x-csrc" }
	};
	size_t i;
	char const * p;

	if((mime = mime_new(NULL)) == NULL)
		return 2;
	for(i = 0; i < sizeof(tests) / sizeof(*tests); i++)
		if((p = mime_type(mime, tests[i].filename)) == NULL)
			/* XXX ignore this as shared-mime-info may be missing */
			printf("%s: %s (expected: %s)\n", tests[i].filename,
					"Unknown type", tests[i].type);
		else if(strcmp(tests[i].type, p) != 0)
		{
			printf("%s: %s (expected: %s)\n", tests[i].filename, p,
					tests[i].type);
			ret = 3;
		}
		else
			printf("%s: %s\n", tests[i].filename, p);
	mime_delete(mime);
	return ret;
}
