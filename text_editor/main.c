#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	char buffer[255][255];
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("./data", "r");
	
	int i = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
		for (int j=0; j<len; j++) buffer[i][j] = line[j];
    }

    fclose(fp);

	for (i=0; i<255; i++) {
		for (int j=0; j<255; j++) printf("%c", buffer[i][j]);
		printf("\n");
	}

	int r = 0, c = 0;
	int input;

	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	move(0, 0);












	for (int i=0; i<count; i++) {
		mvprintw(r, c, "%c", buffer[i]);
		c++;
		if (buffer[i] == '\n') {
			r++;
			c = 0;
		}
	}

	refresh();
	move(0, 0);
	r = 0; c = 0;

	do {
		input = getch();

		if (input == KEY_F(1)) {
			break;
		} else if (input == KEY_UP) {
			r--;
		} else if (input == KEY_DOWN) {
			r++;
		} else if (input == KEY_RIGHT) {
			c++;
		} else if (input == KEY_LEFT) {
			c--;
		} else if (input == KEY_BACKSPACE) {
			input = ' ';
			mvprintw(r, c-1, "%c", input);
			c--;
		} else {
			mvprintw(r, c, "%c", input);
			c++;
		}
		move(r, c);
		refresh();
	} while (1);

	endwin();

	return 0;
}
