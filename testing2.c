//compile by -lm -lbsd
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <bsd/md5.h>

#define LEN_MAX 312
#define DATA_MAX 4004
#define DIR_S_FLAG (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define REG_S_FLAG (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void md5(const char* str, size_t len, uint8_t digest[MD5_DIGEST_LENGTH]) {
  MD5_CTX ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, (const uint8_t*)str, len);
  MD5Final(digest, &ctx);
}

int md5_file(const char* path, uint8_t digest[MD5_DIGEST_LENGTH]) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    return 0;
  }
  char buf[4096];
  size_t len;
  MD5_CTX ctx;
  MD5Init(&ctx);
  while ((len = read(fd, buf, 4096)) > 0) {
    MD5Update(&ctx, (const uint8_t*)buf, len);
  }
  MD5Final(digest, &ctx);
  close(fd);
  return 1;
}
int main(int argc, char const *argv[])
{
	char homedir[LEN_MAX] = "../cdir/";
	char dir[LEN_MAX] = "dir", path[LEN_MAX];
	char file[LEN_MAX] = "file", num[10] = "123456789", data[300];
	char sdir[LEN_MAX] = "../sdir/user/";
	int len3 = strlen(sdir), ok;
	struct stat statbuf1, statbuf2;
	strcpy(path, homedir);
	for (int i = 0; i < 4; i++) {
		ok = 1;
		int len2 = strlen(path);
		strcat(path, file);
		int len = strlen(path);
		path[len] = num[i];
		path[len+1] = '\0';
		
		strcpy(sdir+len3, path+8);
		printf("file: %s %s\n", path, sdir);
		lstat(path, &statbuf1);
		lstat(sdir, &statbuf2);
		if (statbuf2.st_mode != statbuf1.st_mode) {
			printf("   permission wrong!\n");
			ok = 0;
		}
		if (statbuf1.st_atime != statbuf2.st_atime || statbuf1.st_mtime != statbuf2.st_mtime) {
			printf("   time wrong!\n");
			ok = 0;
		}
		char md5_1[40], md5_2[40];
		md5_file(path, md5_1);
		md5_file(sdir, md5_2);
		if (strcmp(md5_2, md5_1) != 0) {
			printf("   data wrong!\n");
			ok = 0;
		}
		if (ok) printf("   ok\n");

		ok = 1;
		strcpy(path+len2, dir);
		len = strlen(path);
		path[len++] = num[i];
		path[len++] = '/';
		path[len] = '\0';
		
		strcpy(sdir+len3, path+8);
		printf("dir: %s %s\n", path, sdir);
		lstat(path, &statbuf1);
		lstat(sdir, &statbuf2);
		if (statbuf2.st_mode != statbuf1.st_mode) {
			printf("   permission wrong!\n");
			ok = 0;
		}
		if (statbuf1.st_atime != statbuf2.st_atime || statbuf1.st_mtime != statbuf2.st_mtime) {
			printf("   time wrong!\n");
			ok = 0;
		}
		if (ok) printf("   ok\n");
	}
	//link("../cdir/file1", "../cdir/dir1/hardlink");
	lstat("../sdir/user/file1", &statbuf1);
	lstat("../sdir/user/dir1/hardlink", &statbuf2);
	if (statbuf2.st_ino != statbuf1.st_ino) {
		printf("hardlink wrong!\n");
	} else {
		printf("hardlink ok\n");
	}
	//symlink("xxx", "../cdir/s_link2");
	ok = 1;
	lstat("../sdir/user/s_link2", &statbuf1);
	if (!S_ISLNK(statbuf1.st_mode)) {
		printf("softlink type wrong\n");
		ok = 0;
	}
	//symlink("dir1/file2", "../cdir/s_link");
	stat("../sdir/user/dir1/file2", &statbuf1);
	stat("../sdir/user/s_link", &statbuf2);
	if (statbuf2.st_ino != statbuf1.st_ino) {
		printf("softlink point wrong!\n");
	}
	lstat("../cdir/s_link", &statbuf1);
	lstat("../sdir/user/s_link", &statbuf2);
	if (statbuf1.st_atime != statbuf2.st_atime || statbuf1.st_mtime != statbuf2.st_mtime) {
		printf("   time wrong!\n");
		ok = 0;
	}
	if (ok) {
		printf("softlink ok\n");
	}
	return 0;
}