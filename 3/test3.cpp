#include"ffmpeg_audio_read.h"
#include<iostream>
using namespace std;

int main()
{
    string sample_buf;
    init_ffmpeg_audio_read_from_local(sample_buf);

    return 0;
}