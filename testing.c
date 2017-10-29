#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define LEN_MAX 312
#define DATA_MAX 4004
#define DIR_S_FLAG (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define REG_S_FLAG (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char const *argv[])
{
	char homedir[LEN_MAX] = "../cdir/";
	char dir[LEN_MAX] = "dir", path[LEN_MAX];
	char file[LEN_MAX] = "file", num[10] = "123456789", data[300];
	strcpy(path, homedir);
	for (int i = 0; i < 4; i++) {
		int len2 = strlen(path);
		strcat(path, file);
		int len = strlen(path);
		path[len] = num[i];
		path[len+1] = '\0';
		int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		printf("file: %s\n", path);
		for (int j = 0; j < 100; j++)
			data[j] = num[i];
		data[100] = '\0';
		write(fd, data, strlen(data));
		close(fd);

		strcpy(path+len2, dir);
		len = strlen(path);
		path[len++] = num[i];
		path[len++] = '/';
		path[len] = '\0';
		mkdir(path, DIR_S_FLAG);
		printf("dir: %s\n", path);
	}
	link("../cdir/file1", "../cdir/dir1/hardlink");
	symlink("dir1/file2", "../cdir/s_link");
	symlink("xxx", "../cdir/s_link2");
	return 0;
}