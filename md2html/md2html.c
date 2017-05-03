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

#define MAX_PATH_LEN 1024

char *get_git_path(const char *path);

void parse_dir(const char *path, const int order);

gboolean if_file_name_md(const char *fname);

void process_file_md(const char *fname_md, const time_t mtime, const int order);

char *get_file_name_html(const char *fname_md);

void generate_html_file(const char *fname_md, const char *fname_html, const int order);

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "ru_RU.utf8");

    // Буфер в который будет помещен путь
    // к текущей директории
    char path_current[MAX_PATH_LEN];
    // Переменная, в которую буднт помещен указатель на PathName
    char *pn;

    // Определяем путь к текущей директории
    pn = getcwd(path_current, MAX_PATH_LEN);

    // Вывод результата на консоль
    if (pn == NULL) {
        printf("Ошибка определения пути\n");
    }
    else {
        printf("Текущая директория: %s\n", path_current);
    }

    char *out = get_git_path(path_current);

    printf("dirname=%s\n", out);

    parse_dir(out, 0);

    return 0;
}

char *get_git_path(const char *path)
{
    char *dirc, *dirc_orig, *dname;
    char *basec, *basec_orig, *bname;
    char *path_out = NULL;

    dirc_orig = dirc = strdup(path);
    basec_orig = basec = strdup(path);

    dname = dirname(dirc);
    bname = basename(basec);

    if (strcmp(bname, "md2html")) {
        path_out = strdup(path);
    }
    else {
        path_out = strdup(dname);
    }

    free(dirc_orig);
    free(basec_orig);

    return path_out;
}

void parse_dir(const char *path, const int order)
{

    g_print("\nКаталог = %s, order = %d\n\n", path, order);

    DIR *dir;
    struct dirent *entry;
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
        if (!strcmp(entry->d_name, "md2html") && order == 0) {
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
                process_file_md(ffname, st.st_mtime, order);
            }
        }

        g_free(ffname);
        ffname = NULL;
    }

    closedir(dir);

    return;
}

gboolean if_file_name_md(const char *fname)
{
    int len = strlen(fname);
    if (fname[len - 3] == '.' && fname[len - 2] == 'm' && fname[len - 1] == 'd') {
        return TRUE;
    }
    return FALSE;
}

char *get_file_name_html(const char *fname_md)
{
    int len = strlen(fname_md) - 2;
    int len_new = len + 5;
    char *fname_html = calloc(len_new, sizeof(char));
    fname_html = strncpy(fname_html, fname_md, len);
    fname_html = strncat(fname_html, "html", 4);

    return fname_html;
}

void process_file_md(const char *fname_md, const time_t mtime, const int order)
{
    g_print("md Файл = %s\n", fname_md);
    g_print("Дата и время изменения файла = %ld\n", mtime);

    gboolean i_create_html = FALSE;

    char *fname_html = get_file_name_html(fname_md);

    g_print("html Файл = %s\n", fname_html);

    if (g_file_test(fname_html, G_FILE_TEST_IS_REGULAR)) {
        struct stat st;
        if (lstat(fname_html, &st) == -1) {
            fprintf(stderr, "lstat() error for %s\n", fname_html);
            i_create_html = FALSE;
        }
        else {
            if (st.st_mtime < mtime) {
                i_create_html = TRUE;
            }
        }
    }
    else if (g_file_test(fname_html, G_FILE_TEST_IS_DIR)) {
        fprintf(stderr, "Error: %s is a directory!\n", fname_html);
        i_create_html = FALSE;
    }
    else {
        i_create_html = TRUE;
    }

    if (i_create_html) {
        printf("Генерировать файл!\n");
        generate_html_file(fname_md, fname_html, order);
    }
    else {
        printf("Ничего не делаем!\n");
    }

    free(fname_html);

    return;
}

void generate_html_file(const char *fname_md, const char *fname_html, const int order)
{

    FILE *file_html;

    file_html = fopen(fname_html, "w");

    if (file_html == NULL) {
        fprintf(stderr, "First fopen() error for %s\n", fname_html);
        return;
    }

    fprintf(file_html, "<!DOCTYPE HTML>\n");
    fprintf(file_html, "<html>\n");
    fprintf(file_html, "  <head>\n");
    fprintf(file_html, "    <meta charset=\"utf-8\">\n");
    fprintf(file_html, "    <link rel=\"stylesheet\" href=\"/home/maksim/Projects/md2html/style.css\" />\n");

    fprintf(file_html, "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.7.0/styles/default.min.css\">\n");
    fprintf(file_html, "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.7.0/highlight.min.js\"></script>\n");
    fprintf(file_html, "<script>hljs.initHighlightingOnLoad();</script>\n");
    fprintf(file_html, "  </head>\n");
    fprintf(file_html, "  <body>\n");
    fprintf(file_html, "\n\n\n");

    fclose(file_html);

    char *command = calloc(1024, sizeof(char));

    sprintf(command, "multimarkdown %s >> %s\n", fname_md, fname_html);
    system(command);
    free(command);

    file_html = fopen(fname_html, "a");

    if (file_html == NULL) {
        fprintf(stderr, "Second fopen() error for %s\n", fname_html);
        return;
    }

    fprintf(file_html, "\n\n\n");

    fprintf(file_html, "  </body>\n");
    fprintf(file_html, "</html>\n");
    fclose(file_html);
}
