#include "MsgQueue.hpp"
#include <sys/ipc.h>
#include <iostream>


MsgQueue::MsgQueue()
{
    msqid_ = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0666);
    if (msqid_ == -1)
    {
        throw std::runtime_error("Failed to create/get message queue");
    }
}

MsgQueue::~MsgQueue() {}

void MsgQueue::send(const Msg_t& msg)
{
    if (msgsnd(msqid_, &msg, sizeof(Msg_t), 0) == -1)
    {
        throw std::runtime_error("msgsnd failed");
    }
}

void MsgQueue::receive(Msg_t& msg)
{
    ssize_t ret = msgrcv(msqid_, &msg, sizeof(Msg_t), 0, 0);
    if (ret == -1)
    {
        throw std::runtime_error("msgrcv failed");
    }
}

void MsgQueue::remove()
{
    if (msgctl(msqid_, IPC_RMID, nullptr) == -1)
    {
        throw std::runtime_error("msgctl IPC_RMID failed");
    }
}
