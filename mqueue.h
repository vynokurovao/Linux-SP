#ifndef MQUEUE_H
#define MQUEUE_H

#include <string>

/*!
 * just a thin wrapper around System V message queue.
 * it's used for convenience.
 */

class MQueue
{
public:
    enum class MessageType
    {
        ManagerToProc1,
        ManagerToProc2,
        ResultReturn
    };

    MQueue(const std::string &path, int mode);
    ~MQueue();

    bool isValid() const;
    
    std::string path() const;
    int mode() const;

    void send(int value, MessageType type);
    int read(MessageType type);

private:
    std::string m_path;
    int m_mode;
    bool m_isValid;
    int m_queueId;

};

#endif // MQUEUE_H
