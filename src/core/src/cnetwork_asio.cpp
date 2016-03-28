/*
 * cnetwork_asio.cpp
 *
 *  Created on: Nov 23, 2015
 *      Author: ctorres
 */

#include <cstdlib>
#include <iostream>
#include <thread>
#include "cnetwork_asio.h"

namespace Core {

NetworkThreadPool* NetworkThreadPool::instance_ = nullptr;

CNetwork_Asio::CNetwork_Asio()
    : INetwork(),
      networkService_(&NetworkThreadPool::GetInstance()),
      socket_(*networkService_->Get_IO_Service()),
      listener_(*networkService_->Get_IO_Service()),
      packet_offset_(0),
      packet_size_(6),
      active_(true),
      last_update_time_( Core::Time::GetTickCount() ) {
    logger_ = CLog::GetLogger(log_type::NETWORK).lock();
}

CNetwork_Asio::~CNetwork_Asio() {
  Shutdown();

  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  } while (!discard_queue_.empty());

  while( send_queue_.empty() == false )
    send_queue_.pop();

//  logger_->debug() << "net shared_ptr used by " << logger_.use_count()-1;
  logger_.reset();
}

bool CNetwork_Asio::Init(std::string _ip, uint16_t _port) {
  if (_ip.length() < 2)  // We can actually use hostnames instead of IP
    // addresses. Ex. google.com
    return false;

  network_ip_address = _ip;
  network_port_ = _port;
  return true;
}

bool CNetwork_Asio::Shutdown() {

  if(OnShutdown() == true) {
    Disconnect();

    if (listener_.is_open()) {
      std::error_code ignored;
      listener_.close(ignored);
    }
    active_ = false;
  }
  return true;
}

bool CNetwork_Asio::Connect() {
  tcp::resolver resolver(*networkService_->Get_IO_Service());
  auto endpoint_iterator =
      resolver.resolve(network_ip_address, std::to_string(network_port_));

  OnConnect();
  asio::async_connect(
      socket_, endpoint_iterator,
      [this](std::error_code errorCode, const asio::ip::tcp::endpoint) {
        if (!errorCode) {
          OnConnected();
        }
      });
  active_ = true;
  return true;
}

bool CNetwork_Asio::Listen() {
  OnListen();
  tcp::endpoint endpoint(tcp::v4(), network_port_);
  listener_.open(endpoint.protocol());
  listener_.set_option(tcp::acceptor::reuse_address(true));
  listener_.non_blocking(true);
  listener_.bind(endpoint);
  listener_.listen();
  logger_->notice() << "Listening started on " << GetIpAddress() << ":" << GetPort();
  active_ = true;
  AcceptConnection();
  OnListening();
  return true;
}

bool CNetwork_Asio::Reconnect() {
  Disconnect();
  Connect();
  return true;
}

bool CNetwork_Asio::Disconnect() {
  OnDisconnect();
  //  networkService_->Get_IO_Service()->post([this]() {
  std::error_code ignored;
  socket_.shutdown(asio::socket_base::shutdown_both, ignored);
  OnDisconnected();
  //  });
  return true;
}

<<<<<<< e02ff60e42f0d538692c52e359cc88f8c1061d0a
void CNetwork_Asio::ProcessSend()
{
  if(true == active_)
  {
    discard_mutex_.lock();
    bool discard_empty = discard_queue_.empty();
    discard_mutex_.unlock();

    send_mutex_.lock();
    bool send_empty = send_queue_.empty();
    send_mutex_.unlock();

    if( send_empty != true && discard_empty == true )
    {
      send_mutex_.lock();
      std::unique_ptr<uint8_t> _buffer = std::move(send_queue_.front());
      send_queue_.pop();
      send_mutex_.unlock();

      uint8_t* raw_ptr = _buffer.get();
      uint16_t _size = (uint16_t)raw_ptr[0];
      uint16_t _command = (uint16_t)raw_ptr[2];
      discard_mutex_.lock();
      discard_queue_.push(std::move(_buffer));
      raw_ptr = discard_queue_.back().get();
      discard_mutex_.unlock();

      if (OnSend(raw_ptr))
        asio::async_write(socket_, asio::buffer(raw_ptr, _size),
                          [this](const asio::error_code& error,
                                 std::size_t bytes_transferred) {
          (void)bytes_transferred;
          if (!error) {
            OnSent();
          }

          discard_mutex_.lock();
          {
            std::unique_ptr<uint8_t> _buffer = std::move(discard_queue_.front());
            discard_queue_.pop();
            _buffer.reset(nullptr);
          }
          discard_mutex_.unlock();

          ProcessSend();

        });
      else
        logger_->debug("Not sending packet: [{0}, {1:x}] to client {2}", _size, _command, GetId());
    }
  }
}

bool CNetwork_Asio::Send(std::unique_ptr<uint8_t> _buffer) {
  //std::lock_guard<std::mutex> lock(send_mutex_);
  send_mutex_.lock();
  send_queue_.push(std::move(_buffer));
  send_mutex_.unlock();

  ProcessSend();

//  discard_mutex_.lock();
//  uint16_t discard_size = discard_queue_.size();
//  discard_mutex_.unlock();

//  if( discard_size == 0 )
//  {
//  send_mutex_.lock();
//  _buffer = send_queue_.front();
//  send_queue_.pop();
//  send_mutex.unlock();

//  uint8_t* raw_ptr = _buffer.get();
//  uint16_t _size = (uint16_t)raw_ptr[0];
//  uint16_t _command = (uint16_t)raw_ptr[2];

//  discard_mutex_.lock();
//  discard_queue_.push(std::move(_buffer));
//  raw_ptr = discard_queue_.back().get();
//  discard_mutex_.unlock();

//  if (OnSend(raw_ptr))
//    asio::async_write(socket_, asio::buffer(raw_ptr, _size),
//                      [this](const asio::error_code& error,
//                             std::size_t bytes_transferred) {
//      (void)bytes_transferred;
//      if (!error) {
//        OnSent();
//      }

//      discard_mutex_.lock();
//      {
//        std::unique_ptr<uint8_t> _buffer = std::move(discard_queue_.front());
//        discard_queue_.pop();
//        _buffer.reset(nullptr);
//      }
//      discard_mutex_.unlock();

//    });
//  else
//    logger_->debug("Not sending packet: [{0}, {1:x}] to client {2}", _size, _command, GetId());
//  }
  return true;
}

bool CNetwork_Asio::Recv(uint16_t _size /*= 6*/) {
  OnReceive();
  {
    (void)_size;

    std::error_code errorCode;
    int16_t BytesToRead = packet_size_ - packet_offset_;
    asio::async_read(socket_,
                     asio::buffer(&buffer_[packet_offset_], BytesToRead),
                     asio::transfer_exactly(
                         BytesToRead),  // We want at least 6 bytes of data
                     [this](std::error_code errorCode, std::size_t length) {
      packet_offset_ += length;
      if (!errorCode || errorCode.value() == 11) {
        if (OnReceived() == false) {
          logger_->debug() << "Something bad happened in OnReceived... Shutting down -> client " << GetId();
          Shutdown();
        }
      } else {
        if (errorCode.value() == 2) {
          logger_->notice() << "Client " << GetId() << " disconnected.";
          OnDisconnected();
          Shutdown();
        } else {
          logger_->debug() << GetId() << "-> Error " << errorCode.value() << " occurred: " << errorCode.message();

          Shutdown();
          return;
        }
      }
//      std::chrono::steady_clock::time_point update = Core::Time::GetTickCount();
//      int64_t dt = std::chrono::duration_cast<std::chrono::milliseconds>( update - last_update_time_ ).count();
//      if( dt > (1000 * 60) * 5 ) // wait 5 minutes before time out
//      {
//        logger_->notice() << "Client " << GetId() << " timed out. " << dt;
//        Shutdown();
//        return;
//      }
      last_update_time_ = Core::Time::GetTickCount();
      recv_condition_.notify_all();
      if (active_) Recv();
    });
  }
  return true;
}

void CNetwork_Asio::AcceptConnection() {
  listener_.async_accept([this](std::error_code ec, tcp::socket socket) {
    if (!ec) {
      if (this->OnAccept())  // This should be changed to use a client session
      // instead of a CNetwork_Asio class
      {
        socket.non_blocking(true);
        // Do something here for the new connection.
        // Make sure to use std::move(socket)
        // std::make_shared<CClientSesson>( std::move(socket) );
        this->OnAccepted(std::move(socket));
      } else {
        // Kill the socket
        std::error_code ignored;
        socket.close(ignored);
      }
    }
    if (active_) AcceptConnection();
  });
}

bool CNetwork_Asio::OnConnect() { return true; }

void CNetwork_Asio::OnConnected() {
  //  if (!listener_.is_open()) Recv();
}

bool CNetwork_Asio::OnListen() { return true; }

void CNetwork_Asio::OnListening() {
  //  if (!listener_.is_open()) Recv();
}

bool CNetwork_Asio::OnDisconnect() { return true; }

void CNetwork_Asio::OnDisconnected() {}

bool CNetwork_Asio::OnReceive() { return true; }

bool CNetwork_Asio::OnReceived() { return true; }

bool CNetwork_Asio::OnShutdown() { return true; }

bool CNetwork_Asio::OnSend(uint8_t* _buffer) {
  (void)_buffer;
  return true;
}

void CNetwork_Asio::OnSent() {}

bool CNetwork_Asio::OnAccept() { return true; }

void CNetwork_Asio::OnAccepted(tcp::socket _sock) { (void)_sock; }

bool CNetwork_Asio::HandlePacket(uint8_t* _buffer) {
  (void)_buffer;
  return true;
}
}
