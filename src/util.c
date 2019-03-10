/* Copyright (C) 2018 Marios Tsolekas <marios.tsolekas@gmail.com>

   This file is part of Tagit.

   Tagt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Tagit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Tagit.  If not, see <http://www.gnu.org/licenses/>.

   Author: Marios Tsolekas */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void *xmalloc(size_t size)
{
    void *ret;

    ret = malloc(size);
    if (!ret && size) {
        fprintf(stderr, "%s:%d out of memory, ", __FILE__, __LINE__);
        abort();
    }

    return ret;
}

void *xrealloc(void *ptr, size_t size)
{
    void *ret;

    ret = realloc(ptr, size);
    if (!ret && size) {
        fprintf(stderr, "%s:%d out of memory, ", __FILE__, __LINE__);
        abort();
    }

    return ret;
}


char *xstrdup(const char *s)
{
    char *ret;

    ret = strdup(s);
    if (!ret) {
        fprintf(stderr, "%s:%d out of memory, ", __FILE__, __LINE__);
        abort();
    }

    return ret;
}

char *xstrndup(const char *s, size_t n)
{
    char *ret;

    ret = strndup(s, n);
    if (!ret) {
        fprintf(stderr, "%s:%d out of memory, ", __FILE__, __LINE__);
        abort();
    }

    return ret;
}


char *strconcat(char *base, char *src)
{
    char *str;
    size_t len;

    str = xstrdup(base);
    len = strlen(base) + strlen(src) + 1;

    str = xrealloc(str, len);
    str = strcat(str, src);

    return str;
}

char *pathconcat(char *base, char *src)
{
    char *str;
    size_t len;

    str = xstrdup(base);
    len = strlen(base) + strlen(src) + 2;

    str = xrealloc(str, len);
    str = strcat(str, "/");
    str = strcat(str, src);

    return str;
}

void progress_bar(size_t cur, size_t total)
{
    printf("\r[");
    for (int i = 1; i < 21; ++i)
        if ((float) cur / (float) total < i * 0.05)
            printf(" ");
        else
            printf("#");
    printf("] [%ld/%ld]", cur, total);
}
