#ifndef __STREAM_SERVER_H
#define __STREAM_SERVER_H

#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <typeinfo>
#include <mutex>
#include <thread>
#include <condition_variable>




class stream_server_impl;
class jpeg_streams;


class stream_server
{
public:
	stream_server(int port, int stream_count);
	~stream_server();
	
	void run();
	void stop();
	
	int add_header(int channel, const std::string key);
	
	int check_watch(int channel);
	
	void post_frame(int channel, int frame_count = 0);
	void set_image(int channel, std::vector<unsigned char> &image);
	int set_header(int channel, const std::string key, std::string &value);
	
	
protected:
	stream_server_impl *impl_;
	jpeg_streams *streams_;
	std::thread *run_thread_;
	
};






#endif







