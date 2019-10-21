#ifndef __JPEG_STREAMS_H
#define __JPEG_STREAMS_H




#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <typeinfo>
#include <mutex>
#include <thread>
#include <condition_variable>


 




class jpeg_stream
{
public:
	jpeg_stream();
	~jpeg_stream();
	void add_header(const std::string key);
	
	int check_watch();
	void watch_inc();
	void watch_dec(); 
	

	void post_frame(int frame_count = 0);
	void set_image(std::vector<unsigned char> &image);
	int set_header(const std::string key, std::string &value);
	
	int query_frame(int timeout = 5);
	void get_image(std::vector<unsigned char> &image);
	void get_image(std::string &image);
	void get_frame_count(std::string &value);
	int get_header(const std::string key, std::string &value);
	
	std::map<std::string, std::string> get_headers(); 
	
protected:
	int watch_;
	int frame_count_;
	std::vector<unsigned char> image_;
	std::map<std::string, std::string> headers_;


	std::mutex mux_;
	std::condition_variable cond_;	
};



class jpeg_streams
{
public:
	jpeg_streams(int count);
	~jpeg_streams();
	
	int add_header(int channel, const std::string key);
	
	int size()
	{
		return streams_.size();
	}
	
	int check_watch(int channel);
	int watch_inc(int channel);
	int watch_dec(int channel); 
	
	
	void post_frame(int channel, int frame_count = 0);
	void set_image(int channel, std::vector<unsigned char> &image);
	int set_header(int channel, const std::string key, std::string &value);
	
	int query_frame(int channel, int timeout = 5);
	void get_image(int channel, std::vector<unsigned char> &image);
	void get_image(int channel, std::string &image);
	void get_frame_count(int channel, std::string &value);
	int get_header(int channel, const std::string key, std::string &value);
	
	int get_headers(int channel, std::map<std::string, std::string> &value);
	
protected:
	std::vector<jpeg_stream> streams_;
	
};




#endif

