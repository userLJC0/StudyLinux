#include<iostream>
#include<string>
#include<fstream>
 extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
//#include "thread_safe.h"
}
#include <sstream>
#include <thread>
#include <vector>
using namespace std;


// int init_ffmpeg_audio_read_from_local(std::vector<std::uint8_t> &sample_buf);
// void ffread_start_capture(std::vector<std::uint8_t> &sample_buf);
void release_and_recycle_resources();

int init_ffmpeg_audio_read_from_local(std::string &sample_buf);
void ffread_start_capture(std::string &sample_buf);