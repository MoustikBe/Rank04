
// Declaration de la strucute utiliser, array de struct contenant char **cmd
// et un id -> 1 pour | -> 2 pour ; -> 0 pour le reste, ? Comment connaitre l'id. Regarder le | ou le ; d'apres et si pas alors mettre 0. 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct t_shell
{
	char **cmd;
	int id;
	int pipe_needed;
}	t_shell;

typedef struct t_utils
{
	int fd[2];
}	t_utils;

int ft_strlen(char *str)
{
	int i = 0;

	while(str[i])
		i++;
	return(i);
}

int mem_struct(int argc, char **argv)
{
	int i = 1;
	int len_mem = 0;

	if(argc != 1)
		len_mem++;
	while(i < argc)
	{
		if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0)
		{
			if(argv[i + 1])
				len_mem++;
		}
		i++;
	}
	len_mem++;
	return(len_mem);
}

void mem_for_cmd(t_shell *shell, char **argv, int len, int i, int index)
{
	int i_cmd = 0;

	if(!argv[i + 1] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
			i++;
	shell[index].cmd[len] = NULL;
	while(len)
	{
		//printf("len  -> %d | argv[i - 1] -> %s | len_cmd -> %d\n", len -1, argv[i - 1], ft_strlen(argv[i -1]) + 1);
		//shell[index].cmd[len - 1] = malloc(sizeof(char) * (ft_strlen(argv[i - 1]) + 1));
		shell[index].cmd[len - 1] = strdup(argv[i - 1]);
		//printf("Valeur copier -> %s\n", shell[index].cmd[len - 1]);
		i_cmd++;
		len--;
		i--;
	}

}



void mem_for_shell(t_shell *shell, char **argv, int argc)
{
	int i = 1;
	int len = 0;
	int index = 0;

	if(argc == 1)
		return ;
	while(i < argc)
	{
		if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0 || !argv[i + 1])
		{
			if(!argv[i + 1] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
			{
				shell[index].id = 0;
				len++;
			}
			else if(strcmp(argv[i], "|") == 0)
				shell[index].id = 1;
			else if(strcmp(argv[i], ";") == 0)
				shell[index].id = 2;
			if(index > 0)
			{
				if(shell[index - 1].id == 1 && strcmp(argv[i], "|") != 0)
				{
					shell[index].id = 1;
					shell[index].pipe_needed = 1;
				}
				else 
					shell[index].pipe_needed = 0;
			}
			//printf("len + 1 -> %d\n", len + 1);
			shell[index].cmd = malloc(sizeof(char *) * (len + 1));
			mem_for_cmd(shell, argv, len, i, index);
			index++;
			len = 0;
		}
		else 
			len++;
		i++;
	}
	return ;
}

void print_struct(t_shell *shell, int mem)
{
	int i = 0;
	int j = 0;

	if (!shell)
	{
		printf("Erreur: shell est NULL\n");
		return;
	}

	while (i < mem)
	{
		if (!shell[i].cmd)
		{
			printf("shell[%d].cmd est NULL\n", i);
			i++;
			continue;
		}

		j = 0;
		while (shell[i].cmd[j])
		{
			printf("shell[%d].cmd[%d]: %s\n", i, j, shell[i].cmd[j]);
			j++;
		}
		printf("shell[%d].id = %d \n", i, shell[i].id);
		i++;
	}
}

void exec_nothing(t_shell *shell, char **envp, int i)
{
	execve(shell[i].cmd[0], shell[i].cmd, envp);
}


void exec_pipe(t_utils *utils, t_shell *shell, char **envp, int i, int pipe_needed)
{
    int new_pipe[2];

    // Créez un nouveau pipe si la commande courante doit écrire vers un pipe
    if (pipe_needed)
    {
        if (pipe(new_pipe) < 0)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Dans le processus enfant

        // Si la commande précédente a écrit dans un pipe, redirigez l'entrée standard
        if (utils->fd[0] != -1)
        {
            dup2(utils->fd[0], STDIN_FILENO);
            close(utils->fd[0]);
        }

        // Si la commande courante est suivie d'un pipe, redirigez la sortie standard
        if (pipe_needed)
        {
            dup2(new_pipe[1], STDOUT_FILENO);
            close(new_pipe[0]);
            close(new_pipe[1]);
        }

        // Exécutez la commande
        execve(shell[i].cmd[0], shell[i].cmd, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Dans le processus parent

        // Si un pipe a été créé, fermez l'extrémité d'écriture dans le parent
        if (pipe_needed)
            close(new_pipe[1]);

        // Si un pipe existait déjà pour la lecture (issu de la commande précédente), fermez-le
        if (utils->fd[0] != -1)
            close(utils->fd[0]);

        // Si la commande courante est suivie d'un pipe, sauvegardez le nouveau descripteur de lecture
        if (pipe_needed)
            utils->fd[0] = new_pipe[0];
        else
            utils->fd[0] = -1; // Réinitialisez si aucun pipe n'est utilisé

        waitpid(pid, NULL, 0);
    }
}


void exec_more(t_shell *shell, char **envp, int i)
{
	pid_t pid;
	int status;

	pid = fork();
	if(pid == 0)
		execve(shell[i].cmd[0], shell[i].cmd, envp);
	else
		waitpid(-1, &status, 0);
	return ;

}
int main(int argc, char **argv, char **envp)
{
	// Declarer et initier la struct
	t_shell *shell;
	t_utils *utils;
	int mem;
	// Faire une fonction qui calcule la memoire necessaire
	mem = mem_struct(argc, argv);
	utils = malloc(sizeof(t_utils));
	//printf("L'alloc de mem est egal a -> %d\n", mem);
	shell = malloc(sizeof(t_shell) * mem);
	shell[mem - 1].cmd = NULL;
	shell[mem - 1].id  = 0;
	mem_for_shell(shell, argv, argc);

	//Print la struct final.
	//print_struct(shell, mem);
	// jusque la parfait 
	// Executer
	execution(utils, shell, envp, mem);
	free(utils);
}

