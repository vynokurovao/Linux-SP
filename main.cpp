#include <iostream>
#include <thread>
#include <future>

#include <unistd.h>

#include "mqueue.h"

template <class R>
bool is_ready(const std::future<R> &f)
{
    return f.wait_for(std::chrono::seconds(0));
}

int process_result(MQueue &queue)
{
    const int res1 = queue.read(MQueue::MessageType::ResultReturn);
    std::cout << "Received value: " << res1 << std::endl;

    if (res1 == 0)
        return 0;

    const int res2 = queue.read(MQueue::MessageType::ResultReturn);
    std::cout << "Received value: " << res2 << std::endl;

    return res1 * res2;
}

void spawn_process(const std::string &binName)
{
    const pid_t pid = fork();

    if (pid < 0) {
        perror("Can't fork.");
        exit(-1);
    } else if (pid == 0) {
        execl(binName.c_str(), binName.c_str());
    }

    std::cout << "Spawned process: " << binName << std::endl;
}

int main(int argc, char **argv)
{
    MQueue queue(".", 0660);

    if (!queue.isValid()) {
        std::cout << "Can't open message queue. Exiting ..." << std::endl;
        exit(-1);
    }

    int x_value = 0;
    std::cout << "Please, enter X value:" << std::endl;
    std::cin >> x_value;

    spawn_process("proc1");
    spawn_process("proc2");

    queue.send(x_value, MQueue::MessageType::ManagerToProc1);
    std::cout << "Value is sent to proc#1" << std::endl;

    queue.send(x_value, MQueue::MessageType::ManagerToProc2);
    std::cout << "Value is sent to proc#2" << std::endl;

    std::future<int> result = std::async(std::launch::async, process_result, queue);
    
    while (true) {
        result.wait_for(std::chrono::seconds(5));

        if (is_ready(result)) {
            break;
        }

        int command = 0;
        std::cout << "1. Wait more\n2. Stop\n3. Ask no more" << std::endl;
        std::cin >> command;
        
        if (command == 1) {
            continue;
        } else if (command == 2) {
            std::cout << "Stopping ..." << std::endl;
            exit(0);
        } else if (command == 3) {
            result.wait();
            break;
        } else {
            std::cout << "Wrong index. Continuing ..." << std::endl;
            continue;
        }
    }

    std::cout << "Result: " << result.get() << std::endl;
    return 0;
}
