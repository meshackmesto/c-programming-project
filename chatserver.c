#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_TEXT 512
#define MSG_KEY 1234

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
};

void* listen_for_messages(void* arg) {
    int msgid = *((int*)arg);
    struct msg_buffer message;

    while (1) {
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) > 0) {
            pthread_mutex_lock(&print_mutex);
            printf("[Server Received]: %s\n", message.msg_text);
            pthread_mutex_unlock(&print_mutex);

            // Broadcast to other clients
            message.msg_type = 2;
            msgsnd(msgid, &message, sizeof(message.msg_text), 0);
        }
    }
    return NULL;

}
int main() {
    int msgid;
    pthread_t listener_thread;

    // Create message queue
    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }

    printf("Server started. Waiting for clients...\n");

    // Start thread to listen for messages
    pthread_create(&listener_thread, NULL, listen_for_messages, &msgid);
    pthread_join(listener_thread, NULL);

    return 0;
}