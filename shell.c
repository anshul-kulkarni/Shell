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

char cmd[MAX], cwd[MAX];
char temp[MAX][MAX];
pid_t pid;
int flag = -1;
char PATH[MAX] = "/bin/";
char input_print[MAX];
int count;
char buf[MAX];

void set_prompt () {
        getlogin_r(buf,10);
        strcpy (input_print, buf);
        strcat (input_print, ":~");
        strcat (input_print, getcwd (cwd, sizeof (cwd)));
}

void handle_input () {
	printf ("%s\n", input_print);
	char *input = NULL;
        input = readline("");
	if (input == NULL) {
		exit (1);
	}
	if (strcmp (input, "exit") == 0) {
		printf ("Goodbye\n");
		exit (0);
	}
	add_history (input);
	strcpy (cmd, input);
	count = 0;
}

void change_prompt () {
         if (strstr (cmd, "PS1") != NULL) {
                 char *t = strtok (cmd, "=");
                 t = strtok (NULL, "=");
                 strcpy (input_print, t);
                 handle_input ();
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
	int i = 1;
	while (t != NULL) {
		strcpy (temp[i], t);
		i++;
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

void process_multiple () {
	if (strcmp (temp[0], "cd") == 0) {
		char *t;
		if (check_prompt () == 1) {
			chdir (temp[1]);
			set_prompt ();
		}
		else {
			chdir (temp[1]);	
		}
		return;
	}
	char c[MAX];
	strcpy (c, PATH);
	strcat (c, temp[0]);
	char *ptr[] = {c, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	for (int i = 1; i < count; i++) {
		ptr[i] = temp[i];
	}
	if (execv (c, ptr) == -1) {
		printf ("command not found");
	}
}

int main () {
	set_prompt ();
	while (1) {
		handle_input ();
		if (strstr (cmd, "PATH") != NULL) {
			set_path ();
		}	
		else if (strcmp (cmd, "cd") == 0) {
			chdir ("/home");
			set_prompt ();
		}
		else if (strstr (cmd, "PS1") != NULL) {
			change_prompt ();
			continue;
		}
		else {
			pid = fork ();
			if (pid == 0) {
				if (handle_spaces() == 0) {
					int ret = execl (cmd, cmd, NULL);
			        	if (ret == -1) {
                				char start[10];
						strcpy (start, PATH);
                				strcat (start, cmd);
                				int try = execl (start, start, NULL);
                				if (try == -1) {
                        				printf ("%s: command not found\n", cmd);
                				}
        				}

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
