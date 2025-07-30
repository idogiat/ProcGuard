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

    /* send msg to queue
     * return -1 if field
    */
    int send(const Msg_t& msg);
    
    /* recieve msg from queue
     * return -1 if field
    */
    ssize_t receive(Msg_t& out_msg_t);

    /* remove queue from memory (optional)
    * return -1 if field
    */
    int remove();

private:
    int msqid_;
};