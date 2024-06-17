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

int main()
{
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
    frame.can_dlc = 2;
    frame.data[0] = 0xAB;
    frame.data[1] = 0xCD;

    // Send the CAN frame
    if (write(s, &frame, sizeof(frame)) != sizeof(frame))
    {
        perror("write");
        return 1;
    }

    // Close the socket
    close(s);
    syslog(LOG_USER, "close simcan.");
    closelog();
    exit(EXIT_SUCCESS);
}
