#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

// void childterm(){
// 	wait(NULL);
// 	printf("Shell: Background process terminated\n");
// }

int chi;

void funct()
{
	// printf("pid: %d has group id: %d \n", getpid(), getpgid(getpid()));
	// printf("%d", chi);
	if (chi > 0)
	{
		kill(chi, SIGTERM);
	}
}

int main(int argc, char *argv[])
{
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;
	int back = 0;
	int foregroundgroup;
	int backgroundgroup;

	while (1)
	{

		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; // terminate with new line
		tokens = tokenize(line);

		// do whatever you want with the commands, here we just print them
		int parentid = getpid();
		if (tokens[0] == NULL)
		{
			continue;
		}
		int j = 0;
		int status;
		int childid;
		// if (back == 1)
		// {
		// 	if (setpgid(childid, parentid) < 0)
		// 	{
		// 		printf("could'nt change group id");
		// 	}
		// }
		back = 0;
		while (waitpid(-1, &status, WNOHANG) > 0)
		{
			printf("Shell: Background process terminated\n");
		}
		for (j = 0; tokens[j] != NULL; j++)
			;
		if (strcmp("&", tokens[j - 1]) == 0)
		{
			back = 1;
			tokens[j - 1] = NULL;
		}
		if (strcmp("cd", tokens[0]) == 0)
		{
			if (strcmp("..", tokens[1]) == 0)
			{
				chdir("..");
			}
			else
			{
				if (tokens[2] == NULL)
				{
					if (chdir(tokens[1]) != 0)
					{
						printf("Shell:Incorrect command\n");
					}
				}
				else
				{
					printf("Please enter only one argument\n");
				}
			}
			continue;
		}
		if (tokens[0] != NULL)
		{

			childid = fork();
			int childstatus;
			if (childid == 0)
			{
				execvp(tokens[0], tokens);
				printf("Command not Found \n");
				exit(1);
			}

			else if (childid < 0)
			{
				printf("Some Error Occured due to unknown reason\n");
				continue;
			}
			else
			{
				if (back == 0)
				{
					if (setpgid(childid, foregroundgroup) < 0)
						setpgid(childid, childid);
					foregroundgroup = childid;
				}
				else
				{
					if (setpgid(childid, backgroundgroup) < 0)
					{
						setpgid(childid, childid);
						backgroundgroup = childid;
					}
				}
				if (strcmp("exit", tokens[0]) == 0)
				{
					kill(-foregroundgroup, SIGTERM);
					kill(-backgroundgroup, SIGTERM);
					kill(0, SIGTERM);
				}
				// if ( setpgid( getpid() ,0 ) == -1 ){
				// 	printf("cannot change group");
				// }
				//printf("pid: %d has group id: %d \n", childid, getpgid(childid));
				if (back == 1)
				{
					waitpid(childid, &status, WNOHANG);
					// signal(SIGCHLD,childterm);
				}
				else
				{
					int st;
					chi = foregroundgroup;
					signal(SIGINT, funct);
					waitpid(childid, &st, 0);
					back = 0;
				}
			}
		}

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}
