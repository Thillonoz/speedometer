#ifndef TCPCOM_H
#define TCPCOM_H

#include "comservice.h"
#include <thread>
#include <unistd.h>
#include <sys/socket.h>

class TCPService : public COMService
{
  int socketHandle;
  std::atomic<bool> end{false};

  std::thread thrd{&TCPService::run, this};
  void run(void) override;

public:
  TCPService() = default;
  ~TCPService()
  {
    end = true;
    shutdown(socketHandle, SHUT_RDWR);
    close(socketHandle);
    thrd.join();
  }
};

#endif