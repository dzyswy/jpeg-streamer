#include "stream_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <asio.hpp>

#include "server.hpp"
#include "jpeg_streams.h"



class stream_server_impl
{
public:
	stream_server_impl(int port, jpeg_streams& streams)
	{
		std::string _ip = "0.0.0.0";
		std::string _port = std::to_string(port);
		std::string _root = ".";
		srv_ = new http::server::server(_ip, _port, _root, streams);
	}
	
	~stream_server_impl()
	{
		delete srv_;
	}
	
	void run()
	{
		srv_->run();
	}
	
	void stop()
	{
		srv_->stop();
	}
	
protected:
	http::server::server srv_;		
};


stream_server::stream_server(int port, int stream_count)
{
	streams_ = new jpeg_streams(stream_count);
	impl_ = new stream_server_impl(port, *streams_);
}

stream_server::~stream_server()
{
	delete impl_;
	delete streams_;
}



void stream_server::run()
{
	run_thread_ = new std::thread([this] () {impl_->run();printf("stream server done!\n");});
}


void stream_server::stop()
{
	impl_->stop();
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
	}
}

int stream_server::add_header(int channel, const std::string key)
{
	return streams_->add_header(channel, key);
}


int stream_server::check_watch(int channel)
{
	return streams_->check_watch(channel);
}


void stream_server::post_frame(int channel, int frame_count)
{
	return streams_->post_frame(channel, frame_count);
}

void stream_server::set_image(int channel, std::vector<unsigned char> &image)
{
	return streams_->set_image(channel, image);
}

int stream_server::set_header(int channel, const std::string key, std::string &value)
{
	return streams_->set_header(channel, key, value);
}









