#include "tcpservice.h"
#include "comservice.h"
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

void TCPService::run(void)
{
    while (1)
    {
        // Create socket and check
        sockfd = -1;
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sockfd == -1)
        {
            std::cout << "Failed to create the socket..." << std::endl;
        }
        else
        {
            int opt = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            {
                std::cout << "Socket options failed." << std::endl;
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

                while (status)
                {
                    {
                        std::lock_guard<std::mutex> lock(mtx);

                        ssize_t bytes_read = read(sockfd, buffer, BUFLEN);
                        if (bytes_read <= 0)
                        {
                            std::cout << "Failed to read or connection closed by server." << std::endl;
                            status = false;
                            break;
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

                    usleep(Setting::INTERVAL * 1000);
                }

                shutdown(sockfd, SHUT_RDWR);
                close(sockfd);
                status = false;
            }
            else
            {
                std::cout << "Connection to the server failed." << std::endl;
                usleep(Setting::INTERVAL * 2000);
            }
        }
        usleep(Setting::INTERVAL * 1000);
    }
}