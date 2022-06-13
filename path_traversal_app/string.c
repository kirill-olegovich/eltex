#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void path_handler(char path[100], char str[20]) {
    char temp_str[100];
    int index_l = -1, index_r = -1, count = 0;

    memset(temp_str, 0, 100);

    for (int i = 99; i > -1; i--) {
        if (path[i] == '/') {
            count++;
            if (count == 1)
                index_r = i;
            if (count == 2) {
                index_l = i;
                break;
            }
        }
    }

    if (strcmp(str, "..") == 0) {
        // printf("%d %d\n\n\n", index_l, index_r);

        if (index_l != -1) {
            strncpy(temp_str, &path[index_l + 1], (index_r - index_l) - 1);

            if (strcmp(temp_str, "..") != 0) {
                count = 0;

                for (int i = 99; i > -1; i--) {
                    if (path[i] == '/')
                        count++;
                    if (count == 2) {
                        path[i + 1] = '\0';
                        break;
                    }

                    path[i] = 0;
                }

            } else {
                strcat(path, str);
                strcat(path, "/");
            }

        } else {
            strcat(path, str);
            strcat(path, "/");
        }

    } else {
        char temp_path[100];

        // printf("\nlocal path - %s\n", path);
        // printf("\nindex_l - %d\n", index_l);

        memset(temp_path, 0, 100);
        strncpy(temp_str, path, index_l + 1);

        // printf("\ntemp_str - %s\n", temp_str);

        realpath(temp_str, temp_path);

        // printf("\nabs path - %s\n", temp_path);

        for (int i = 0; temp_path[i] != '\0'; i++) {
            if (temp_path[i] == '/')
                index_l = i;
        }

        // printf("\nindex_l2 - %d\n", index_l);

        strncpy(temp_str, &temp_path[index_l + 1], 99);

        // printf("\nfolder name - %s\n", temp_str);

        if (strcmp(temp_str, str) == 0) {
            count = 0;

            for (int i = 99; i > -1; i--) {
                if (path[i] == '/')
                    count++;
                if (count == 2) {
                    path[i + 1] = '\0';
                    break;
                }

                path[i] = 0;
            }

        } else {
            strcat(path, str);
            strcat(path, "/");
        }
    }
}

int main(void) {
    char input[20];
    char path[100];

    memset(path, 0, 100);
    memset(input, 0, 20);

    strcat(path, "./");

    while (1) {
        fgets(input, 20, stdin);
        input[strcspn(input, "\n")] = 0;
        path_handler(path, input);
        printf("\n%s\n", path);
    }

    return 0;
}