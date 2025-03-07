#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct t_shell
{
	char **cmd;
	int id;
	int pipe_before;
	int prev_fd;
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
		if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0 && argv[i + 1])
			len++;
		i++;
	}
	return(len);
}

int	nb_cmd(char **argv, int i)
{
	int len = 0;

	while (strcmp(argv[i], "|") && strcmp(argv[i], ";") && argv[i + 1])
	{
		i++;
		len++;
	}
	if(!argv[i + 1])
		len++;
	//printf("Nb_cmd -> %d\n", len);
	return(len);
}

void	store_data(t_shell *shell, char **argv)
{
	int i = 1;
	int index = 0;
	int i_;

	while (argv[i])
	{
		if((strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0) && argv[i + 1])
			i++;
		else 
		{
			shell[index].cmd = malloc(sizeof(char *) * (nb_cmd(argv, i) + 1));
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
			if(!argv[i + 1] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				shell[index].cmd[i_++] = argv[i]; 
			if(strcmp(argv[i], "|") == 0)
				shell[index].id = 1;
			else 
				shell[index].id = 0;
			shell[index].cmd[i_] = NULL;
			index++;
			i++;
		}
	}
	shell[index].cmd = NULL;
}

void print_the_struct(t_shell *shell)
{
	int i = 0;
	int index = 0;

	while(shell[index].cmd)
	{
		i = 0;
		printf("---------------\nID -> %d\n", shell[index].id);
		printf("Pipe_before -> %d\n", shell[index].pipe_before);
		while (shell[index].cmd[i])
		{
			printf("shell[%d]->cmd[%d] %s\n", index, i, shell[index].cmd[i]);
			i++;
		}
		index++;
		printf("---------------\n");
	}
}
int	cd(char **argv, int i)
{
	if (i != 2)
	{
		write(2, "error: cd: bad arguments\n", ft_strlen("error: cd: bad arguments\n"));
		return (1);
	}
	if (chdir(argv[1]) == -1)
	{
		write(2, "error: cd: cannot change directory to", ft_strlen("error: cd: cannot change directory to"));
		write(2, argv[1], ft_strlen(argv[1]));
		write(2, "\n", 1);
		return (1);
	}
	return (0);
}

int len_array(char **cmd)
{
	int i = 0;

	while (cmd[i])
		i++;
	return(i);

}

void exec(t_shell *shell, char **envp)
{
	int i = 0;
	int status;
	int fd[2];
	pid_t pid;

	if(shell[0].cmd[0] == NULL)
		return ;
	while (shell[i].cmd)
	{
		if(shell[i].cmd[0] == NULL)
			return ;
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
		else if(pid == 0)
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
			if(strcmp(shell[i].cmd[0] ,"cd") == 0)
				exit(cd(shell[i].cmd, len_array(shell[i].cmd)));
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
	if(argc < 2)
		write(2, "Error bad arguments\n", ft_strlen("Error bad arguments\n"));
	t_shell *shell = malloc(sizeof(t_shell) * (mem_struct(argv) + 1));
	store_data(shell, argv);
	//print_the_struct(shell);
	exec(shell, envp);
}