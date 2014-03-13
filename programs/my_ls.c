#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<limits.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<dirent.h>

#define Max_Depth 2

int l = 0, r = 0;

int change_dir(char* path) {
	int flag = chdir(path);
	if (flag < 0)
		perror("Невозможно открыть дерикторию");
	return flag;
}

void read_args(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-l") == 0) 
			l = 1;
		else if (strcmp(argv[i], "-r") == 0)
			r = 1;
		else if (change_dir(argv[i]) < 0) 
			_exit(1);
	}
}

void print_mode(int mode) {
	char str[11] = "----------\0";
	if (S_ISDIR(mode)) str[0] = 'd';
	if (mode & S_IRUSR) str[1] = 'r';
	if (mode & S_IWUSR) str[2] = 'w';
	if (mode & S_IXUSR) str[3] = 'x';
	if (mode & S_IRGRP) str[4] = 'r';
	if (mode & S_IWGRP) str[5] = 'w';
	if (mode & S_IXGRP) str[6] = 'x';
	if (mode & S_IROTH) str[7] = 'r';
	if (mode & S_IWOTH) str[8] = 'w';
	if (mode & S_IXOTH) str[9] = 'x';
	printf("%s", str);
}

void print(struct dirent *dir, struct stat *stbuf, int depth) {
	if (dir->d_name[0] == '.') 
		return;
	for (int i = 0; i < depth*4; i++)
		printf(" ");
	if (l == 1) {
		print_mode(stbuf->st_mode);
		printf("%d ", (int)stbuf->st_nlink);
		printf("%s ", getpwuid(stbuf->st_uid)->pw_name);
		printf("%s ", getgrgid(stbuf->st_gid)->gr_name);
		printf("%.12s ", 4 + ctime(&(stbuf->st_mtime)));
		printf("%ld ", (long) stbuf->st_size);
	}
	printf("%s\n", dir->d_name);
	return;
} 

int ls(char* name, int depth) {
	struct stat stbuf;
	if (change_dir(name) < 0) 
		return 0;
	DIR *dir = opendir("./");
	if (dir == NULL) {
		perror(name);
		return 0;
	}
	struct dirent *curr_dir;
	while ((curr_dir = readdir(dir)) != NULL) {
		if (stat(curr_dir->d_name, &stbuf) == -1)
			fprintf(stderr, "fsize: can't access %s\n", name);
		print(curr_dir, &stbuf, depth);
		if (r == 1 && S_ISDIR(stbuf.st_mode) && depth < Max_Depth && curr_dir->d_name[0] != '.')
			if (ls(curr_dir->d_name, depth + 1) == 1)
				if (change_dir("./../") < 0)
					return 0;
	}
	closedir(dir);
	return 1;
}

int main(int argc, char* argv[]) {
	read_args(argc, argv);
	ls("./", 0);
	return 0;
}
