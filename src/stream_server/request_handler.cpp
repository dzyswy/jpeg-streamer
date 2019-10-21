//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"


#define BOUNDARYSTRING "--BOUNDARYSTRING\r\n"

using namespace std;

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root, jpeg_streams& streams)
  : doc_root_(doc_root),
    streams_(streams)
{
}


void request_handler::handle_request(const request& req, reply& rep, int &channel)
{
	// Decode url to path.
	std::string request_path;
	if (!url_decode(req.uri, request_path))
	{
		rep = reply::stock_reply(reply::bad_request);
		return;
	}

	string schannel = request_path.substr(1, request_path.length() - 1);
	channel = atoi(schannel.c_str());
	if (channel >= streams_.size()) 
	{
		rep = reply::stock_reply(reply::not_found);
		return;  
	}  

	rep.status = reply::ok;
	rep.headers.clear();
	rep.content.clear();
	rep.headers.push_back(header("Connection", "close"));
	rep.headers.push_back(header("Max-Age", "0"));
	rep.headers.push_back(header("Expires", "0"));
	rep.headers.push_back(header("Cache-Control", "no-cache"));
	rep.headers.push_back(header("Pragma", "no-cache"));
	rep.headers.push_back(header("Content-Type", "multipart/x-mixed-replace; boundary=" BOUNDARYSTRING));

}

void request_handler::handle_boundary(reply& rep)
{
	rep.status = reply::ok;
	rep.headers.clear();
	rep.content.clear();
	rep.content = BOUNDARYSTRING;
}

int request_handler::handle_stream(int channel, reply& rep)
{
	streams_->watch_inc(channel);
	int ret = streams_->query_frame(channel, 10);
	if (ret < 0)
		return -1;
	
	
	rep.status = reply::ok;
	rep.headers.clear();
	rep.content.clear(); 
	
	streams_->get_image(channel, rep.content);
	std::string frame_count_s = "0";
	std::map<std::string, std::string> stream_headers; 
	streams_->get_frame_count(channel, frame_count_s);
	streams_->get_headers(channel, stream_headers);

	rep.headers.push_back(header("Content-type", "image/jpeg"));
	rep.headers.push_back(header("Content-Length", std::to_string(rep.content.size())));
	rep.headers.push_back(header("Content-frame-count", frame_count_s));
	
	for (auto it = stream_headers.begin(); it != stream_headers.end(); ++it) 
	{
		rep.headers.push_back(header(it->first, it->second));
	}	
	
	streams_->watch_dec(channel);
	return 0;
}


bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // namespace server
} // namespace http
