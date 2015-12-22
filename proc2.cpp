#include <thread>
#include <chrono>
#include <iostream>

#include <signal.h>
#include <sys/prctl.h>

#include "mqueue.h"


int f(int x)
{
    std::this_thread::sleep_for(std::chrono::seconds(20));
    return 3;
}

int main(int argc, char **argv)
{
    prctl(PR_SET_PDEATHSIG, SIGTERM);

    MQueue queue(".", 0660);

    if (!queue.isValid()) {
        std::cout << "Can't open queue. Exiting ..." << std::endl;
        return -1;
    }

    const int value = queue.read(MQueue::MessageType::ManagerToProc2);
    std::cout << "Proc#2 received: " << value << std::endl;

    const int returnValue = f(value);
    queue.send(returnValue, MQueue::MessageType::ResultReturn);
    std::cout << "Proc#2 sent: " << returnValue << std::endl;

    return 0;
}
