/* Copyright (C) 2018, 2019 Marios Tsolekas <marios.tsolekas@gmail.com>

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
#include <unistd.h>
#include <libgen.h>
#include <glob.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tag_c.h>

#include "util.h"

size_t total, skipped;

int edit(glob_t *, char *, char *);

int main(int argc, char **argv)
{
    char *format, *pat;
    int globerr;
    glob_t pglob;

    if (argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        printf("Usage: tagit DIR_TO_EDIT EXT_1 EXT_2\n\
Automatically assign artist and title tags, based on filename\n\n\
Available options are\n\
  -h    print this message\n");
        return EXIT_SUCCESS;
    }

    if (argc < 3) {
        fprintf(stderr, "%s: too few arguments\n", basename(argv[0]));
        return EXIT_FAILURE;
    }

    skipped = 0;
    total = 0;

    for (int i = 2; i < argc; ++i) {
        format = strconcat("*.", argv[i]);
        pat = pathconcat(argv[1], format);

        globerr = glob(pat, GLOB_ERR, NULL, &pglob);

        if (globerr == GLOB_NOMATCH)
            fprintf(stderr, "%s: no match for %s files\n",
                    basename(argv[0]), argv[i]);
        else if (globerr)
            fprintf(stderr, "%s: glob error\n", basename(argv[0]));

        if (!globerr) {
            edit(&pglob, argv[0], format);
            printf(" %s files\n", argv[i]);
        }

        free(format);
        free(pat);
        globfree(&pglob);
    }

    printf("\nEdited %ld files(s) (%ld skipped)\n", total - skipped, skipped);

    return EXIT_SUCCESS;
}

int edit(glob_t *pglob, char *prog, char *format)
{
    char *fname, *delim, *ext;
    char *artist, *tag_artist, *title, *tag_title;
    size_t interval;
    TagLib_File *file;
    TagLib_Tag *tag;

    total += pglob->gl_pathc;
    interval = pglob->gl_pathc / 10;

    for (size_t i = 0; i < pglob->gl_pathc; ++i) {
        fname = basename(pglob->gl_pathv[i]);

        if (!(file = taglib_file_new(pglob->gl_pathv[i]))) {
            fprintf(stderr, "%s: file read error for", basename(prog));
            total -= i + 1;
            return -1;
        }

        if (!(tag = taglib_file_tag(file))) {
            fprintf(stderr, "%s: tag read error for", basename(prog));
            total -= i + 1;
            return -1;
        }

        delim = strstr(fname, " - ") + 3;
        ext = strstr(fname, format + 1);

        artist = xstrndup(fname, strlen(fname) - strlen(delim - 3));
        tag_artist = taglib_tag_artist(tag);

        title = xstrndup(delim, strlen(delim) - strlen(ext));
        tag_title = taglib_tag_title(tag);

        if (strcmp(artist, tag_artist) || strcmp(title, tag_title)) {
            taglib_tag_set_artist(tag, artist);
            taglib_tag_set_title(tag, title);
            taglib_file_save(file);
        } else {
            ++skipped;
        }

        if (i % interval == 0 || i + 10 >= pglob->gl_pathc)
            progress_bar(i + 1, pglob->gl_pathc);

        free(artist);
        free(title);

        taglib_tag_free_strings();
        taglib_file_free(file);
    }

    return 0;
}
