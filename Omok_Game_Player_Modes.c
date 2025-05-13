/*
 * 오목 게임 (사람 vs 사람 / 사람 vs 컴퓨터)
 * - 폭포수 모델 기반 설계
 * - 핵심 포인트: 게임 모드 분기, 컴퓨터 자동 수, 유효성 검사, 승리 판정
 * - 재귀 함수 사용: 게임 루프를 재귀로 구현
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
#define EMPTY 0
#define BLACK 200
#define WHITE 100

int board[SIZE][SIZE] = {0};
int idx = -1;
int u, v;

int mode; // 게임 모드: 1 = 사람 vs 사람, 2 = 사람 vs 컴퓨터

// 승리 조건 검사 함수 (4방향 검사)
int CheckWinner(int x, int y) {
    int stone = board[y][x];
    int count;
    int dx[] = {1, 0, 1, -1};
    int dy[] = {0, 1, 1, 1};

    for (int d = 0; d < 4; d++) {
        count = 1;
        for (int i = 1; i < 5; i++) {
            int nx = x + dx[d] * i;
            int ny = y + dy[d] * i;
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) break;
            if (board[ny][nx] == stone) count++;
            else break;
        }
        for (int i = 1; i < 5; i++) {
            int nx = x - dx[d] * i;
            int ny = y - dy[d] * i;
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) break;
            if (board[ny][nx] == stone) count++;
            else break;
        }
        if (count >= 5) return 1;
    }
    return 0;
}

// 돌 두기 함수 (사람 또는 컴퓨터)
int InputPosition(int mode) {
    while (1) {
        if (idx % 2 == 0) {
            printf("\n흑돌 차례입니다. 좌표 입력 (x,y): ");
            scanf("%d,%d", &u, &v);
        } else {
            if (mode == 2) {
                printf("\n백돌(컴퓨터) 차례입니다. 자동으로 둡니다\n");
                u = rand() % SIZE;
                v = rand() % SIZE;
                while (board[v][u] != EMPTY) {
                    u = rand() % SIZE;
                    v = rand() % SIZE;
                }
            } else {
                printf("\n백돌 차례입니다. 좌표 입력 (x,y): ");
                scanf("%d,%d", &u, &v);
            }
        }

        if (u < 0 || u >= SIZE || v < 0 || v >= SIZE) {
            printf("잘못된 범위입니다. 다시 입력하세요\n");
            continue;
        }
        if (board[v][u] != EMPTY) {
            printf("이미 돌이 놓인 자리입니다. 다시 입력하세요\n");
            continue;
        }

        board[v][u] = (idx % 2 == 0) ? BLACK : WHITE;
        break;
    }
    return 0;
}

// 현재 오목판 출력 함수
void PrintBoard() {
    printf("\n현재 오목판 상태\n");
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == BLACK) printf("B ");
            else if (board[y][x] == WHITE) printf("W ");
            else printf("+ ");
        }
        printf("\n");
    }
}

// 재귀적으로 게임을 진행하는 함수
void PlayGame() {
    idx++;
    PrintBoard();
    InputPosition(mode);

    if (CheckWinner(u, v)) {
        PrintBoard();
        printf("\n승리: %s\n", (idx % 2 == 0) ? "흑돌" : "백돌");
        return; // 종료 조건
    }

    // 재귀 호출로 다음 턴 진행
    PlayGame();
}

// 메인 함수 - 게임 모드 선택 및 초기화, 재귀 함수 진입
int main() {
    srand(time(NULL));

    printf("게임 모드를 선택하세요\n");
    printf("1. 사람 vs 사람\n");
    printf("2. 사람 vs 컴퓨터\n");
    scanf("%d", &mode);

    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++)
            board[y][x] = EMPTY;

    PlayGame(); // 재귀 함수로 게임 시작

    return 0;
}
