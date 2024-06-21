#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <chrono>
#include <syslog.h>
#include <random>
#include <csignal>

volatile sig_atomic_t stop = 0;

void handle_signal(int signal)
{
    if (signal == SIGINT)
    {
        stop = 1;
    }
}

void generate_canbus_data(can_frame &frame, std::mt19937 &mt, std::uniform_int_distribution<int> &dist)
{
    for (size_t i = 0; i < 8; i++)
    {
        frame.data[i] = dist(mt);
    }
}

int main()
{
    std::signal(SIGINT, handle_signal);
    openlog(NULL, 0, LOG_USER);
    syslog(LOG_USER, "Running simcan.");
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // Open a socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        perror("socket");
        return 1;
    }

    // Specify the CAN interface
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    // Bind the socket to the CAN interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    // Prepare a CAN frame
    frame.can_id = 0x123;
    frame.can_dlc = 8;
    std::random_device rd;
    std::mt19937 mt(rd());

    // Define the distribution range (0 to 255)
    std::uniform_int_distribution<int> dist(0, 255);
    generate_canbus_data(frame, mt, dist);

    auto start = std::chrono::steady_clock::now();
    const auto interval = std::chrono::seconds(1);

    while (!stop)
    {
        auto now = std::chrono::steady_clock::now();
        if (now - start >= interval)
        {
            // Send the CAN frame
            generate_canbus_data(frame, mt, dist);
            if (write(s, &frame, sizeof(frame)) != sizeof(frame))
            {
                perror("write");
                return 1;
            }
            start = now;
        }
    }

    // Close the socket
    close(s);
    syslog(LOG_USER, "close simcan.");
    closelog();
    exit(EXIT_SUCCESS);
}
