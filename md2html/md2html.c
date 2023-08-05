/*
 * md2html.c
 *
 * Copyright 2017 Maksim Demyanov (neon1ks) <neon1ks@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include <dirent.h>
#include <libgen.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <glib.h>

enum {
    MAX_PATH_LEN = 1024
};

int force = 0;

char *get_git_path(const char *path);

void parse_dir(const char *path, unsigned long order);

gboolean if_file_name_md(const char *fname);

void process_file_md(const char *fname_md, time_t mtime, unsigned long order);

char *get_file_name_html(const char *fname_md);

void generate_html_file(const char *fname_md, const char *fname_html, unsigned long order);

char *read_line_from_file(FILE *stream);

char *get_title(const char *fname_md);

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "ru_RU.utf8");
    force = 0;
    int i = 0;
    for (i = 0; i <argc; ++i) {
        if (!strcmp(argv[i], "force")) {
            force = 1;
        }
    }

    // Буфер в который будет помещен путь
    // к текущей директории
    char path_current[MAX_PATH_LEN];
    // Переменная, в которую буднт помещен указатель на PathName
    char *pn = NULL;

    // Определяем путь к текущей директории
    pn = getcwd(path_current, MAX_PATH_LEN);

    // Вывод результата на консоль
    if (pn == NULL) {
        fprintf(stderr, "Ошибка определения пути\n");
    }

    char *out = get_git_path(path_current);
    parse_dir(out, 0);

    free(out);

    return 0;
}

char *get_git_path(const char *path)
{
    char *dirc_orig = strdup(path);
    char *basec_orig = strdup(path);

    char *dirc = dirc_orig;
    char *dname = dirname(dirc);

    char *basec = basec_orig;
    char *bname = basename(basec);

    char *path_out = NULL;
    if (strcmp(bname, "md2html") != 0) {
        path_out = strdup(path);
    }
    else {
        path_out = strdup(dname);
    }

    free(dirc_orig);
    free(basec_orig);

    return path_out;
}

void parse_dir(const char *path, const unsigned long order)
{
    //g_print("\nКаталог = %s, order = %d\n\n", path, order);

    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat st;

    dir = opendir(path);

    if (dir == NULL) {
        fprintf(stderr, "opendir() error for %s\n", path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        if ((order == 0) &&
            (!strcmp(entry->d_name, "md2html") ||
            !strcmp(entry->d_name, "css") ||
            !strcmp(entry->d_name, "js") ||
            !strcmp(entry->d_name, "fonts"))) {
            continue;
        }

        if (!strcmp(entry->d_name, "README.md")) {
            continue;
        }

        gchar *ffname = g_strconcat(path, "/", entry->d_name, NULL);

        if (lstat(ffname, &st) == -1) {
            fprintf(stderr, "lstat() error for %s\n", ffname);
            g_free(ffname);
            ffname = NULL;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            parse_dir(ffname, (order + 1));
        }
        else if (S_ISREG(st.st_mode)) {
            if (if_file_name_md(entry->d_name)) {
                //g_print("\nProcess file = %s\n", ffname);
                process_file_md(ffname, st.st_mtime, order);
            }
        }

        g_free(ffname);
        ffname = NULL;
    }

    closedir(dir);
}

gboolean if_file_name_md(const char *fname)
{
    unsigned long len = strlen(fname);
    if (fname[len - 3] == '.' && fname[len - 2] == 'm' && fname[len - 1] == 'd') {
        return TRUE;
    }
    return FALSE;
}

char *get_file_name_html(const char *fname_md)
{
    unsigned long len = strlen(fname_md) - 2;
    unsigned long len_new = len + 5;
    char *fname_html = calloc(len_new, sizeof(char));
    fname_html = strncpy(fname_html, fname_md, len);
    fname_html = strncat(fname_html, "html", 4);

    return fname_html;
}

void process_file_md(const char *fname_md, const time_t mtime, const unsigned long order)
{
    //g_print("md Файл = %s\n", fname_md);
    //g_print("Дата и время изменения файла = %ld\n", mtime);

    gboolean i_create_html = FALSE;

    char *fname_html = get_file_name_html(fname_md);

    //g_print("html Файл = %s\n", fname_html);

    if (g_file_test(fname_html, G_FILE_TEST_IS_REGULAR)) {
        struct stat st;
        if (lstat(fname_html, &st) == -1) {
            fprintf(stderr, "lstat() error for %s\n", fname_html);
            i_create_html = FALSE;
        }
        else if (force == 1 || st.st_mtime < mtime) {
            i_create_html = TRUE;
        }
    }
    else if (g_file_test(fname_html, G_FILE_TEST_IS_DIR)) {
        fprintf(stderr, "Error: %s is a directory!\n", fname_html);
        i_create_html = FALSE;
    }
    else {
        fprintf(stderr, "Error: %s has unknow type!\n", fname_html);
        i_create_html = FALSE;
    }

    if (i_create_html) {
        g_print("Создаётся файл: %s\n", fname_html);
        generate_html_file(fname_md, fname_html, order);
    }

    free(fname_html);
}

void generate_html_file(const char *fname_md, const char *fname_html, const unsigned long order)
{
    FILE *file_html = NULL;

    file_html = fopen(fname_html, "w");

    if (file_html == NULL) {
        fprintf(stderr, "First fopen() error for %s\n", fname_html);
        return;
    }

    char *title = NULL;
    title = get_title(fname_md);
    if (title == NULL) {
        return;
    }

    unsigned long len = order * 3 + 1;
    char *lpath = calloc(len, sizeof(char));
    for (unsigned long i = 0; i < order; i++) {
        lpath = strncat(lpath, "../", 3);
    }
    lpath[len - 1] = '\0';

    fprintf(file_html, "<!DOCTYPE html>\n");
    fprintf(file_html, "<html lang=\"ru\">\n");
    fprintf(file_html, "  <head>\n");
    fprintf(file_html, "    <meta charset=\"utf-8\">\n");
    fprintf(file_html, "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n");
    fprintf(file_html, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\n");

    if (title != NULL) {
        fprintf(file_html, "    <title>%s</title>\n\n", title);
    }

    fprintf(file_html, "    <link href=\"%sfavicon.ico\" rel=\"shortcut icon\" type=\"image/vnd.microsoft.icon\" />\n\n",  lpath);

    fprintf(file_html, "    <link href=\"%scss/bootstrap.css\" rel=\"stylesheet\">\n", lpath);
    fprintf(file_html, "    <link href=\"%scss/librebay.css\" rel=\"stylesheet\">\n\n", lpath);

    fprintf(file_html, "    <link href=\"%scss/highlight.css\" rel=\"stylesheet\">\n", lpath);
    fprintf(file_html, "    <script src=\"%sjs/highlight.pack.js\"></script>\n", lpath);
    fprintf(file_html, "    <script>hljs.initHighlightingOnLoad();</script>\n\n");

    fprintf(file_html, "  </head>\n");
    fprintf(file_html, "  <body>\n\n");

    fprintf(file_html, "    <div class=\"container\" role=\"main\">\n");
    fprintf(file_html, "      <article class=\"content\">\n\n\n\n");

    fclose(file_html);

    gchar *command = g_strconcat("multimarkdown --to=html ", fname_md, " >> ", fname_html, "\n", NULL);
    system(command);
    g_free(command);

    file_html = fopen(fname_html, "a");

    if (file_html == NULL) {
        fprintf(stderr, "Second fopen() error for %s\n", fname_html);
        return;
    }

    fprintf(file_html, "\n\n\n      </article>\n");
    fprintf(file_html, "    </div>\n");
    fprintf(file_html, "  </body>\n");
    fprintf(file_html, "</html>\n");
    fclose(file_html);

    free(lpath);
}

char *read_line_from_file(FILE *stream)
{

    unsigned int N = 256;
    unsigned int delta = 256;
    unsigned int i = 0;
    char *buf = (char *)malloc(sizeof(char) * N);
    buf[i] = getc(stream);
    while (buf[i] != EOF && buf[i] != '\n') {
        ++i;
        if (i >= N) {
            N += delta;
            buf = (char *)realloc(buf, sizeof(char) * N);
        }
        buf[i] = getc(stream);
    }
    buf[i] = '\0';
    return buf;
}

char *get_title(const char *fname_md)
{
    FILE *file = NULL;
    file = fopen(fname_md, "r");

    if (file == NULL) {
        fprintf(stderr, "fopen() error for %s\n", fname_md);
        return NULL;
    }

    char *line = NULL;
    char *line_orig = NULL;
    char *title = NULL;

    while (!feof(file)) {
        if (line_orig != NULL) {
            free(line_orig);
            line_orig = NULL;
        }

        line_orig = read_line_from_file(file);
        line = line_orig;

        if (strlen(line) >= 3 && (line[0] == '#' && (line[1] == ' ' || line[1] == '\t'))) {
            line[0] = ' ';
            line = g_strchomp(line);
            unsigned long len = strlen(line);
            if (line[len - 1] == '#') {
                line[len - 1] = ' ';
            }

            unsigned long i = 0;
            int i_cut = 0;
            for (i = 0; i < len; i++) {
                if (line[i] == '[') {
                    i_cut = 1;
                }
                if (line[i] == ']') {
                    line[i] = ' ';
                    i_cut = 0;
                }
                if (i_cut == 1) {
                    line[i] = ' ';
                }
            }

            line = g_strchug(line);
            line = g_strchomp(line);

            if (strlen(line) > 0) {
                title = strdup(line);
            }
            break;
        }
    }

    if (line_orig != NULL) {
        free(line_orig);
        line_orig = NULL;
    }

    if (title == NULL) {
        title = strdup("Title");
    }

    return title;
}
