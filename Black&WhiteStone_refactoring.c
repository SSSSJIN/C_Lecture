#include <stdio.h>
#include <stdlib.h>

// 오목판 배열: 9x9 바둑판을 2차원 배열로 구현 (0: 비어있음, 200: 흑돌, 100: 백돌)
int plane[9][9];

// 턴 인덱스 (0부터 시작해서 홀수/짝수로 흑백 결정)
int idx = -1;

// 사용자 입력 좌표
int u, v;

/**
 * 승리 판정 함수
 * 현재 둔 돌의 좌표(x, y)를 기준으로 4방향(가로, 세로, 대각선 ↘, ↙) 검사
 * 연속 5개 이상 같은 돌이 있는지 확인
 */
int CheckWinner(int x, int y) {
	int stone = plane[y][x]; // 현재 위치의 돌 색상
	int count;

	// 가로 방향 (좌우)
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (x + i > 8 || plane[y][x + i] != stone) break;
		count++;
	}
	for (int i = 1; i < 5; i++) {
		if (x - i < 0 || plane[y][x - i] != stone) break;
		count++;
	}
	if (count >= 5) return 1;

	// 세로 방향 (상하)
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (y + i > 8 || plane[y + i][x] != stone) break;
		count++;
	}
	for (int i = 1; i < 5; i++) {
		if (y - i < 0 || plane[y - i][x] != stone) break;
		count++;
	}
	if (count >= 5) return 1;

	// 대각선 ↘ 방향
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (x + i > 8 || y + i > 8 || plane[y + i][x + i] != stone) break;
		count++;
	}
	for (int i = 1; i < 5; i++) {
		if (x - i < 0 || y - i < 0 || plane[y - i][x - i] != stone) break;
		count++;
	}
	if (count >= 5) return 1;

	// 대각선 ↙ 방향
	count = 1;
	for (int i = 1; i < 5; i++) {
		if (x - i < 0 || y + i > 8 || plane[y + i][x - i] != stone) break;
		count++;
	}
	for (int i = 1; i < 5; i++) {
		if (x + i > 8 || y - i < 0 || plane[y - i][x + i] != stone) break;
		count++;
	}
	if (count >= 5) return 1;

	// 아직 승자가 없음
	return 0;
}

/**
 * 사용자 좌표 입력 함수
 * - 범위를 벗어나거나 이미 놓인 자리는 다시 입력 받도록 유도
 */
int InputPosition() {
	while (1) {
		if (idx % 2 == 0)
			printf("\n흑돌 차례입니다. 좌표 입력 (x,y): ");
		else
			printf("\n백돌 차례입니다. 좌표 입력 (x,y): ");

		scanf("%d,%d", &u, &v);

		// 유효성 검사: 범위 체크
		if (u < 0 || u > 8 || v < 0 || v > 8) {
			printf("❌ 범위를 벗어났습니다. 0~8 사이로 입력하세요.\n");
			continue;
		}
		// 중복 체크
		if (plane[v][u] != 0) {
			printf("❌ 이미 돌이 있는 자리입니다. 다시 입력하세요.\n");
			continue;
		}

		// 돌 놓기 (흑: 200, 백: 100)
		plane[v][u] = (idx % 2 == 0) ? 200 : 100;
		break;
	}
	return 0;
}

/**
 * 메인 함수 - 전체 게임 흐름 관리
 */
int main() {
	// [초기화 단계]
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			plane[y][x] = 0;

	// [실행 단계]
	while (++idx >= 0) {
		// 바둑판 출력
		printf("\n\n현재 오목판 상태:\n");
		for (int y = 0; y < 9; y++) {
			for (int x = 0; x < 9; x++) {
				if (plane[y][x] == 200) printf("B ");
				else if (plane[y][x] == 100) printf("W ");
				else printf("+ ");
			}
			printf("\n");
		}

		// 좌표 입력
		InputPosition();

		// 승리 판정
		if (CheckWinner(u, v)) {
			printf("\n🎉 %s 승리!\n", (idx % 2 == 0) ? "흑돌" : "백돌");
			break;
		}
	}

	// [유지보수 단계는 향후 기능 확장 시 고려]
	return 0;
}
