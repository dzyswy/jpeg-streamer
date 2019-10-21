#include "jpeg_streams.h"





using keyspace std;




jpeg_stream::jpeg_stream()
{
	frame_count_ = 0;
	image_.push_back('a');
}

jpeg_stream::~jpeg_stream()
{
	image_.clear();
	headers_.clear();
}

void jpeg_stream::add_header(const std::string key)
{
	string value = "";
	headers_.insert(make_pair(key, value));
}



void jpeg_stream::post_frame(int frame_count)
{
	std::unique_lock<std::mutex> lock(mux_);
	frame_count_ = frame_count;
	cond_.notify_all();	
}

void jpeg_stream::set_image(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image_ = image;
}

int jpeg_stream::set_header(const std::string key, std::string &value)
{
	typename std::map<std::string, std::string>::iterator it;
	it = headers_.find(key);
	if (it == headers_.end())
		return -1;
	it->second = value;
	return 0;
}


int jpeg_stream::query_frame(int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
	
	return 0;
}

void jpeg_stream::get_image(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image = image_;
}

void jpeg_stream::get_image(std::string &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image.clear();
	image.append(image_.begin(), image_.end());
}

void jpeg_stream::get_frame_count(std::string &value)
{
	std::unique_lock<std::mutex> lock(mux_);
	value = std::string(frame_count_);
}

int jpeg_stream::get_header(const std::string key, std::string &value)
{
	std::unique_lock<std::mutex> lock(mux_);
	typename std::map<std::string, std::string>::iterator it;
	it = headers_.find(key);
	if (it == headers_.end())
		return -1;
	value = it->second;
	return 0;
}













jpeg_streams::jpeg_streams(int count)
{
	streams_.resize(count);
}

jpeg_streams::~jpeg_streams()
{
	streams_.clear();
}


int jpeg_streams::add_header(int channel, const std::string key)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].add_header(key);
	return 0;
}


void jpeg_streams::post_frame(int channel, int frame_count)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].post_frame(frame_count);
	return 0;
}

void jpeg_streams::set_image(int channel, std::vector<unsigned char> &image)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].set_image(image);
	return 0;
}

int jpeg_streams::set_header(int channel, const std::string key, std::string &value)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].set_header(key, value);
	return 0;
}


int jpeg_streams::query_frame(int channel, int timeout)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].query_frame(timeout);
	return 0;
}

void jpeg_streams::get_image(int channel, std::vector<unsigned char> &image)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].get_image(image);
	return 0;
}

void jpeg_streams::get_image(int channel, std::string &image)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].get_image(image);
	return 0;
}

void jpeg_streams::get_frame_count(int channel, std::string &value)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].get_frame_count(value);
	return 0;
}

int jpeg_streams::get_header(int channel, const std::string key, std::string &value)
{
	if (channel >= streams_.size())
		return -1;
	
	streams_[channel].get_header(key, value);
	return 0;
}

































