/*
 * SPDX-FileCopyrightText: 2024 Alain Emilia Anna Zscheile <fogti+devel@ytrizja.de>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef YHAS_SYS_DIRENT
# include <sys/dirent.h>
#endif
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void mmzx_run_on_dir(DIR *dir, const char *path) {
    mmzx_names_t names;
    names.names = 0;
    names.length = 0;
    names.capacity = 0;

    const size_t pathlen = strlen(path);
    const int cur_dirfd = dirfd(dir);

    struct dirent *ditem;
    const char *firstname = 0;
    size_t llcs = 0;

    while((ditem = readdir(dir))) {
        // skip hidden items
        if(!ditem->d_name[0] || ditem->d_name[0] == '.')
            continue;

        const size_t curnamlen = strlen(ditem->d_name);

        if(ditem->d_type == DT_DIR) {
            const int next_dirfd = openat(cur_dirfd, ditem->d_name, O_RDONLY | O_NDELAY | O_DIRECTORY | O_CLOEXEC
#ifdef YHAS_O_LARGEFILE
                | O_LARGEFILE
#endif
            );
            DIR *next_dir = fdopendir(next_dirfd);
            if(!next_dir) {
                if(next_dirfd >= 0) {
                    close(next_dirfd);
                }
                switch(errno) {
                    case ENOTDIR:
                        break;
                    case ENOENT:
                        continue;
                    default:
                        fprintf(stderr, "ERR: %s: unable to recurse into directory %s: %s\n", path, ditem->d_name, strerror(errno));
                        continue;
                }
            } else {
                // TODO: only allocate a single buffer for the path
                char *next_path = malloc(pathlen + curnamlen + 2);
                if(!next_path) {
                    fprintf(stderr, "ERROR: %s: OOM during recursion\n", path);
                    closedir(next_dir);
                    goto names_cleanup;
                }
                memcpy(next_path, path, pathlen);
                next_path[pathlen] = '/';
                strcpy(&next_path[pathlen + 1], ditem->d_name);
                mmzx_run_on_dir(next_dir, next_path);
                free(next_path);
                closedir(next_dir);
                continue;
            }
        }

        if(ditem->d_type != DT_REG && ditem->d_type != DT_LNK) {
            continue;
        }

        // we got a file
        const size_t extoffset = mmzx_find_ext(ditem->d_name);
        if(!extoffset) continue;

        char *const dup_name_new = strndup(ditem->d_name, curnamlen);
        if(!dup_name_new) {
            fprintf(stderr, "ERROR: %s: OOM during recursion\n", path);
            goto names_cleanup;
        }

        {
            char *const ext = &dup_name_new[extoffset];
            mmzx_normalize_ext(ext);
            if(!mmzx_has_known_ext(ext)) {
                free(dup_name_new);
                continue;
            }
        }

        mmzx_name_ent_t curent;
        curent.orig_name = 0;
        curent.name = dup_name_new;
        // find prefix to prune
        if(names.length > 0) {
            mmzx_update_llcs(&llcs, firstname, curent.name);
        } else {
            firstname = curent.name;
            llcs = curnamlen;
        }

        if(!strcmp(ditem->d_name, curent.name)) {
            // names are equal
            curent.orig_name = curent.name;
        } else {
            curent.orig_name = strndup(ditem->d_name, curnamlen);
            if(!curent.orig_name) goto curname_cleanup;
        }

        if(names.capacity <= names.length) {
            // allocate more
            if(!names.capacity)
                names.capacity = 64;
            else
                names.capacity <<= 1;
            mmzx_name_ent_t *newnames = reallocarray(names.names, names.capacity, sizeof(mmzx_name_ent_t));
            if(!newnames) goto curname_cleanup;
            names.names = newnames;
        }

        mmzx_copy_name_ent(&names.names[names.length], &curent);
        names.length++;
        continue;

        curname_cleanup:
            mmzx_deinit_name_ent(&curent);
            fprintf(stderr, "ERROR: %s: OOM during recursion\n", path);
            goto names_cleanup;
    }

    if(!names.names)
        goto names_cleanup;

    printf("%s:\n", path);

    // prune prefix
    for(size_t i = 0; i < names.length; ++i) {
        mmzx_name_ent_t *item = &names.names[i];
        const char *old_name = item->orig_name;
        // TODO: don't run chmod on symlinks (this could break links pointing to folders)
        if(-1 == fchmodat(cur_dirfd, old_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 0)) {
            switch(errno) {
                case ENOENT:
                    continue;
                default:
                    printf("\tCHM %s ERR %s\n", old_name, strerror(errno));
                    break;
            }
        }

        const char *new_name = &item->name[llcs];
        if(!strcmp(old_name, new_name) || !new_name[0]) continue;

        printf("\tMV %s -> %s; ", old_name, new_name);

        if(-1 == faccessat(cur_dirfd, new_name, W_OK, 0)) {
            switch(errno) {
                case EACCES:
                    printf("ERR: destination already exists, inaccessible\n");
                    continue;

                case ENOENT:
                    break;

                default:
                    break;
            }
            if(-1 == renameat(cur_dirfd, old_name, cur_dirfd, new_name)) {
                printf("ERR: rename failed; %s\n", strerror(errno));
            } else {
                printf("OK\n");
            }
        } else {
            printf("ERR: destination already exists\n");
        }
    }

  names_cleanup:
    mmzx_deinit_names(&names);
}

int main() {
    DIR *startdir = opendir(".");
    if(!startdir) {
        fprintf(stderr, "ERR: unable to recurse into directory .: %s\n", strerror(errno));
        return -1;
    }

    mmzx_run_on_dir(startdir, ".");

    closedir(startdir);
    return 0;
}
