#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char *sep = " -\r\t\n./,";

void check(int num, int has_digit, int bad)
{
	if(has_digit && !bad && (num % 5 == 0 || num % 6 == 0))
		printf("%d\n", num);
}

void sixfive(int fd)
{
	char c;
	int num = 0;
	int has_digit = 0;
	int bad = 0;

	while(read(fd, &c, 1) == 1){
		if(c >= '0' && c <= '9'){
			num = num * 10 + (c - '0');
			has_digit = 1;
		}
		else if(strchr(sep, c)){ // strchr => find the first occurrence of a specified character in a string.
			check(num, has_digit, bad);
			num = 0;
			has_digit = 0;
			bad = 0;
		}
		else{ // bad character e.g. 817a, aa918, adv
			bad = 1;
		}
	}
	check(num, has_digit, bad);
}

int main(int argc, char *argv[]){
	if(argc < 2){
		fprintf(2, "Usage: sixfive file...\n");
		exit(1);
	}
	for(int i = 1; i < argc; i++){
		int fd = open(argv[i], O_RDONLY);
		if(fd < 0){
			fprintf(2, "sixfive: cannot open %s\n", argv[i]);
			exit(1);
		}
		sixfive(fd);
		close(fd);
	}
	exit(0);
}
