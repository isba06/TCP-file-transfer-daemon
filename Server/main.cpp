#include <iostream>

#include <boost/asio/io_service.hpp>
#include <boost/filesystem.hpp>
#include "server.h"
#include "../Log/logger.h"
#include <boost/log/trivial.hpp>
#include <boost/bind.hpp>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: server <port> <workDirectory>\n";
        return 1;
    }

    Logger::instance().setOptions("server_%3N.log", 1 * 1024 * 1024, 10 * 1024 * 1024);

    try {
        boost::asio::io_service io_service;

        Server server(io_service, std::stoi(argv[1]), argv[2]);
        boost::asio::signal_set signals(io_service, SIGHUP, SIGTERM);
        signals.async_wait(
                boost::bind(&boost::asio::io_service::stop, &io_service));

        io_service.notify_fork(boost::asio::io_service::fork_prepare);
        if (pid_t pid = fork())
        {
            if (pid > 0)
            {
                exit(0);
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "First fork failed";
                return 1;
            }
        }
        setsid();
        umask(0);
        if (pid_t pid = fork())
        {
            if (pid > 0)
            {
                exit(0);
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Second fork failed";
                return 1;
            }
        }

        close(0);
        close(1);
        close(2);
        if (open("/dev/null", O_RDONLY) < 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Unable to open /dev/null";
            return 1;
        }
        const char* output = "/tmp/asio.daemon.out";
        const int flags = O_WRONLY | O_CREAT | O_APPEND;
        const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        if (open(output, flags, mode) < 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Unable to open output file %s: " << output;
            return 1;
        }

        if (dup(1) < 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Unable to dup output descriptor";
            return 1;
        }

        io_service.notify_fork(boost::asio::io_service::fork_child);
        BOOST_LOG_TRIVIAL(info) << "Daemon started";
        io_service.run();
        BOOST_LOG_TRIVIAL(info) << "Daemon stopped";
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
