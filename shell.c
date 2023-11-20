#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sigintHandler(int signum)
{
    if (signum == SIGINT)
    {
        printf("\nCtrl-C가 눌렸습니다. 프로그램을 종료합니다.\n");
    }
    else if (signum == SIGQUIT)
    {
        printf("\nnCtrl-Z가 눌렸습니다. 프로그램을 종료합니다\n");
    }

    exit(EXIT_SUCCESS);
}

int main()
{
    char input[100];
    signal(SIGINT, sigintHandler);
    signal(SIGQUIT, sigintHandler);

    while (1)
    {
        printf("쉘> ");
        fgets(input, sizeof(input), stdin);

        // "exit"를 입력하면 루프를 종료
        if (strcmp(input, "exit\n") == 0)
        {
            break;
        }

        // 명령어 처리 등 프로그램 로직 추가

        // &
        char *background = strchr(input, '&');
        if (background != NULL)
        {
            *background = '\0';
            printf("백그라운드 실행: %s\n", input);
            continue;
        }

        // 예시로 입력된 명령어 출력
        printf("입력된 명령어: %s", input);
    }

    return 0;
}
