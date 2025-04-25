#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define MAX_TEXT 512
#define MSG_KEY 1234

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
};

void* receive_messages(void* arg) {
    int msgid = *((int*)arg);
    struct msg_buffer message;

    while (1) {
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 2, 0) > 0) {
            printf("\n[Broadcast]: %s\n> ", message.msg_text);
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    int msgid;
    pthread_t recv_thread;
    struct msg_buffer message;

    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }

    // Start receive thread
    pthread_create(&recv_thread, NULL, receive_messages, &msgid);

    while (1) {
        printf("> ");
        fgets(message.msg_text, MAX_TEXT, stdin);
        message.msg_type = 1;
        msgsnd(msgid, &message, sizeof(message.msg_text), 0);
    }

    return 0;
}