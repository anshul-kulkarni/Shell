#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <sys/stat.h>
#define MAX 128

char cmd[MAX];
char cwd[MAX];
char temp[MAX][MAX];
pid_t pid;
char PATH[MAX] = "/bin/";
char *username;
char input_print[MAX];
int count;

void set_prompt () {
	strcpy (input_print, username);
	strcat (input_print, ":~");
	strcat (input_print, getcwd (cwd, sizeof (cwd)));
}

void handle_input () {
	printf ("%s\n", input_print);
	char *input = NULL;
	input = readline ("");
	if (input == NULL || strcmp (input, "exit") == 0) {
		printf ("Goodbye\n");
		exit (1);
	}
	add_history (input);
	strcpy (cmd, input);
	count = 0;
}

int prompt_helper (char *t) {
	int sum = 0;
	for (int i = 0; t[i] != '\0'; i++) {
		int d = t[i];
		sum += d;
	}
	return sum;
}

void change_prompt () {
	char *t = strtok (cmd, "=");
	t = strtok (NULL, "=");
	if (prompt_helper (t) == 315) {
		set_prompt ();
	}
	else {
		strcpy (input_print, t);
	}
}

void set_path () {
	char *t = strtok (cmd, "=");
	t = strtok (NULL, "=");
	strcpy (PATH, t);
}

int handle_spaces () {
	char *t = strtok (cmd, " ");
	strcpy (temp[0], t);
	count++;
	t = strtok (NULL, " ");
	if (t == NULL) {
		return 0;
	}
	for (int i = 1; t != NULL; i++) {
		strcpy (temp[i], t);
		count++;
		t = strtok (NULL, " ");
	}
	return -1;
}

int check_prompt () {
	char *t = strtok (input_print, "~");
	t = strtok (NULL, "~");
	if (strcmp (t, getcwd (cwd, sizeof (cwd))) == 0) {
		return 1;
	}
	return 0;
}

void process_single () {
	char *ptr[] = {cmd, NULL};
	if (execv (cmd, ptr) == -1) {
		char c[MAX];
		strcpy (c, PATH);
		strcat (c, cmd);
		ptr[0] = c;
		if (execv (c, ptr) == -1) {
			perror ("error");
		}	
	}
}

void process_multiple () {
	if (strcmp (temp[0], "cd") == 0) {
		if (check_prompt () == 1) {
			chdir (temp[1]);
			set_prompt ();
		}
		else {
			chdir (temp[1]);
		}
		return ;
	}
	char *ptr[] = {temp[0], NULL, NULL, NULL, NULL, NULL};
	for (int i = 1; i < count; i++) {
		ptr[i] = temp[i];
	}
	if (execv (temp[0], ptr) == -1) {
		char c[MAX];
		strcpy (c, PATH);
		strcat (c, temp[0]);
		ptr[0] = c;
		if (execv (c, ptr) == -1) {
			perror ("error");
		}
	}
}

int main () {
	username = getenv("USER");
	set_prompt ();
	while (1) {
		handle_input ();
		if (strstr (cmd, "PATH") != NULL) {
			set_path ();
		}
		else if (strcmp (cmd, "cd") == 0) {
			char cd[MAX];
			strcpy (cd, "/home/");
			strcat (cd, username);
			chdir (cd);
			set_prompt ();	
		}
		else if (strstr (cmd, "PS1") != NULL) {
			change_prompt ();
		}
		else {
			pid = fork ();
			if (pid == 0) {
				if (handle_spaces () == 0) {
					process_single ();
				}
				else {
					process_multiple ();
				}
			}
			else {
				wait (NULL);
			}
		}
	}
}
