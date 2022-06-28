#include <string>
#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>

#include "client.h"


Client::Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator, 
    std::string const& t_path)
    : m_ioService(t_ioService), m_socket(t_ioService), 
    m_endpointIterator(t_endpointIterator), m_path(t_path)
{
    doConnect();
    openFile(m_path);
}


void Client::openFile(std::string const& t_path)
{
    m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
    if (m_sourceFile.fail())
        throw std::fstream::failure("Failed while opening file " + t_path);
    
    m_sourceFile.seekg(0, std::ifstream::end);
    m_filesize = m_sourceFile.tellg();
    m_sourceFile.seekg(0, std::ifstream::beg);

    std::ostream requestStream(&m_request);
    boost::filesystem::path p(t_path);
    requestStream << p.filename().string() << "\n" << m_filesize << "\n\n";
    BOOST_LOG_TRIVIAL(trace) << "Request size: " << m_request.size();
}


void Client::doConnect()
{
    boost::asio::async_connect(m_socket, m_endpointIterator, 
        [this](boost::system::error_code ec, const TcpResolverIterator&)
        {
            if (!ec) {
                std::cout << "Send...\n";
                writeBuffer(m_request);
            } else {
                std::cout << "Coudn't connect to host. Please run server "
                    "or check network connection.\n";
                BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
            }
        });
}


void Client::doWriteFile(const boost::system::error_code& t_ec)
{
    if (!t_ec) {
        if (m_sourceFile) {
            m_sourceFile.read(m_buf.data(), m_buf.size()); 
            if (m_sourceFile.fail() && !m_sourceFile.eof()) {
                auto msg = "Failed while reading file";
                BOOST_LOG_TRIVIAL(error) << msg;
                throw std::fstream::failure(msg);
            }
            std::stringstream ss;
            m_sumSentBytes += static_cast<std::size_t>(m_sourceFile.gcount());
            ss << "Send " << m_sourceFile.gcount() << " bytes, total: "
               << m_sumSentBytes << " bytes";
            BOOST_LOG_TRIVIAL(trace) << ss.str();

            auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
            writeBuffer(buf);
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error: " << t_ec.message();
    }
}

std::ostream & operator << (std::ostream & out, const Client & client)
{
    out << "Sent: " << client.m_sumSentBytes << "/" << client.m_filesize << " bytes" << std::endl;
    return out;
}
