#pragma once
#include "common.h"
#include <sys/msg.h>
#include <cstring>
#include <stdexcept>

struct Msg_t {
    int pid;
    ProcType type;
};

class MsgQueue {
public:
    static constexpr key_t MSG_QUEUE_KEY = 0x1234;
    static constexpr size_t MAX_TEXT_SIZE = 256;


    MsgQueue();
    ~MsgQueue();

    void send(const Msg_t& msg);
    void receive(Msg_t& out_msg_t);
    void remove();

private:
    int msqid_;
};