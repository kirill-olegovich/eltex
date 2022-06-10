#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_start(int n, struct dirent **dirlist, WINDOW *win) {
	int l = 1;

	box(win, 0, 0);

    for (int i=0; i<n; i++) {
		if (l == 1) wattron( win, A_STANDOUT );
		else wattroff( win, A_STANDOUT );
	    mvwprintw(win, l, 1, "%s", dirlist[i]->d_name);
	    l++;
    }
}


void free_dirlist(int n, struct dirent **dirlist) {
	while (n--) free(dirlist[n]);
}


void path_handler(char path[100], char str[20]) {
	char temp_str[100];
	int index_l = -1, index_r = -1, count = 0;

	memset(temp_str, 0, 100);

	for (int i=99; i>-1; i--) {
		if (path[i] == '/') {
			count++;
			if (count == 1) index_r = i;
			if (count == 2) {
				index_l = i;
				break;
			}
		}
	}

	if (strcmp(str, "..") == 0) {
		// printf("%d %d\n\n\n", index_l, index_r);

		if (index_l != -1) {
			strncpy(temp_str, &path[index_l+1], (index_r - index_l) - 1);
			
			if (strcmp(temp_str, "..") != 0) {
				count = 0;

				for (int i=99; i>-1; i--) {
					if (path[i] == '/') count++;
					if (count == 2) {
						path[i+1] = '\0';
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
		strncpy(temp_str, path, index_l+1);

		// printf("\ntemp_str - %s\n", temp_str);

		realpath(temp_str, temp_path);

		// printf("\nabs path - %s\n", temp_path);

		for (int i=0; temp_path[i] != '\0'; i++) {
			if (temp_path[i] == '/') index_l = i;
		}

		// printf("\nindex_l2 - %d\n", index_l);

		strncpy(temp_str, &temp_path[index_l+1], 99);

		// printf("\nfolder name - %s\n", temp_str);

		if (strcmp(temp_str, str) == 0) {
			count = 0;

			for (int i=99; i>-1; i--) {
				if (path[i] == '/') count++;
				if (count == 2) {
					path[i+1] = '\0';
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
	initscr();
	
	int height, width, y, x;
	struct dirent **namelist;
    int dir1, ch, offset = 0;
    char path1[100];
    memset(path1, 0, 100);

	height = 43;
	width = 183;
	y = x = 0;

	WINDOW *win = newwin(height, width, y, x);
	refresh();

	noecho();
	keypad(win, TRUE);
	curs_set(0);

	dir1 = scandir(".", &namelist, NULL, alphasort);

	print_start(dir1, namelist, win);

    while((ch = wgetch(win)) != 'q') {
        mvwprintw(win, offset+1, 1, "%s", namelist[offset]->d_name);

		switch( ch ) {
			case 'w':
				offset--;
				offset = (offset < 0) ? dir1-1 : offset;
				break;
			case 's':
				offset++;
				offset = (offset > dir1-1) ? 0 : offset;
				break;
			case 'o':;
				char temp_str[100];
				strcpy(temp_str, namelist[offset]->d_name);

				free_dirlist(dir1, namelist);

				path_handler(path1, temp_str);

				dir1 = scandir(path1, &namelist, NULL, alphasort);

			    if (dir1 == -1) {
			    	wclear(win);
			    	mvwprintw(win, 1, 1, "Cannot open dir");
					getch();
					delwin(win);
					endwin();
			        return 1;
			    }

				wclear(win);
				print_start(dir1, namelist, win);
				offset = 0;
				break;
		}

		wattron( win, A_STANDOUT );
        mvwprintw(win, offset+1, 1, "%s", namelist[offset]->d_name);
		wattroff( win, A_STANDOUT );
    }

    free_dirlist(dir1, namelist);
    free(namelist);

    delwin(win);
	endwin();



	return 0;
}