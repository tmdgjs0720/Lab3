#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sigintHandler(int signum) {
    if (signum == SIGINT) {
        printf("\nCtrl-C가 눌렸습니다. 프로그램을 종료합니다.\n");
    } else if (signum == SIGQUIT) {
        printf("\nnCtrl-Z가 눌렸습니다. 프로그램을 종료합니다\n");
    }

    exit(EXIT_SUCCESS);
}

int main() {
    char input[100];
    signal(SIGINT, sigintHandler);
    signal(SIGQUIT, sigintHandler);

    while (1) {
        printf("쉘> ");
        fgets(input, sizeof(input), stdin);

        // "exit"를 입력하면 루프를 종료
        if (strcmp(input, "exit\n") == 0) {
            break;
        }

        // '&'를 찾아서 명령을 백그라운드에서 실행
        char *background = strchr(input, '&');
        if (background != NULL) {
            *background = '\0'; // '&' 문자를 NULL 문자로 변경하여 문자열 종료
            printf("백그라운드 실행: %s\n", input);

            pid_t pid = fork();
            if (pid == 0) { // 자식 프로세스
                // 예시로 실행되는 명령어 출력
                printf("입력된 명령어: %s", input);
                // 여기에 명령어 실행 로직 추가 (system 함수를 사용하여 실행 가능)
                exit(EXIT_SUCCESS); // 자식 프로세스 종료
            } else if (pid < 0) {
                fprintf(stderr, "프로세스 생성 오류\n");
            }
            continue;
        }

        // 입력된 명령어 출력
        printf("입력된 명령어: %s", input);
    }

    return 0;
}
