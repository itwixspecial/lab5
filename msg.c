#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_SEND_SIZE 80

struct mymsgbuf {
  long mtype;
  char mtext[MAX_SEND_SIZE];
};

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text);
void read_message(int qid, struct mymsgbuf *qbuf, long type);
void remove_queue(int qid);
void change_queue_mode(int qid, char *mode);

int main(int argc, char *argv[])
{
  key_t  key;
  int    msgqueue_id;
  struct mymsgbuf qbuf;


  key = ftok(".", 'm');
  msgqueue_id = msgget(key, IPC_CREAT | 0660);

  switch(tolower(argv[1][0]))
  {
    case 's':
      send_message(msgqueue_id, (struct mymsgbuf *)&qbuf, atol(argv[2]), argv[3]);
      break;
    case 'r':
      read_message(msgqueue_id, &qbuf, atol(argv[2]));
      break;
    case 'd':
      remove_queue(msgqueue_id);
      break;
    case 'm':
      change_queue_mode(msgqueue_id, argv[2]);
      break;
    default:
      exit(1);
  }

  return 0;
}

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text)
{
  printf("Sending a message ...\n");
  qbuf->mtype = type;
  strcpy(qbuf->mtext, text);
  msgsnd(qid, (struct msgbuf *)qbuf, strlen(qbuf->mtext)+1, 0);
}

void read_message(int qid, struct mymsgbuf *qbuf, long type)
{
  printf("Reading a message ...\n");
  qbuf->mtype = type;
  msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
  printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
}

void remove_queue(int qid)
{
  msgctl(qid, IPC_RMID, 0);
}

void change_queue_mode(int qid, char *mode)
{
  struct msqid_ds myqueue_ds;
  msgctl(qid, IPC_STAT, &myqueue_ds);
  sscanf(mode, "%ho", &myqueue_ds.msg_perm.mode);
  msgctl(qid, IPC_SET, &myqueue_ds);
}
