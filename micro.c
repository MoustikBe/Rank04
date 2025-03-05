#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef struct t_shell
{
	char **cmd;
	int id;
	int pipe_before;
	int prev_fd;
	int pipe_fd[2];
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
	printf("%d\n", len);
	return(len);
}

int main(int argc, char **argv)
{
	if(argc < 2)
		write(2, "Error bad arguments\n", ft_strlen("Error bad arguments\n"));
	t_shell *shell = malloc(sizeof(t_shell) * (mem_struct(argv) + 1));
//	store_data(shell);
}