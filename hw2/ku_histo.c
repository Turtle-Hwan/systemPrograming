#include <string.h>	//strlen
#include <stdlib.h>	//atoi

void	my_write(int fd, char *str)
{
	write(fd, str, strlen(str));
}

//201911560 김지환
int	main(int argc, char **argv)
{
	if (argc != 4)
	{
		my_write(2, "ERR: 입력 양식은  ./ku_histo n i input.bmp  입니다.\n");
		return(1);
	}
	int	thread_num = atoi(argv[1]);
	int	histo_range = atoi(argv[2]);
	char bmp_fname = argv[3];
	if (thread_num <= 0 || histo_range <= 0 || histo_range > 256)
	{
		my_write(2, "ERR: 범위 에러");
		return(1);
	}

	int bmp_fd = open();

}