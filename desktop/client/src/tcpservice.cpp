#include "tcpservice.h"
#include "comservice.h"
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

void TCPService::run(void)
{
    // Create socket and check
    sockfd = -1;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sockfd == -1)
    {
        std::cout << "Failed to create the socket..." << std::endl;
    }

    sockaddr_in servaddr{};

    // Assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(Setting::TCPIP::PORT);
    servaddr.sin_addr.s_addr = inet_addr(Setting::TCPIP::IP);

    // Connect to the server
    if (0 == connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)))
    {
        std::cout << "Connected to the server."
                  << std::endl
                  << std::endl;
        status = true;

        while (1)
        {
            mtx.lock();
                bzero(buffer, sizeof(buffer));

            // Receive data from the server and store it in buffer
            if (BUFLEN != read(sockfd, buffer, BUFLEN))
            {
                std::cout << "Failed to read ..." << std::endl;
                 status = false;
                mtx.unlock(); break;
            }
            mtx.unlock();

                    std::cout
                << "Received: " << "Speed: " << TCPService::getSpeed() << " Temperature: " << TCPService::getTemperature() << " Battery: " << TCPService::getBatteryLevel() << " Left: " << TCPService::getLeftLight() << " Right: " << TCPService::getRightLight()
                << std::endl
                << std::endl;

            usleep(Setting::INTERVAL * 1000);
        }

        status = false;
    }
    else
    {
        std::cout << "Connection to the server failed." << std::endl;
    }
}