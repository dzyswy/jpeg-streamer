//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>
#include <iostream>
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

connection::connection(asio::ip::tcp::socket socket,
    connection_manager& manager, request_handler& handler)
  : socket_(std::move(socket)),
    connection_manager_(manager),
    request_handler_(handler),
	stream_id_(0)
{
	std::cout << "create new connection\n";
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  socket_.close();
}

void connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(asio::buffer(buffer_),
      [this, self](std::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          request_parser::result_type result;
          std::tie(result, std::ignore) = request_parser_.parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);

          if (result == request_parser::good)
          {
            request_handler_.handle_request(request_, reply_, stream_id_);
			if (reply_.status != reply::ok) {
			  do_write();
			} else {
				do_response();
			}
            
          }
          else if (result == request_parser::bad)
          {
            reply_ = reply::stock_reply(reply::bad_request);
            do_write();
          }
          else
          {
            do_read();
          }
        }
        else if (ec != asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

void connection::do_write()
{
  auto self(shared_from_this());
  asio::async_write(socket_, reply_.to_buffers(),
      [this, self](std::error_code ec, std::size_t)
      {
        if (!ec)
        {
          // Initiate graceful connection closure.
          asio::error_code ignored_ec;
          socket_.shutdown(asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
        }

        if (ec != asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

void connection::do_response()
{
  auto self(shared_from_this());
  asio::async_write(socket_, reply_.to_buffers(),
      [this, self](std::error_code ec, std::size_t)
      {
        if (!ec)
        {
          do_boundary();
        }

        if (ec && (ec != asio::error::operation_aborted))
        {
          connection_manager_.stop(shared_from_this());
        }
      });
} 

void connection::do_boundary()
{
//	std::cout << "do_boundary\n";
	
	request_handler_.handle_boundary(reply_);
	auto self(shared_from_this());
	asio::async_write(socket_, reply_.content_to_buffers(),
	  [this, self](std::error_code ec, std::size_t)
	  {
		if (!ec)
		{
		  do_stream();
		}
		
		if (ec && (ec != asio::error::operation_aborted))
        {
          connection_manager_.stop(shared_from_this());
        }

	  });
}

void connection::do_stream()
{
//	std::cout << "do_stream\n";
	
	int ret = request_handler_.handle_stream(stream_id_, reply_);
	if (ret < 0)
	{
	  std::cout << "no more image, close connect\n";
	  connection_manager_.stop(shared_from_this());	
	}	
	
	auto self(shared_from_this());
	asio::async_write(socket_, reply_.headers_content_to_buffers(),
	  [this, self](std::error_code ec, std::size_t)
	  {
		if (!ec)
		{
		  do_boundary();
		}
		
		if (ec && (ec != asio::error::operation_aborted))
        {
          connection_manager_.stop(shared_from_this());
        }
	  });
}

} // namespace server
} // namespace http
