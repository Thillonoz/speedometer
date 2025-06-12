#include "setting.h"
#include "tcpservice.h"

#include <cctype>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

void TCPService::run(void)
{
  socketHandle = -1;
  socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (socketHandle == -1)
  {
    std::cout << "Socket failed to create." << std::endl;
  }
  else
  {
    int opt = 1;
    if (setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
      std::cout << "Socket options failed." << std::endl;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(Setting::TCPIP::PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (0 == bind(socketHandle, (sockaddr *)&address, sizeof(address)))
    {
      while (!end)
      {
        if (0 == listen(socketHandle, 1))
        {
          sockaddr_in client;
          socklen_t clientLength = sizeof(client);

          int connectionHandle = accept(socketHandle, (sockaddr *)&client, &clientLength);
          if (connectionHandle >= 0)
          {
            std::cout << "Connected to client." << std::endl;
            status = true;

            while (1)
            {
              mtx.lock();
              if (BUFLEN != write(connectionHandle, buffer, BUFLEN))
              {
                std::cout << "Failed to write." << std::endl;
                mtx.unlock();
                break;
              }
              mtx.unlock();
              usleep(Setting::INTERVAL * 1000);
            }

            shutdown(connectionHandle, SHUT_RDWR);
            close(connectionHandle);
            status = false;
          }
          else
          {
            std::cout << "Connection failed." << std::endl;
          }
        }
        else
        {
          std::cout << "PORT already used or failed." << std::endl;
        }
      }
    }
    else
    {
      std::cout << "Binding failed." << std::endl;
    }
  }
}