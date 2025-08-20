#include "setting.h"
#include "tcpservice.h"

#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>

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

            uint8_t tempBuffer[BUFLEN]{0};
            while (!end)
            {
              status = true;

              {
                std::scoped_lock<std::mutex> lock(mtx);
                memcpy(tempBuffer, buffer, BUFLEN);
              }

              if (BUFLEN != write(connectionHandle, tempBuffer, BUFLEN))
              {
                std::cout << "Failed to write." << std::endl;
                status = false;
                break;
              }
              std::this_thread::sleep_for(std::chrono::milliseconds(Setting::INTERVAL));
            }

            shutdown(connectionHandle, SHUT_RDWR);
            close(connectionHandle);
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