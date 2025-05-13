#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
#define EMPTY 0
#define BLACK 200
#define WHITE 100

// 오목판 선언 (전역)
int board[SIZE][SIZE] = {0};

// 함수 선언
void PlayGame(int turn);
int CheckWinner(int x, int y, int** board);
void PlaceStone(int player, int* x, int* y);

/**
 * 승리 조건 검사 함수
 * - 포인터를 사용하여 오목판 접근
 * - 4방향으로 5개 돌이 이어졌는지 확인
 */
int CheckWinner(int x, int y, int** board) {
	int stone = board[y][x];
	int count;

	// 방향 벡터: [가로, 세로, ↘, ↙]
	int dx[] = {1, 0, 1, -1};
	int dy[] = {0, 1, 1, 1};

	for (int d = 0; d < 4; d++) {
		count = 1;
		// 정방향
		for (int i = 1; i < 5; i++) {
			int nx = x + dx[d]*i;
			int ny = y + dy[d]*i;
			if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) break;
			if (board[ny][nx] == stone) count++;
			else break;
		}
		// 역방향
		for (int i = 1; i < 5; i++) {
			int nx = x - dx[d]*i;
			int ny = y - dy[d]*i;
			if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) break;
			if (board[ny][nx] == stone) count++;
			else break;
		}
		if (count >= 5) return 1;
	}
	return 0;
}

/**
 * 돌을 둘 좌표를 선택하는 함수
 * - 무작위로 비어있는 좌표를 찾아서 반환
 * - x, y는 포인터로 전달받아 바뀐 값이 반영됨
 */
void PlaceStone(int player, int* x, int* y) {
	while (1) {
		*x = rand() % SIZE;
		*y = rand() % SIZE;
		if (board[*y][*x] == EMPTY) {
			board[*y][*x] = (player == 0) ? BLACK : WHITE;
			break;
		}
	}
}

/**
 * 메인 게임 재귀 함수
 * - 각 턴마다 돌을 놓고 결과 출력
 * - 승자가 생기면 종료
 * - 재귀적으로 다음 턴 호출
 */
void PlayGame(int turn) {
	int player = turn % 2; // 0: 흑, 1: 백
	int x, y;

	// 돌 두기
	PlaceStone(player, &x, &y);

	// 바둑판 출력
	printf("\n[%s 차례] (%d, %d) 위치에 돌을 놓았습니다.\n", 
		player == 0 ? "흑돌" : "백돌", x, y);

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (board[i][j] == BLACK) printf("B ");
			else if (board[i][j] == WHITE) printf("W ");
			else printf("+ ");
		}
		printf("\n");
	}

	// 승리 판정
	int* boardPtr[SIZE];
	for (int i = 0; i < SIZE; i++) boardPtr[i] = board[i];

	if (CheckWinner(x, y, boardPtr)) {
		printf("\n %s 승리!\n", player == 0 ? "흑돌" : "백돌");
		return;
	}

	// 다음 턴 (재귀 호출)
	PlayGame(turn + 1);
}

/**
 * 메인 함수
 * - 랜덤 시드 초기화
 * - 게임 시작
 */
int main() {
	srand(time(NULL));
	printf(" 오목 컴퓨터 대결을 시작합니다!\n");
	PlayGame(0);
	return 0;
}
