#include "tcpservice.h"
#include "comservice.h"
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>

void TCPService::run(void)
{
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(sockaddr_in));

    // Assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(Setting::TCPIP::PORT);
    servaddr.sin_addr.s_addr = inet_addr(Setting::TCPIP::IP);

    while (!end)
    {
        // Create socket and check
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sockfd > -1)
        {

            // Connect to the server
            if (0 == connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)))
            {
                std::cout << "Connected to the server."
                          << std::endl
                          << std::endl;

                uint8_t temp[sizeof(buffer)]{0};
                status = true;
                
                while (!end)
                {
                    {
                        
                        if (sizeof(temp) != read(sockfd, temp, sizeof(temp)))
                        {
                            status = false;
                            break;
                        }
                        else 
                        {
                            std::scoped_lock<std::mutex> locker{mtx};

                             std::memcpy(buffer, temp, sizeof(buffer));
                        }
                    }

                    std::cout
                        << "Received: "
                        << "Speed: " << TCPService::getSpeed()
                        << " Temperature: " << TCPService::getTemperature()
                        << " Battery: " << TCPService::getBatteryLevel()
                        << " Left: " << TCPService::getLeftLight()
                        << " Right: " << TCPService::getRightLight()
                        << std::endl
                        << std::endl;

                        std::this_thread::sleep_for(std::chrono::milliseconds(Setting::INTERVAL));

                }

                shutdown(sockfd, SHUT_RDWR);
                close(sockfd);
                status = false;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(Setting::INTERVAL * 2));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Setting::INTERVAL));
    }
}