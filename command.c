#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <sys/types.h>
#define BUFSIZE 256
char path[BUFSIZE];
void launch(int narg, char **argv);
void ls(int narg, char **argv);
void pwd();
void cd(int narg, char **argv);
void mkdir_(int narg, char **argv);
void rmdir_(int narg, char **argv);
void rmdir_rm(int narg, char **argv);
void ln(int narg, char **argv);
void cp(int narg, char **argv);
void mv(int narg, char **argv);
void cat(int narg, char **argv);
int main()
{
	char buf[256];
	char * argv[50];
	int narg;
	while (1)
    	{
		char *argv[50] = {'\0'};
		getcwd(path, BUFSIZE);
        	printf("%s$ ", path);
        	gets(buf);
        narg = getargs(buf, argv);
        handler(narg, argv);
    }
}
int getargs(char *cmd, char **argv){
	int narg =0;
	
	while(*cmd){
		if(*cmd ==' '||*cmd =='\t')
			*cmd++='\0';
		else{
			argv[narg++] = cmd++;
			while(*cmd !='\0'&&*cmd !=' '&&*cmd !='\t')
			cmd++;
		}
	}
	argv[narg] =NULL;
	return narg;
}
//함수호출 핸들러 함수
void handler(int narg, char **argv) {
    int i =0;
    int is_background =0, is_redirection =0, is_pipe =0;
    for(i =0; i < narg; i++) {
        if( (!strcmp(argv[i], ">")) || (!strcmp(argv[i], "<"))) {
            is_redirection =1;
            break;
        }
        else if (!strcmp(argv[i], "|")) {
            is_pipe =1;
            break;
        }
        else if (!strcmp(argv[i], "&")) {
            is_background =1;
            break;
        }
    }
    
    if(!strcmp(argv[0], "ls")){
        ls(narg, argv);
    }
    else if(!strcmp(argv[0], "cd")){
        cd(narg, argv);
    }
    else if (!strcmp(argv[0], "rmdir")){
        rmdir_(narg, argv);
    }
    else if (!strcmp(argv[0], "cp")){
        cp(narg, argv);
    }
    else if (!strcmp(argv[0], "mv")){
        mv(narg, argv);
    }
    else if (!strcmp(argv[0], "pwd")){
        pwd(narg, argv);
    }
    else if (!strcmp(argv[0], "rm")){
        rmdir_rm(narg, argv);
    }
    else if (!strcmp(argv[0], "mkdir")){
        mkdir_(narg, argv);
    }
    else if (!strcmp(argv[0], "ln")){
        ln(narg, argv);
    }
    else if (!strcmp(argv[0], "cat")){
        cat(narg, argv);
    }
}
/* pwd
 * 현재 작업 중인 디렉토리 출력 
 * authour tmdgjs0720
 */
void pwd(){
	char *buf = (char *)malloc(sizeof(char)*(BUFSIZE));
	
	if(getcwd(buf,BUFSIZE)==NULL){
		perror("(Error) Pwd");
		exit(EXIT_FAILURE);
	}
	else
		printf("%s \n",buf);
	
	free(buf);
}
/* ls
 * 현재 디렉토리 파일 목록 출력
 * authour tmdgjs0720
 */
void ls(int narg, char **argv){
	char temp[256];
	if(narg ==1){
		getcwd(temp, 256);
		printf("%s", temp);
		argv[1] = temp;
	}
	
	DIR *pdir;
	struct dirent *pde;
	int i =0;
	if((pdir = opendir(argv[1])) <0) {
		perror("(Error) Opendir: ");
	}
	printf("\n");
	while((pde = readdir(pdir)) !=NULL) {
		printf("%-20s", pde->d_name);
		if(++i % 3 ==0)
			printf("\n");
		}
	printf("\n");
	closedir(pdir);
}
/* cd
 * 디렉토리 위치 이동하는 명령어
 * authour tmdgjs0720
 */
void cd(int narg, char **argv){
	if(narg ==1){
		chdir("Home");
	}
	else{
		if(chdir(argv[1]) ==-1){
			printf("%s : no search file or directory \n", argv[1]);
		}
	}
}
/* mkdir
 * 디렉토리 생성
 * authour cmsong111
 */
void mkdir_(int narg, char **argv){
	umask(0);
	if(argv)
	
	if(narg <2)
		fprintf(stderr, "Pathname does not exists \n");
	else{
		if(mkdir(argv[1], umask(0)) <0) {
			perror("(Error) mkdir");
			exit(EXIT_FAILURE);
		}
	}
}
/* rmdir
 * 디렉토리 삭제
 * authour cmsong111
 */
void rmdir_(int narg, char **argv){
    int i =0;
    char temp[256];
    if(narg ==1){
        printf("rmdir: missing operand\n");
    }
    else{
        if (rmdir(argv[1]) ==-1) {
            perror("rmdir");
        }
    }
}
void rmdir_rm(int narg, char ** argv) {
    if(narg <2)
        fprintf(stderr, "Path is not exists\n");
    else {
        if(remove(argv[1]) <0) {
            perror("[ERROR] RM/RMDIR");
            exit(EXIT_FAILURE);
        }
    }
}
/* ln
 * 파일 링크 생성
 * authour cmsong111
 */
void ln(int narg, char **argv){
	char cmd;
    char *src;  
    char *target;
    if (narg <2) {
        fprintf(stderr, "Usage: ln [u,s] ...\n");
        fprintf(stderr, " ln src target\n");
        fprintf(stderr, " ln u[nlink] filename\n");
        fprintf(stderr, " ln s[ymlink] src target\n");
        exit(1);
    }
    
    if (!strcmp(argv[1],"-s")) {
        if (narg <4) {
            fprintf(stderr, "ln l src target [link]\n");
            exit(1);
        }
        src = argv[2];
        target = argv[3]; 
        if (symlink(src, target) <0) {
            perror("symlink");
            exit(1);
        }
    }
    else if (!strcmp(argv[1],"-u")) {
        src = argv[2];
        if (unlink(src) <0) {
            perror("unlink");
            exit(1);
        }
    }
    else if (narg ==3) {
        src = argv[1];
        target = argv[2];
        if (link(src, target) <0) {
            perror("link");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Unknown command...\n");
    }
}
/* cp
 * 파일 복사
 * authour cdiff
 */
void cp(int narg, char **argv){
	int src_fd; 
    int dst_fd;
    char buf[256];
    ssize_t rcnt;
    ssize_t tot_cnt =0;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 
    if (narg <3){
        fprintf(stderr, "Usage: file_copy src_file dest_file\n");
        exit(1);
    }
    if ((src_fd = open(argv[1], O_RDONLY)) ==-1){
        perror("[ERROR]SRC OPEN");
        exit(1);
    }
    if ((dst_fd = creat(argv[2], mode)) ==-1){
        perror("[ERROR]DST OPEN");
        exit(1);
    }
    while ((rcnt = read(src_fd, buf, 256)) >0){
        tot_cnt += write(dst_fd, buf, rcnt);
    }
    if (rcnt <0){
        perror("[ERROR]READ");
        exit(1);
    }
    close(src_fd);
    close(dst_fd);
}
/* mv
 * 파일 이동 및 파일 이름 변경
 * authour cdiff
 */
void mv(int narg, char **argv){
struct stat buf;
    char *target;
    char *src_file_name_only;
    if (narg <3)
    {
        fprintf(stderr, "Usage: file_rename src target\n");
        exit(1);
    }
   
    if (access(argv[1], F_OK) <0)
    {
        fprintf(stderr, "%s not exists\n", argv[1]);
        exit(1);
    }
    else
    {
        char *slash = strrchr(argv[1], '/');
        src_file_name_only = argv[1];
        if (slash !=NULL)
        { 
            src_file_name_only = slash +1;
        }
    }
   
    target = (char *)calloc(strlen(argv[2]) +1, sizeof(char));
    strcpy(target, argv[2]);
    if (access(argv[2], F_OK) ==0)
    {
        if (lstat(argv[2], &buf) <0)
        {
            perror("lstat");
            exit(1);
        }
        else
        {
            if (S_ISDIR(buf.st_mode))
            {
                free(target);
                target = (char *)calloc(strlen(argv[1]) + strlen(argv[2]) +2, sizeof(char));
                strcpy(target, argv[2]);
                strcat(target, "/");
                strcat(target, src_file_name_only);
            }
        }
    }
    printf("target = %s\n", target);
    if (rename(argv[1], target) <0){
        perror("rename");
        exit(1);
    }
    free(target);
}
/* cat
 * 파일 내용 출력
 * authour cdiff
 */
void cat(int narg, char **argv) {
    FILE *file[narg - 1];
    int loop;
    int buf;
    if (narg < 2) {
        fprintf(stderr, "Please Input Files\n");
        exit(1);
    }
    for (loop = 0; loop < narg - 1; loop++) {
        file[loop] = fopen(argv[loop + 1], "r");
        if (file[loop] == NULL) {
            printf("cat: %s: No such file or directory\n", argv[loop + 1]);
            continue;
        } else {
            while ((buf = fgetc(file[loop])) != EOF) {
                printf("%c", buf);
            }
            if (fclose(file[loop]) != 0) {
                perror("fclose");
            }
        }
    }
}
