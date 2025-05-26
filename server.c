#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h> // 스레드 사용을 위한 헤더

#define PORT 12345
#define BUFFER_SIZE 2048
#define MAX_CLIENTS 10 // 최대 클라이언트 수

// 클라이언트 소켓들을 저장할 배열과 이를 보호할 뮤텍스
int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t client_sockets_mutex;

// 각 클라이언트 연결을 처리할 함수 (스레드에서 실행됨)
void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int str_len;
    int current_client_idx = -1; // 현재 스레드가 담당하는 클라이언트의 인덱스

    // 현재 클라이언트 소켓의 인덱스 찾기 (브로드캐스트 시 자신 제외 위함)
    pthread_mutex_lock(&client_sockets_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_sock) {
            current_client_idx = i;
            break;
        }
    }
    pthread_mutex_unlock(&client_sockets_mutex);

    printf("Client %d connected (socket %d).\n", current_client_idx + 1, client_sock);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        // 클라이언트로부터 메시지 수신
        str_len = read(client_sock, buffer, BUFFER_SIZE - 1);

        if (str_len == -1) {
            perror("Error: read from client failed");
            break;
        }
        if (str_len == 0) { // 클라이언트가 연결을 종료한 경우
            printf("Client %d disconnected (socket %d).\n", current_client_idx + 1, client_sock);
            break;
        }

        buffer[str_len] = '\0'; // 널 문자 추가

        printf("Received from Client %d: %s\n", current_client_idx + 1, buffer);

        // 'quit' 메시지 수신 시 해당 클라이언트 연결 종료
        if (strcmp(buffer, "quit") == 0) {
            printf("Client %d requested quit. Disconnecting.\n", current_client_idx + 1);
            break;
        }

        // 수신된 메시지를 다른 모든 클라이언트에게 브로드캐스트
        pthread_mutex_lock(&client_sockets_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] != 0 && client_sockets[i] != client_sock) { // 자신을 제외한 다른 클라이언트에게
                if (write(client_sockets[i], buffer, str_len) == -1) {
                    perror("Error: write to other client failed");
                    // 여기서 해당 클라이언트 소켓을 제거하는 로직 추가 가능
                }
            }
        }
        pthread_mutex_unlock(&client_sockets_mutex);
    }

    // 클라이언트 소켓 닫기 및 배열에서 제거
    close(client_sock);
    pthread_mutex_lock(&client_sockets_mutex);
    if (current_client_idx != -1) {
        client_sockets[current_client_idx] = 0; // 해당 슬롯 비움
    }
    client_count--;
    printf("Current connected clients: %d\n", client_count);
    pthread_mutex_unlock(&client_sockets_mutex);

    pthread_exit(NULL); // 스레드 종료
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    pthread_t thread_id;

    // 뮤텍스 초기화
    pthread_mutex_init(&client_sockets_mutex, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0; // 배열 초기화
    }

    // 1. 서버 소켓 생성
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Error: server socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Server socket created successfully.\n");

    // Time-wait 상태 문제 해결 (선택 사항)
    // 서버 종료 후 재실행 시 "Address already in use" 에러 방지
    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


    // 2. 서버 주소 설정 및 바인딩
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP 주소로부터 연결 허용
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Server bound to port %d.\n", PORT);

    // 3. 클라이언트 연결 대기 (최대 MAX_CLIENTS 개의 대기열)
    if (listen(server_sock, MAX_CLIENTS) == -1) {
        perror("Error: listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // 4. 클라이언트 연결 수락 및 스레드 생성 루프
    while (1) {
        client_addr_size = sizeof(client_addr);
        // 클라이언트 연결 요청 수락
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_sock == -1) {
            perror("Error: accept failed");
            continue; // 에러 발생해도 서버는 계속 실행
        }

        // 연결된 클라이언트를 client_sockets 배열에 추가
        pthread_mutex_lock(&client_sockets_mutex);
        if (client_count < MAX_CLIENTS) {
            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) { // 빈 슬롯 찾기
                    client_sockets[i] = client_sock;
                    client_count++;
                    break;
                }
            }
            if (i == MAX_CLIENTS) { // 빈 슬롯을 찾지 못한 경우 (불가능한 경로)
                printf("Error: Max clients reached but no empty slot found.\n");
                close(client_sock); // 연결 거부
            } else {
                // 클라이언트 처리를 위한 스레드 생성
                if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_sock) != 0) {
                    perror("Error: pthread_create failed");
                    close(client_sock);
                    client_sockets[i] = 0; // 스레드 생성 실패 시 슬롯 다시 비움
                    client_count--;
                } else {
                    pthread_detach(thread_id); // 스레드가 종료될 때 자원 자동 해제
                }
            }
        } else {
            printf("Max clients (%d) reached. Connection rejected from %s:%d.\n",
                   MAX_CLIENTS, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            const char *reject_msg = "Server full. Please try again later.\n";
            write(client_sock, reject_msg, strlen(reject_msg));
            close(client_sock); // 연결 거부
        }
        pthread_mutex_unlock(&client_sockets_mutex);
    }

    // 서버 소켓 닫기 (실제로는 무한 루프이므로 잘 호출되지 않음)
    close(server_sock);
    // 뮤텍스 파괴
    pthread_mutex_destroy(&client_sockets_mutex);
    printf("Server socket closed.\n");

    return 0;
}
