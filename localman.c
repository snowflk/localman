/*
MIT License
Copyright (c) 2019 Thien Phuc Tran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <regex.h>
#include <sys/stat.h>
#include <pwd.h>
#include <wordexp.h>


#define START_MARKER "### localman ###"
#define END_MARKER "### end localman ###"
#define _WORKDIR "~/.localman"
#define _BACKUP_FILENAME  _WORKDIR "/hosts.bak"
#define _SCRATCH_FILENAME _WORKDIR "/hosts"
#define _SCRATCH_FILENAME_EDITING _WORKDIR "/hosts.scratch"
#define HOSTS_PATH "/etc/hosts"
#define MAX_ENTRIES 255

wordexp_t exp_result;
char WORKDIR[255];
char BACKUP_FILENAME[255];
char SCRATCH_FILENAME[255];
char SCRATCH_FILENAME_EDITING[255];

void resolve_path() {
    char* paths[] = {WORKDIR, BACKUP_FILENAME, SCRATCH_FILENAME, SCRATCH_FILENAME_EDITING};
    char* _paths[] = {_WORKDIR, _BACKUP_FILENAME, _SCRATCH_FILENAME, _SCRATCH_FILENAME_EDITING};
    for (int i = 0; i < 4; i++){
        wordexp(_paths[i], &exp_result, 0);
        snprintf(paths[i], sizeof(WORKDIR), "%s", exp_result.we_wordv[0]);
    }
}

void get_current_table(char **entries) {
    bool is_in_section = false;
    int current_table_length = 0;

    FILE *file = fopen(SCRATCH_FILENAME, "r");
    if (file != NULL) {
        char line[255];
        while (fgets(line, sizeof line, file) != NULL) {
            line[strlen(line) - 1] = '\0';
            if (strcmp(line, START_MARKER) == 0) {
                is_in_section = true;
            } else if (strcmp(line, END_MARKER) == 0) {
                is_in_section = false;
            } else if (is_in_section) {
                strtok(line, " \t");
                char *entry = strtok(NULL, " \t");
                if (current_table_length == 0 ||
                    (current_table_length > 0 && strcmp(entries[current_table_length - 1], entry) != 0)) {
                    entries[current_table_length] = malloc(255);
                    strcpy(entries[current_table_length], entry);
                    current_table_length++;
                }

            }
        }
        // mark end of array
        entries[current_table_length] = NULL;
        fclose(file);
    } else {
        perror("Error: ");
    }
}

void show_current_table() {
    char **entries = malloc(MAX_ENTRIES);
    get_current_table(entries);
    char *entry;
    int i = 0;

    puts("┌────────────────────────────┐");
    puts("│    Current local domains   │");
    puts("├────────────────────────────┤");

    while ((entry = entries[i]) != NULL) {
        printf("│ %-26s │\n", entry);
        i++;
    }
    puts("└────────────────────────────┘");


    free(entries);
}

void run_cp(char *from, char *to, bool root_access) {
    char cmd[255];
    char sudo[10] = "";
    if (root_access){
        strcpy(sudo, "sudo ");
    }
    snprintf(cmd, sizeof(cmd), "%scp %s %s", sudo,from, to);
    system(cmd);
}

void replace_realhosts() {
    run_cp(SCRATCH_FILENAME, HOSTS_PATH, true);
}

void restore_realhosts() {
    run_cp(BACKUP_FILENAME, HOSTS_PATH, true);
}

void delete_file(char *f) {
    char cmd[255];
    snprintf(cmd, sizeof(cmd), "rm %s", f);
    system(cmd);
}

void persist(char **entries, char *exclude) {
    FILE *origin = fopen(SCRATCH_FILENAME, "r");
    FILE *newfile = fopen(SCRATCH_FILENAME_EDITING, "w");
    bool is_in_section = false;

    if (origin != NULL) {
        char line[255];
        while (fgets(line, sizeof line, origin) != NULL) {
            line[strlen(line) - 1] = '\0';
            if (strcmp(line, START_MARKER) == 0) {
                is_in_section = true;
            } else if (strcmp(line, END_MARKER) == 0) {
                is_in_section = false;
            } else if (!is_in_section) {
                fprintf(newfile, "%s\n", line);
            }
        }

        fprintf(newfile, "%s\n", START_MARKER);

        int idx = 0;
        while (entries[idx] != NULL) {
            if (exclude == NULL || strcmp(entries[idx], exclude)) {
                fprintf(newfile, "127.0.0.1\t%s\n", entries[idx]);
                fprintf(newfile, "::1\t%s\n", entries[idx]);
            }
            idx++;
        }

        fprintf(newfile, "%s\n", END_MARKER);
        fclose(origin);
        fclose(newfile);

        run_cp(SCRATCH_FILENAME_EDITING, SCRATCH_FILENAME, false);
        delete_file(SCRATCH_FILENAME_EDITING);
    } else {
        perror("Error: ");
    }
}

void copy_hosts_if_not_exist(char *dest) {
    if (access(dest, F_OK) == -1) {
        char cmd[255];
        snprintf(cmd, sizeof(cmd), "cp %s %s", HOSTS_PATH, dest);
        system(cmd);

    }
}

void backup_if_needed() {
    // copy hosts file as backup if not exist
    copy_hosts_if_not_exist(BACKUP_FILENAME);
}

void add_domain(char *new_domain) {
    backup_if_needed();

    char **entries = malloc(MAX_ENTRIES);
    get_current_table(entries);
    int current_table_length = 0;
    while (entries[current_table_length] != NULL) {
        if (strcmp(entries[current_table_length], new_domain) == 0) {
            puts("This domain already exists");
            return;
        }
        current_table_length++;
    }
    entries[current_table_length++] = new_domain;
    entries[current_table_length] = NULL;
    persist(entries, NULL);
}

void remove_domain(char *old_domain) {
    backup_if_needed();

    char **entries = malloc(MAX_ENTRIES);
    get_current_table(entries);
    persist(entries, old_domain);
}

void show_help() {
    puts("Usage: localman COMMAND [PARAM]");
    puts("Commands:");
    printf("%-20s%s\n", "ls", "List all custom domains");
    printf("%-20s%s\n", "add <domain>", "Add a domain pointing to localhost");
    printf("%-20s%s\n", "rm <domain>", "Remove a domain");
    printf("%-20s%s\n", "apply", "Apply changes (require root access)");
    printf("%-20s%s\n", "revert", "Restore original hosts file (require root access)");
}

void create_scratch_if_needed() {
    if (access(WORKDIR, F_OK) == -1) {
        char cmd[255];
        snprintf(cmd, sizeof(cmd), "mkdir %s", WORKDIR);
        system(cmd);
    }
    copy_hosts_if_not_exist(SCRATCH_FILENAME);
}

int main(int argc, char *argv[]) {
    resolve_path();

    create_scratch_if_needed();

    if (argc < 2) {
        show_help();
    } else if (!strcmp(argv[1], "ls")) {
        show_current_table();
    } else if (!strcmp(argv[1], "apply")) {
        replace_realhosts();
    } else if (!strcmp(argv[1], "revert")) {
        restore_realhosts();
    } else if (!strcmp(argv[1], "add") && argc > 2) {

        add_domain(argv[2]);
    } else if (!strcmp(argv[1], "rm") && argc > 2) {
        remove_domain(argv[2]);
    } else {
        show_help();
    }
    return 0;
}