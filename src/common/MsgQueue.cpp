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

int MsgQueue::send(const Msg_t& msg)
{
    return msgsnd(msqid_, &msg, sizeof(Msg_t), 0);
}

ssize_t MsgQueue::receive(Msg_t& msg)
{
    return msgrcv(msqid_, &msg, sizeof(Msg_t), 0, 0);
}

int MsgQueue::remove()
{
    
    return msgctl(msqid_, IPC_RMID, nullptr);
}
