#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef struct t_shell
{
	char 	**cmd;
	int		id;
	int		pipe_before;
}	t_shell;

int nb_cmd(int argc, char **argv)
{
	int i = 0;
	int len = 1;

	while (argv[i])
	{
		if((strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0) && argv[i + 1])
			len++;
		i++;
	}
	printf("len -> %d\n", len);
	return(len);
}


int	alloc_cmd(t_shell *shell, char **argv, int index, int i)
{
	int len = 0;
	while (strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0 && argv[i + 1])
	{
		i++;
		len++;
	}
	if(!argv[i + 1])
		len++;
	printf("len_char -> %d\n", len);
	return(len);
}

void store_in_struct(t_shell *shell, char **argv)
{
	int i = 1;
	int index = 0;
	int i_ = 0;

	while (argv[i])
	{
		if((strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0) && argv[i + 1])
			i++;
		else
		{
			shell[index].cmd = malloc(sizeof(char *) * alloc_cmd(shell, argv, index, i) + 1);
			if(strcmp(argv[i - 1], "|") == 0)
				shell[index].pipe_before = 1;
			else
				shell[index].pipe_before = 0;
			shell[index].id = 0;
			i_ = 0;
			while (strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0 && argv[i + 1])
			{
				shell[index].cmd[i_] = strdup(argv[i]);
				i++;
				i_++;
			}
			if(strcmp(argv[i], "|") == 0)
				shell[index].id = 1;
			else if(strcmp(argv[i], ";") == 0)
				shell[index].id = 2;
			if(strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0 && !argv[i + 1])
				shell[index].cmd[i_++] = strdup(argv[i]);
			shell[index].cmd[i_] = NULL;
			i++;
			index++;
		}
	}
	shell[index].cmd = NULL;
}

void print_all(t_shell *shell)
{
	int i = 0;
	int i_ = 0;

	while (shell[i].cmd != NULL)
	{
		i_ = 0;
		printf("------------------------\n");
		while (shell[i].cmd[i_])
		{
			printf("shell[%d].cmd[%d] -> %s\n", i, i_, shell[i].cmd[i_]);
			printf("shell[%d].id -> %d\n", i, shell[i].id);
			printf("shell[%d].pipe_before -> %d\n", i, shell[i].pipe_before);
			i_++;
		}
		i++;
	}
}
/*
void exec_dot_come(char **cmd, )
{



}
*/

void exec(t_shell *shell, char **envp)
{
	int i = 0;
	int i_ = 0;

	while (shell[i].cmd != NULL)
	{
		i_ = 0;
		if(shell[i].id == 0)
			execve(shell[i].cmd[0], shell[i].cmd, envp);
		else if(shell[i].id == 2);
			exec_dot_coma(shell.cmd);
		// exec basique.

	}
}

int main(int argc, char **argv, char **envp)
{
	t_shell *shell;

	shell = malloc(sizeof(t_shell) * nb_cmd(argc, argv) + 1); 
	// Etape 1. Stockage && identification.
	store_in_struct(shell, argv);
	//print_all(shell);
	// Etape 3. Execution.
	exec(shell, envp);
}