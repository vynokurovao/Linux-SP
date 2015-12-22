#include "mqueue.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include <iostream>

namespace
{
    const unsigned cMaxLength = 80;

    struct MsgBuf
    {
        long type;
        char text[cMaxLength];
    };
}


MQueue::MQueue(const std::string &path, int mode)
    : m_path(path)
    , m_mode(mode)
    , m_queueId(-1)
    , m_isValid(false)
{
    const key_t ipcKey = ftok(path.c_str(), 'm');

    if ((m_queueId = msgget(ipcKey, IPC_CREAT | mode)) != -1) {
        m_isValid = true;
    }
}

MQueue::~MQueue()
{
}

bool MQueue::isValid() const
{
    return m_isValid;
}

std::string MQueue::path() const
{
    return m_path;
}

int MQueue::mode() const
{
    return m_mode;
}

void MQueue::send(int value, MQueue::MessageType type)
{
    if (m_isValid) {
        MsgBuf buf;
        buf.type = static_cast<long>(type) + 1;
        sprintf(buf.text, "%d", value);

        if (msgsnd(m_queueId, &buf, strlen(buf.text) + 1, 0) == -1) {
            perror("Can't send message.");
            exit(-1);
        }
    }
}

int MQueue::read(MQueue::MessageType type)
{
    if (m_isValid) {
        MsgBuf buf;
        buf.type = static_cast<long>(type) + 1;
        msgrcv(m_queueId, &buf, cMaxLength, buf.type, 0);
        return atoi(buf.text);
    }

    return -1;
}
