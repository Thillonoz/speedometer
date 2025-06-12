#ifndef TCPCOM_H
#define TCPCOM_H

#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include "comservice.h"

class TCPService : public COMService
{
    int sockfd;
    std::atomic<bool> end{false};
    std::thread thrd{&TCPService::run, this};

    void run(void) override;
   

public:
    TCPService() = default;
    ~TCPService()
    {
        end = true;
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        thrd.join();
    }
};

#endif