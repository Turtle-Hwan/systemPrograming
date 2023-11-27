#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

typedef	struct arg
{
	//start_idx 부터 count_num개 만큼 각 스레드가 확인.
	long long	t_start_idx;
	long long	t_count_num;
} thread_arg;

int					bmp_start_idx;	//처음 할당되고 변하지 않음.
unsigned char		*bmp_mmap;		//처음 할당되고 변하지 않고, 읽기만 함.
volatile long long	color_histogram[256] = {0, }; //초기화 된 후, 각 스레드가 접근.
pthread_mutex_t			mutex = PTHREAD_MUTEX_INITIALIZER;


void	my_write(int fd, char *str)
{
	write(fd, str, strlen(str));
}

void	itoa_recursive(long long n, int n_len, char *ret)
{
	if (n < 0)
		ret[n_len - 1] = '0' + (n % 10) * (-1);
	else
		ret[n_len - 1] = '0' + n % 10;
	if (n / 10 == 0)
	{
		if (n < 0)
			ret[0] = '-';
		return ;
	}
	itoa_recursive(n / 10, n_len - 1, ret);
}

char	*itoa(long long n)
{
	int				n_len;
	long long		n_copy;
	char			*ret;

	n_copy = n;
	if (n <= 0)
		n_len = 1;
	else
		n_len = 0;
	while (n)
	{
		n_len += 1;
		n /= 10;
	}
	ret = (char *)malloc(sizeof(char) * (n_len + 1));
	if (ret == NULL)
		return (NULL);
	ret[n_len] = '\0';
	if (n_copy == 0)
		ret[0] = '0';
	else
		itoa_recursive(n_copy, n_len, ret);
	return (ret);
}


/** bmp file fd와 특정 offset 받아서, 여기서부터 4byte를 int형으로 변환하는 함수*/
int	bmp_hex_4byte_to_int(int fd, __off_t offset)
{
	unsigned char	file_buf[4];
	int				ret_val;
	int				d;

	lseek(fd, offset, SEEK_SET);
	read(fd, file_buf, 4);
	ret_val = 0;
	for (int i = 0; i < 4; i++) 
	{
		d = 1;
		for (int j = 0; j < i*2; j++)
			d *= 16;
		ret_val += file_buf[i] * d;
		//printf("%02X %d start : %X, %d\n", file_buf[i], file_buf[i], ret_val, ret_val);
	}
	return ret_val;
}

/** thread function*/
void	*thread_function (void *arg)
{
	thread_arg	*targ = (thread_arg *)arg;
	
	//각자 스레드에 할당된 범위만큼 bmp_mmap[]을 순회하면서 color_histogram에 한 스레드씩만 접근해서 기록
	for (int i = 0; i < targ->t_count_num; i++)
	{
		int	idx = bmp_mmap[bmp_start_idx + targ->t_start_idx + i];
		pthread_mutex_lock(&mutex);
		color_histogram[idx] += 1;
		pthread_mutex_unlock(&mutex);
	}
	return (NULL);
}

#include <stdio.h>
//201911560 김지환
int	main(int argc, char **argv)
{
	if (argc != 4)
	{
		my_write(2, "ERR: 입력 양식은  ./ku_histo n i input.bmp  입니다.\n");
		return(1);
	}
	int		thread_num = atoi(argv[1]);
	int		histo_range = atoi(argv[2]);
	char	*bmp_fname = argv[3];
	if (thread_num <= 0 || histo_range <= 0 || histo_range > 256)
	{
		my_write(2, "ERR: 범위 에러");
		return(1);
	}

	int	bmp_fd = open(bmp_fname, O_RDONLY);
	if (bmp_fd == -1)
	{
		perror("open");
		return (1);
	}

	bmp_start_idx = bmp_hex_4byte_to_int(bmp_fd, 10);
	long long	bmp_width = bmp_hex_4byte_to_int(bmp_fd, 18);
	long long	bmp_height = bmp_hex_4byte_to_int(bmp_fd, 22);

	//mmap
	struct stat	bmp_st;	//file 정보
	if (fstat(bmp_fd, &bmp_st) == -1)
	{
		perror("fstat");
		return (1);
	}
	bmp_mmap = mmap(0, bmp_st.st_size, PROT_READ, MAP_SHARED, bmp_fd, 0);
	if (bmp_mmap == MAP_FAILED)
	{
		perror("mmap");
		return (1);
	}

	/** 스레드 나누기 */
	pthread_t	thread_id[thread_num];
	thread_arg	targ[thread_num];

	//스레드별 계산량 나누기 
	//targ[0].t_start_idx + targ[0].t_count_num == targ[1].t_start_idx
	long long	bmp_size = bmp_width * bmp_height;

	if (bmp_size < thread_num)
		thread_num = bmp_size;
	long long	bmp_size_remainder = bmp_size % thread_num;
	long long	now_start_idx = 0;

	for (int i = 0; i < thread_num; i++)
	{
		targ[i].t_start_idx = now_start_idx;
		targ[i].t_count_num = bmp_size / thread_num;
		if (bmp_size_remainder > 0)
		{
			targ[i].t_count_num += 1;
			bmp_size_remainder -= 1;
		}
		now_start_idx += targ[i].t_count_num;
	}

	int t_status = 0;
	for (int i = 0; i < thread_num; i++)
	{
		t_status = pthread_create(thread_id + i, NULL, thread_function, targ + i);
		if (t_status != 0)
		{
			perror("thread_create");
			return(1);
		}
	}
	
	/** 모든 스레드가 종료될 때까지 기다리기*/
	for (int i = 0; i < thread_num; i++)
	{
		t_status = pthread_join(thread_id[i], NULL);
		if (t_status != 0)
		{
			perror("thread_create");
			return(1);
		}
	}

	/** 결과값 출력 부분 */
	int	output_fd = creat("results.txt", 0666);
	if (output_fd == -1)
	{
		perror("creat");
		return(1);
	}
	for (int i = 0; i < 256; i++)
	{
		char		*c;
		long long	sum_range = 0;

		if (i % histo_range == 0)
		{
			c = itoa(i);
			my_write(output_fd, c);
			my_write(output_fd, " ");
			free(c);

			sum_range = color_histogram[i];
			for (int j = i + 1; j < i + histo_range; j++)
				if (j < 256)
					sum_range += color_histogram[j];
			c = itoa(sum_range);
			my_write(output_fd, c);
			my_write(output_fd, "\n");
			free(c);
		}
	}
}