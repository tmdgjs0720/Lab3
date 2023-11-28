#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Please provide command arguments\n");
        return 1;
    }
    
    // 파일 재지향 및 파이프 기능 실행
    redirection(argc - 1, argv + 1);
    pipe_launch(argc - 1, argv + 1);
    
    return 0;
}
void redirection(int narg, char **argv) {
    pid_t pid;
    int i = 0;
    int fd;
    int split_index = 0;
    int is_write = 0;
    int write_flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char *cmd[10] = {NULL};
    
    for(i = 0; i < narg; i++) {
        if(!strcmp(argv[i], ">")) {
            split_index = i;
            is_write = 1;
        }
        else if(!strcmp(argv[i], "<")) {
            split_index = i;
            is_write = 0;
        }
    }
    
    for(i = 0; i < split_index; i++) {
        cmd[i] = argv[i];
    }
    
    pid = fork();
    
    if(pid == 0) {
        if(is_write) {
            if((fd = open(argv[split_index + 1], write_flags, mode)) == -1) {
                perror("[ERROR] OPEN: ");
                exit(1);
            }
            if(dup2(fd, 1) == -1) {
                perror("[ERROR] DUP2: ");
                exit(1);
            }
        }
        else {
            if((fd = open(argv[split_index + 1], O_RDONLY)) == -1) {
                perror("[ERROR] OPEN: ");
                exit(1);
            }
            if(dup2(fd, 0) == -1) {
                perror("[ERROR] DUP2: ");
                exit(1);
            }
        }
        
        if(close(fd) == -1) {
            perror("[ERROR] CLOSE: ");
            exit(1);
        }
        
        execvp(cmd[0], cmd);
        perror("[ERROR] EXECVP: ");
        exit(1);
    }
    else if(pid > 0) {
        wait(NULL);
    }
}
void pipe_launch(int narg, char **argv) {
    pid_t pid;
    int command_pos = 0;
    int count_pipe = 0;
    int i = 0;
    int pd_idx = 0;
    int k = 0;
    int status;
    
    while(argv[command_pos] != NULL) {
        if(argv[command_pos][0] == '|')
            count_pipe++;
        command_pos++;
    }
    
    int fd[count_pipe * 2];
    
    for(i = 0; i < count_pipe; i++) {
        if(pipe(fd + i * 2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }
    
    command_pos = 0;
    
    for(i = 0; i <= count_pipe; i++) {
        int j = 0;
        char **command = (char **)malloc(narg * sizeof(char *));
        
        if(!strcmp(argv[command_pos], "|"))
            command_pos++;
        
        if(i < count_pipe) {
            while(strcmp(argv[command_pos], "|")) {
                command[j] = (char *)malloc(100 * sizeof(char));
                strcpy(command[j], argv[command_pos]);
                command_pos++;
                j++;
            }
        }
        else {
            while(argv[command_pos] != NULL) {
                command[j] = (char *)malloc(100 * sizeof(char));
                strcpy(command[j], argv[command_pos]);
                command_pos++;
                j++;
            }
        }
        
        command[j] = NULL;
        pid = fork();
        
        if(pid == 0) {
            if(i < count_pipe) {
                if(dup2(fd[pd_idx + 1], 1) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            
            if(pd_idx != 0) {
                if(dup2(fd[pd_idx - 2], 0) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            
            for(k = 0; k < 2 * count_pipe; k++) {
                close(fd[k]);
            }
            
            if(execvp(command[0], command) < 0) {
                perror(command[0]);
                exit(EXIT_FAILURE);
            }
        }
        
        pd_idx += 2;
        k = 0;
        
        while(command[k] != NULL) {
            free(command[k]);
            k++;
        }
        
        free(command);
    }
    
    for(k = 0; k < 2 * count_pipe; k++) {
        close(fd[k]);
    }
    
    for(k = 0; k < count_pipe + 1; k++) {
        wait(&status);
    }
}
