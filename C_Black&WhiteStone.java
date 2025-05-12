#include <stdio.h>
#include <stdlib.h>

int plane[9][9];	// 오목판
int idx = -1;		// 흑돌부터 시작
int u, v;			// 좌표 입력용

int CheckWinner(int x, int y) {
	int stone = plane[y][x];
	int count;

	// 가로 방향 (←,→)
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (x + i > 8) break;
		if (plane[y][x + i] == stone)
			count++;
		else break;
	}
	for (int i = 1; i < 5; i++) {
		if (x - i < 0) break;
		if (plane[y][x - i] == stone)
			count++;
		else break;
	}
	if (count >= 5) return 1;

	// 세로 방향 (↑,↓)
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (y + i > 8) break;
		if (plane[y + i][x] == stone)
			count++;
		else break;
	}
	for (int i = 1; i < 5; i++) {
		if (y - i < 0) break;
		if (plane[y - i][x] == stone)
			count++;
		else break;
	}
	if (count >= 5) return 1;

	// ↘ 대각선 (오른아래, 왼위)
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (x + i > 8 || y + i > 8) break;
		if (plane[y + i][x + i] == stone)
			count++;
		else break;
	}
	for (int i = 1; i < 5; i++) {
		if (x - i < 0 || y - i < 0) break;
		if (plane[y - i][x - i] == stone)
			count++;
		else break;
	}
	if (count >= 5) return 1;

	// ↙ 대각선 (왼아래, 오른위)
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (x - i < 0 || y + i > 8) break;
		if (plane[y + i][x - i] == stone)
			count++;
		else break;
	}
	for (int i = 1; i < 5; i++) {
		if (x + i > 8 || y - i < 0) break;
		if (plane[y - i][x + i] == stone)
			count++;
		else break;
	}
	if (count >= 5) return 1;

	return 0;	// 아직 승부 안 남
}

int InputPosition() {
	while (1) {
		if (idx % 2 == 0)
			printf("\n 흑돌 차례입니다. 좌표 입력 (x,y): ");
		else
			printf("\n 백돌 차례입니다. 좌표 입력 (x,y): ");

		scanf("%d,%d", &u, &v);

		// 유효성 검사: 범위 초과
		if (u < 0 || u > 8 || v < 0 || v > 8) {
			printf(" 범위를 벗어났습니다. 0~8 사이로 입력하세요.\n");
			continue;
		}
		// 이미 돌이 놓인 곳
		if (plane[v][u] != 0) {
			printf(" 이미 돌이 있는 자리입니다. 다시 입력하세요.\n");
			continue;
		}

		// 돌 놓기
		plane[v][u] = (idx % 2 == 0) ? 200 : 100;
		break;
	}
	return 0;
}

int main() {
	// 바둑판 초기화
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			plane[y][x] = 0;

	// 게임 루프
	while (++idx >= 0) {
		// 바둑판 출력
		printf("\n\n현재 오목판:\n");
		for (int y = 0; y < 9; y++) {
			for (int x = 0; x < 9; x++) {
				if (plane[y][x] == 200) printf("B ");
				else if (plane[y][x] == 100) printf("W ");
				else printf("+ ");
			}
			printf("\n");
		}

		// 입력 받기
		InputPosition();

		// 승리 판정
		if (CheckWinner(u, v)) {
			printf("\n %s 승리!\n", (idx % 2 == 0) ? "흑돌" : "백돌");
			break;
		}
	}

	return 0;
}
