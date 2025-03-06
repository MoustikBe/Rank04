#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

typedef struct t_shell
{
	char **cmd;
	int id;
	int prev_fd;
	int pipe_before;
}	t_shell;

int ft_strlen(char *str)
{
	int i = 0;

	while (str[i])
		i++;
	return(i);
}

int mem_struct(char **argv)
{
	int i = 1;
	int len = 1;

	while (argv[i])
	{
		if((strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0) && argv[i + 1])
			len++;
		i++;
	}
	printf("alloc memory -> %d\n", len);
	return(len);
}


int nb_cmd(char **argv, int i)
{
	int len = 0;

	while (strcmp(argv[i], "|") && strcmp(argv[i], ";") && argv[i + 1])
	{
		len++;
		i++;
	}
	if(!argv[i + 1])
		len++;
	printf("char -> %d\n", len);
	return(len);
}

void mem_set(t_shell *shell, char **argv)
{
	int i = 1;
	int i_ = 0;
	int index = 0;


	while (argv[i])
	{
		if((strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0) && argv[i + 1])
			i++;
		else 
		{
			shell[index].cmd = malloc(sizeof(char *) * nb_cmd(argv, i));
			i_ = 0;
			if(strcmp(argv[i - 1], "|") == 0)
				shell[index].pipe_before = 1;
			else 
				shell[index].pipe_before = 0;
			while (strcmp(argv[i], "|") && strcmp(argv[i], ";") && argv[i + 1])
			{
				shell[index].cmd[i_] = strdup(argv[i]);
				i++;
				i_++;
			}
			if(strcmp(argv[i], "|") == 0)
				shell[index].id = 1;
			else 
				shell[index].id = 0;
			if(!argv[i + 1] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
				shell[index].cmd[i_++] = strdup(argv[i]);
			shell[index].cmd[i_] = NULL;
			i++;
			index++;
		}
	}
}

void print_struct(t_shell *shell)
{
	int i = 0;
	int i_ = 0;

	while (shell[i].cmd)
	{
		i_ = 0;
		printf("---------------\nID -> %d\n", shell[i].id);
		printf("pipe_before -> %d\n", shell[i].pipe_before);
		while (shell[i].cmd[i_])
		{
			printf("shell[%d].cmd[%d] -> %s\n", i, i_ ,shell[i].cmd[i_]);
			i_++;
		}
		i++;
		printf("----------------\n");
	}
}

void exec(t_shell *shell, char **envp)
{
	int	i;
	int fd[2];
	int status;
	pid_t pid;

	while (shell[i].cmd)
	{
		if(shell[i].id)
		{
			if(pipe(fd) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
		}
		pid = fork();
		if(pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if(pid == 0)
		{
			if(shell[i].pipe_before)
			{
				dup2(shell[i].prev_fd, STDIN_FILENO);
				close(shell[i].prev_fd);
			}
			if(shell[i].id)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
			}
			execve(shell[i].cmd[0], shell[i].cmd, envp);
			perror("execve");
			exit(EXIT_FAILURE);
		}
		else 
		{
			if(shell[i].pipe_before)
				close(shell[i].prev_fd);
			if(shell[i].id)
			{
				close(fd[1]);
				shell[i + 1].prev_fd = fd[0];
				shell[i + 1].pipe_before = 1;
			}
			waitpid(pid, &status, 0);
		}
		i++;
	}
	

}


int main(int argc, char **argv, char **envp)
{
	if (argc < 2)
		return(write(2, "Error\n", ft_strlen("Error\n")));
	t_shell *shell = malloc(sizeof(t_shell) * mem_struct(argv) + 1);
	mem_set(shell, argv);
	//print_struct(shell);
	exec(shell, envp);
}