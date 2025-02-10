
// Declaration de la strucute utiliser, array de struct contenant char **cmd
// et un id -> 1 pour | -> 2 pour ; -> 0 pour le reste, ? Comment connaitre l'id. Regarder le | ou le ; d'apres et si pas alors mettre 0. 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct t_shell
{
	char **cmd;

}	t_shell;


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

int ft_strlen(char *str)
{
	int i = 0;

	while(str[i])
		i++;
	return(i);
}


void mem_for_cmd(t_shell *shell, char **argv, int len, int i, int index)
{
	int i_cmd = 0;


	while(len)
	{
		printf("len  -> %d | argv[i - 1] -> %s | len_cmd -> %d\n", len -1, argv[i - 1], ft_strlen(argv[i -1]) + 1);
		shell[index].cmd[len - 1] = malloc(sizeof(char) * (ft_strlen(argv[i - 1]) + 1));
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
				len++;
			printf("len + 1 -> %d\n", len + 1);
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

int main(int argc, char **argv, char **envp)
{
	// Declarer et initier la struct
	t_shell *shell;

	// Faire une fonction qui calcule la memoire necessaire
	printf("L'alloc de mem est egal a -> %d\n", mem_struct(argc, argv));
	shell = malloc(sizeof(t_shell) * (mem_struct(argc, argv)));
	mem_for_shell(shell, argv, argc);
}
