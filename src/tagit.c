/* Copyright (C) 2018-2020 Marios Tsolekas <marios.tsolekas@gmail.com>

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
#include <libgen.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include <tag_c.h>

#include "config.h"

#include "util.h"

#define HELP "Usage: tagit DIR [-f EXT1,[EXT2,...]]\n\
Automatically assign artist and title tags, based on filename.\n\
If no extensions are provided, then opus is used by default.\n\n\
Available options are\n\
  -h, --help     print this message\n\
  -v, --version  print version information\n\
  -f, --formats  comma seperated list of extensions of music files\n\
                 (Default is just opus)"


size_t total, skipped;

int edit(glob_t *, char *, char *);

int main(int argc, char **argv)
{
    struct stat sb;
    char *format, *pat;
    int globerr;
    glob_t pglob;
    int formats_n;
    char **formats;

    static struct option long_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { "format", required_argument, NULL, 'f' },
        { NULL, 0, NULL, 0 }
    };

    formats_n = 0;

    for (int optidx = 0;; optidx = 0) {
        int c = getopt_long(argc, argv, "hvf:", long_opts, &optidx);
        if (c == -1)
            break;

        switch (c) {
        case '?':
        case 'h':
            fprintf(stderr, "%s\n", HELP);
            return EXIT_SUCCESS;
        case 'v':
            fprintf(stderr, "%s\n", VERSION);
            return EXIT_SUCCESS;
        case 'f':
            if (!formats_n)
                formats = xmalloc(sizeof(char *) * (formats_n + 1));
            else
                formats = xrealloc(formats, sizeof(char *) * (formats_n + 1));

            formats[formats_n++] = strtok(optarg, ",");
            for (char *token; (token = strtok(NULL, ","));) {
                formats = xrealloc(formats, sizeof(char *) * (formats_n + 1));
                formats[formats_n++] = token;
            }

            break;
        }

    }

    if (argc < 2) {
        fprintf(stderr, "%s: too few arguments\n", basename(argv[0]));
        return EXIT_FAILURE;
    }

    if (stat(argv[optind], &sb) || !S_ISDIR(sb.st_mode)) {
        fprintf(stderr, "%s: first argument must be a directory\n",
                basename(argv[0]));
        return EXIT_FAILURE;
    }

    if (!formats_n && argc - optind == 1) {
        formats_n = 1;
        formats = xmalloc(sizeof(char *) * formats_n);
        formats[0] = "opus";
    } else if (!formats_n) {
        formats_n = argc - optind - 1;
        formats = xmalloc(sizeof(char *) * formats_n);
        for (int i = optind + 1; i < argc; ++i)
            formats[i - optind - 1] = argv[i];
    }

    skipped = 0;
    total = 0;

    for (int i = 0; i < formats_n; ++i) {
        format = strconcat("*.", formats[i]);
        pat = pathconcat(argv[optind], format);

        globerr = glob(pat, GLOB_ERR, NULL, &pglob);

        if (globerr == GLOB_NOMATCH)
            fprintf(stderr, "%s: no match for %s files\n",
                    basename(argv[0]), formats[i]);
        else if (globerr)
            fprintf(stderr, "%s: glob error\n", basename(argv[0]));

        if (!globerr) {
            edit(&pglob, argv[0], format);
            printf(" %s files\n", formats[i]);
        }

        free(format);
        free(pat);
        globfree(&pglob);
    }

    free(formats);

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
    if (!interval)
        interval = 1;

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
