#pragma once
#include "common.h"
#include <sys/msg.h>
#include <cstring>
#include <stdexcept>
#include <string>

struct Msg_t {
    int pid;
    ProcType type;
    
    /*
    * get log file path
    */
    std::string get_log_file_path() const
    {
        return "/tmp/procguard_log/pid_" + std::to_string(pid) + ".log";
    }

    /*
    * get json file path (to save syscalls and signals)
    */
    std::string get_json_file_path() const
    {
        return "/tmp/procguard_json/pid_" + std::to_string(pid) + ".json";
    }

    void delete_tmp_files() const
    {
        std::string log_file = get_log_file_path();
        std::string json_file = get_json_file_path();

        if (remove(log_file.c_str()) != 0) {
            perror(("Failed to remove log file: " + log_file).c_str());
        }
        if (remove(json_file.c_str()) != 0) {
            perror(("Failed to remove json file: " + json_file).c_str());
        }
    }
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