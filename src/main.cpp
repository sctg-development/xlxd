//
//  main.cpp
//  xlxd
//
//  Created by Jean-Luc Deltombe (LX3JL) on 31/10/2015.
//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//
// ----------------------------------------------------------------------------
//    This file is part of xlxd.
//
//    xlxd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    xlxd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "main.h"
#include "creflector.h"
#include "cconfig.h"

#include "syslog.h"
#include <csignal>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <cstring>
#ifdef RUN_AS_DAEMON
#include <systemd/sd-daemon.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////
// global objects

CReflector g_Reflector;

////////////////////////////////////////////////////////////////////////////////////////
// function declaration

#include "cusers.h"

// Check if a string is a valid IPv4 address
static bool is_valid_ipv4(const char *str)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, str, &(sa.sin_addr));
    return result != 0;
}

// Get IPv4 address of a network interface
static std::string get_interface_ipv4(const char *interface_name)
{
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];
    std::string result;

    if (getifaddrs(&ifaddr) == -1)
    {
        std::cerr << "Error: Failed to get interface addresses" << std::endl;
        return result;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == nullptr)
            continue;

        // Check if this is the interface we're looking for and it's IPv4
        if (strcmp(ifa->ifa_name, interface_name) == 0 &&
            ifa->ifa_addr->sa_family == AF_INET)
        {

            int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);

            if (s == 0)
            {
                result = std::string(host);
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    return result;
}

// Returns caught termination signal or -1 on error
static int wait_for_termination()
{
    sigset_t waitset;
    siginfo_t siginfo;

    sigemptyset(&waitset);
    sigaddset(&waitset, SIGTERM);
    sigaddset(&waitset, SIGINT);
    sigaddset(&waitset, SIGQUIT);
    sigaddset(&waitset, SIGHUP);
    pthread_sigmask(SIG_BLOCK, &waitset, nullptr);

    // Wait for a termination signal
    int result = -1;
    while (result < 0)
    {
        result = sigwaitinfo(&waitset, &siginfo);
        if (result == -1 && errno == EINTR)
        {
            // try again
            if (errno == EINTR)
                continue;

            // an unexpected error occurred, consider it fatal
            break;
        }
    }
    return result;
}

int main(int argc, const char *argv[])
{
    CConfig conf = CConfig();

    // check arguments
    if (argc == 4)
    {
        conf.SetCallsign(CCallsign(argv[1]));

        // Check if argv[2] is an IPv4 address or interface name
        if (is_valid_ipv4(argv[2]))
        {
            // It's an IPv4 address, use it directly
            conf.SetListenIp(CIp(argv[2]));
            conf.SetTranscoderListenIp(CIp(argv[2]));
        }
        else
        {
            // It might be an interface name, try to resolve its IPv4 address
            std::string interface_ip = get_interface_ipv4(argv[2]);
            if (!interface_ip.empty())
            {
                std::cout << "Resolved interface " << argv[2] << " to IP address " << interface_ip << std::endl;
                conf.SetListenIp(CIp(interface_ip.c_str()));
                conf.SetTranscoderListenIp(CIp(interface_ip.c_str()));
            }
            else
            {
                std::cerr << "Error: '" << argv[2] << "' is neither a valid IPv4 address nor a valid network interface with an IPv4 address" << std::endl;
                return 1;
            }
        }

        conf.SetTranscoderIp(CIp(argv[3]));
    }
    else if (argc != 1)
    {
        std::cout << "Usage: xlxd callsign xlxdip/interface ambedip" << std::endl;
        std::cout << "example: xlxd XLX999 192.168.178.212 127.0.0.1" << std::endl;
        std::cout << "     or: xlxd XLX999 eth0 127.0.0.1" << std::endl;
        std::cout << "     or: xlxd XLX999 wlan0 127.0.0.1" << std::endl;
        std::cout << std::endl;
        std::cout << "Note: If the second parameter is not a valid IPv4 address," << std::endl;
        std::cout << "      xlxd will attempt to resolve it as a network interface name." << std::endl;

        std::cout << "Startup parameters can also be defined in " << CONFIG_PATH << std::endl;

        return 1;
    }

#ifdef RUN_AS_DAEMON

    // redirect cout, cerr and clog to syslog
    syslog::redirect cout_redir(std::cout);
    syslog::redirect cerr_redir(std::cerr);
    syslog::redirect clog_redir(std::clog);

    // Fork the Parent Process
    pid_t pid, sid;
    pid = ::fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // We got a good pid, Close the Parent Process
    if (pid > 0)
    {
        sd_notifyf(0, "MAINPID=%lu", (unsigned long)pid);
        exit(EXIT_SUCCESS);
    }

    // Change File Mask
    ::umask(0);

    // Create a new Signature Id for our child
    sid = ::setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Change Directory
    // If we cant find the directory we exit with failure.
    if ((::chdir("/")) < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Close Standard File Descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

#endif

    // splash
    std::cout << "Starting xlxd " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION << std::endl
              << std::endl;

    conf.DumpConfig();

    // initialize reflector
    g_Reflector.SetCallsign(conf.GetCallsign());
    g_Reflector.SetListenIp(conf.GetListenIp());
    g_Reflector.SetTranscoderIp(conf.GetTranscoderIp());
    g_Reflector.SetTranscoderListenIp(conf.GetTranscoderListenIp());
    g_Reflector.SetDefaultModuleYSF(conf.GetDefaultModuleYSF());

    // Block all signals while starting up the reflector -- we don't
    // want any of the worker threads handling them.
    sigset_t sigblockall, sigorig;
    sigfillset(&sigblockall);
    pthread_sigmask(SIG_SETMASK, &sigblockall, &sigorig);

    // and let it run
    if (!g_Reflector.Start())
    {
        std::cout << "Error starting reflector" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Restore main thread default signal state
    pthread_sigmask(SIG_SETMASK, &sigorig, nullptr);

    std::cout << "Reflector " << g_Reflector.GetCallsign()
              << "started and listening on " << g_Reflector.GetListenIp() << std::endl;

    // and wait for end
    wait_for_termination();

    g_Reflector.Stop();
    std::cout << "Reflector stopped" << std::endl;

    // done
    exit(EXIT_SUCCESS);
}
