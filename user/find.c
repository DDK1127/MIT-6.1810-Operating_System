#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

void run(char **cmd, char *path){
	char *args[MAXARG];
	int i = 0;

	while(cmd[i] != 0 && i < MAXARG - 2){
		args[i] = cmd[i];
		i++;
	}
	
	args[i++] = path;
	args[i] = 0;

	int pid = fork();
	if(pid < 0){
		fprintf(2, "find: fork failed\n");
		return;
	}
	if(pid == 0){
		exec(args[0], args);
		fprintf(2, "find: exec %s failed\n", args[0]);
		exit(1);
	}
	wait(0);
}

void find(char *path, char *name, char **cmd){
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, O_RDONLY)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	if(st.type != T_DIR){
		close(fd);
		return;
	}

	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
		fprintf(2, "find: path too long\n");
		close(fd);
		return;
	}

	strcpy(buf, path); // buf  = "./a"
	p = buf + strlen(buf);
	*p++ = '/'; // buf = "./a/", p after the '/'
	

	while(read(fd, &de, sizeof(de)) == sizeof(de)){
		if(de.inum == 0) // empty 
			continue;
	
		memmove(p, de.name, DIRSIZ);
		p[DIRSIZ] = 0;

		if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
      		continue;                     

    	if(stat(buf, &st) < 0){
      		fprintf(2, "find: cannot stat %s\n", buf);
      		continue;
    	}

    	if(strcmp(p, name) == 0){         
      		if(cmd == 0)
				printf("%s\n", buf);
			else 
				run(cmd, buf);
		}

    	if(st.type == T_DIR)         
      		find(buf, name, cmd);
	}
	close(fd);
}

int main(int argc, char *argv[]){
	if(argc == 3){
		find(argv[1], argv[2], 0);
	}
	else if(argc >= 5 && strcmp(argv[3], "-exec") == 0){
		find(argv[1], argv[2], &argv[4]);
	}
	else{
		fprintf(2, "usage: find dir name [-exec cmd ...]\n");
		exit(1);
	}
	exit(0);
}

