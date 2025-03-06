#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct t_shell
{
	char 	**cmd;
	int		id;
	int		pipe_before;
	int 	prev_fd;
}	t_shell;

int ft_strlen(char *cmd)
{
	int i = 0;
	while (cmd[i])
		i++;
	return(i);
}

int nb_cmd(char **argv)
{
	int i = 0;
	int len = 1;

	while (argv[i])
	{
		if((strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0) && argv[i + 1])
			len++;
		i++;
	}
	//printf("len -> %d\n", len);
	return(len);
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

int	alloc_cmd(char **argv, int i)
{
	int len = 0;
	while (strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0 && argv[i + 1])
	{
		i++;
		len++;
	}
	if(!argv[i + 1])
		len++;
	//printf("len_char -> %d\n", len);
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
			shell[index].cmd = malloc(sizeof(char *) * alloc_cmd(argv, i) + 1);
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

void exec_dot_come(char **cmd, char **envp)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if(pid == 0)
		execve(cmd[0], cmd, envp);
	else 
		waitpid(-1, &status, 0);
	// FAIL CASE // 
	return ;
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
    int fd[2];
    int status;
    pid_t pid;

	if (shell[0].cmd[0] == NULL)
		return ;
	while (shell[i].cmd != NULL)
    {
        // Si la commande doit envoyer sa sortie dans un pipe,
        // créez le pipe avant le fork.
        if(shell[i].id == 1) 
		{
            if (pipe(fd) == -1) 
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
        else if(pid == 0)  // Processus enfant
        {
			if (shell[i].cmd[0] == NULL)
				return ;
            if(shell[i].pipe_before) {
                // Rediriger l'entrée standard vers le pipe précédent
                dup2(shell[i].prev_fd, STDIN_FILENO);
                close(shell[i].prev_fd);
            }
            if(shell[i].id == 1) {
                // Rediriger la sortie standard vers le pipe nouvellement créé
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }
			if(strcmp(shell[i].cmd[0], "cd") == 0)
				exit(cd(shell[i].cmd, len_array(shell[i].cmd)));
            execve(shell[i].cmd[0], shell[i].cmd, envp);
            write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
			write(2, shell[i].cmd[0], ft_strlen(shell[i].cmd[0]));
			write(2, "\n", 1);
            exit(EXIT_FAILURE);
        }
        else  // Processus parent
        {
            // Si un pipe précède, fermer le descripteur de lecture dans le parent
            if(shell[i].pipe_before)
                close(shell[i].prev_fd);
            // Si la commande a créé un pipe pour la sortie,
            // fermer le côté écriture et conserver le côté lecture pour la prochaine commande.
            if(shell[i].id == 1)
            {
                close(fd[1]);
                // On stocke fd[0] dans la structure de la commande suivante pour pipe_before
                shell[i+1].prev_fd = fd[0];
                shell[i+1].pipe_before = 1;
            }
            waitpid(pid, &status, 0);
        }
        i++;
    }
}



int main(int argc, char **argv, char **envp)
{
	t_shell *shell;

	if(argc < 2)
		return(printf("ERROR \n"));
	//printf("aloc -> %d\n", aloc_shell);
	shell = malloc(sizeof(t_shell) * nb_cmd(argv) + 1); 
	// Etape 1. Stockage && identification.
	store_in_struct(shell, argv);
	//print_all(shell);
	// Etape 3. Execution.
	exec(shell, envp);
}