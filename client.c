#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "10.0.2.15" // <-- 당신의 서버 IP 주소로 수정되었습니다!
#define PORT 12345            // 서버 포트 번호
#define BUFFER_SIZE 2048      // 메시지 버퍼 크기

int main(int argc, char *argv[]) { // 클라이언트 번호를 인자로 받기 위해 argc, argv 사용
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int str_len;
    char client_id[20]; // 클라이언트 식별자 (예: "Client1")

    // 클라이언트 번호를 인자로 받음 (예: ./client 1)
    if (argc != 2) {
        fprintf(stderr, "사용법: %s <클라이언트_번호>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    sprintf(client_id, "Client%s", argv[1]); // "Client1", "Client2" 등으로 설정

    // 1. 소켓 생성 (IPv4, TCP)
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Error: socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("%s socket created successfully.\n", client_id);

    // 2. 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // 지정된 서버 IP 주소 사용
    server_addr.sin_port = htons(PORT);

    // 3. 서버에 연결
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected from %s to server %s:%d.\n", client_id, SERVER_IP, PORT);

    // 4. 채팅 루프
    while (1) {
        printf("%s> ", client_id); // 프롬프트에 클라이언트 ID 표시
        fgets(buffer, BUFFER_SIZE, stdin); // 표준 입력(키보드)에서 메시지 읽기
        buffer[strcspn(buffer, "\n")] = 0; // 입력 버퍼에서 개행 문자 제거

        // 'quit' 입력 시 클라이언트 종료
        if (strcmp(buffer, "quit") == 0) {
            printf("%s requested quit. Exiting.\n", client_id);
            if (write(sock, buffer, strlen(buffer)) == -1) { // 서버에게도 'quit' 전송
                perror("Error: write failed during quit");
            }
            break; // 루프를 빠져나가 프로그램 종료
        }

        // 메시지에 클라이언트 식별자 추가
        char send_msg[BUFFER_SIZE + 30]; // 버퍼 크기 확장 (snprintf 경고 해결)
        snprintf(send_msg, sizeof(send_msg), "%s: %s", client_id, buffer);

        // 메시지 전송
        if (write(sock, send_msg, strlen(send_msg)) == -1) {
            perror("Error: write failed");
            break;
        }

        // 5. 서버로부터 메시지 수신 (이 부분은 Blocking Read이므로, 서버에서 메시지 올 때까지 대기)
        memset(buffer, 0, BUFFER_SIZE);
        str_len = read(sock, buffer, BUFFER_SIZE - 1);
        
        if (str_len == -1) {
            perror("Error: read failed");
            break;
        }
        if (str_len == 0) { // 서버가 연결을 종료한 경우
            printf("Server disconnected.\n");
            break;
        }
        buffer[str_len] = '\0';
        printf("Received from server: %s\n", buffer); // 수신 메시지 출력

        // 서버로부터 "quit" 메시지 수신 시 클라이언트 종료 (서버가 전체 종료를 명령할 때)
        if (strcmp(buffer, "quit") == 0) {
            printf("Server requested quit. Exiting.\n");
            break;
        }
    }

    // 6. 소켓 닫기
    close(sock);
    printf("%s socket closed.\n", client_id);

    return 0;
}
