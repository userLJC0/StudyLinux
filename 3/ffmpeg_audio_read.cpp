#include"ffmpeg_audio_read.h"

std::string fileName = "/home/yaozai/MountDeviceData/Downloads/MV.mp4";
std::string outFileName = "/home/yaozai/MountDeviceData/Downloads/20221117.mp3";
    // ffmpeg相关变量预先定义与分配
    AVFormatContext *pAVFormatContext = 0;          // ffmpeg的全局上下文，所有ffmpeg操作都需要
    AVCodecContext *pAVCodecContext = 0;            // ffmpeg编码上下文
    AVCodec *pAVCodec = 0;                          // ffmpeg编码器
    AVPacket *pAVPacket = 0;                        // ffmpag单帧数据包
    AVFrame *pAVFrame = 0;                          // ffmpeg单帧缓存
     std::ofstream ifs;
    int audioIndex = -1;                            // 音频流所在的序号
    int numBytes = 0;



// std::thread _t;

void ffread_start_capture(std::string &sample_buf)
{
   
    std::stringstream sstream;
    
    auto sample_size = sample_buf.size();
    auto buf = sample_buf.data();
    memset(buf, 0, sample_size*2);

    int ret = 0;
    ifs.open(outFileName, ios::out);
    // 步骤七：读取一帧数据的数据包
    while(av_read_frame(pAVFormatContext, pAVPacket) >= 0)
    {
        if(pAVPacket->stream_index == audioIndex)
        {
            // 步骤八：将封装包发往解码器
            ret = avcodec_send_packet(pAVCodecContext, pAVPacket);
            if(ret)
            {
                std::cout << "Failed to avcodec_send_packet(pAVCodecContext, pAVPacket) ,ret =" << ret << std::endl;
                break;
            }
            // 步骤九：从解码器循环拿取数据帧
            while(!avcodec_receive_frame(pAVCodecContext, pAVFrame))
            {
//                for(int index = 0; index < pAVFrame->linesize[0]; index++)
//                {
                    // 入坑一；字节交错错误，单条音轨是好的，双轨存入文件，使用pcm的软件播放，则默认是LRLRLRLR的方式（采样点交错)
//                    file.write((const char *)(pAVFrame->data[0] + index), 1);
//                    file.write((const char *)(pAVFrame->data[1] + index), 1);
//                }
                // 入坑一；字节交错错误，单条音轨是好的，双轨存入文件，使用pcm的软件播放，则默认是LRLRLRLR的方式（采样点交错)
//                file.write((const char *)(pAVFrame->data[0], pAVFrame->linesize[0]);
//                file.write((const char *)(pAVFrame->data[1], pAVFrame->linesize[0]);
                // 输出为2, S16P格式是2字节
                numBytes = av_get_bytes_per_sample(pAVCodecContext->sample_fmt);
//                cout << "numBytes =" << numBytes;
                /*
                    P表示Planar（平面），其数据格式排列方式为 (特别记住，该处是以点nb_samples采样点来交错，不是以字节交错）:
                    LLLLLLRRRRRRLLLLLLRRRRRRLLLLLLRRRRRRL...（每个LLLLLLRRRRRR为一个音频帧）
                    而不带P的数据格式（即交错排列）排列方式为：
                    LRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRL...（每个LR为一个音频样本）
                */
                // 使用命令行提取pcm ffmpeg.exe -i 1.mp3 -f s16le -ar 44100 -ac 2 -acodec pcm_s16le D:/2.pcm
               string data;
                for (int index = 0; index < pAVFrame->nb_samples; index++)
                {
                    for (int channel = 0; channel < pAVCodecContext->channels; channel++)  // 交错的方式写入, 大部分float的格式输出
                    {
                        // file.write((char *)pAVFrame->data[channel] + numBytes * index, numBytes);
                        memcpy(data.data(), pAVFrame->data[channel] + numBytes * index, numBytes);
                        ifs << data.data();

                        sstream << data.data();
                    }
                }
                av_free_packet(pAVPacket);
            }
        }
    }
    // file.close();
    ifs.close();

    sstream >> sample_buf;

    release_and_recycle_resources();
}

int init_ffmpeg_audio_read_from_local(std::string &sample_buf)
{//解码视频
    
    int ret = 0;  

    pAVFormatContext = avformat_alloc_context();    // 分配
    pAVPacket = av_packet_alloc();                  // 分配
    pAVFrame = av_frame_alloc();                    // 分配

    if(!pAVFormatContext || !pAVPacket || !pAVFrame)
    {
        std::cout << "Failed to alloc"<<std::endl;
        release_and_recycle_resources();
        return 0;
    }
    // 步骤一：注册所有容器和编解码器（也可以只注册一类，如注册容器、注册编码器等）
    av_register_all();

    // 步骤二：打开文件(ffmpeg成功则返回0)
    // std::cout << "文件:" << fileName << "，是否存在：" << QFile::exists(fileName);
//    ret = avformat_open_input(&pAVFormatContext, fileName.toUtf8().data(), pAVInputFormat, 0);
    ret = avformat_open_input(&pAVFormatContext, fileName.data(), 0, 0);
    if(ret)
    {
        std::cout << "Failed"<<std::endl;
        release_and_recycle_resources();
        return 0;
    }

    // 步骤三：探测流媒体信息
    ret = avformat_find_stream_info(pAVFormatContext, 0);
    if(ret < 0)
    {
        std::cout << "Failed to avformat_find_stream_info(pAVCodecContext, 0)"<<std::endl;
        release_and_recycle_resources();
        return 0;
    }
    std::cout << "视频文件包含流信息的数量:" << pAVFormatContext->nb_streams;

    // 步骤四：提取流信息,提取视频信息
    for(int index = 0; index < pAVFormatContext->nb_streams; index++)
    {
        pAVCodecContext = pAVFormatContext->streams[index]->codec;
        switch (pAVCodecContext->codec_type)
        {
        case AVMEDIA_TYPE_UNKNOWN:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_UNKNOWN"<<std::endl;
            break;
        case AVMEDIA_TYPE_VIDEO:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_VIDEO"<<std::endl;
            break;
        case AVMEDIA_TYPE_AUDIO:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_AUDIO"<<std::endl;
            audioIndex = index;
            break;
        case AVMEDIA_TYPE_DATA:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_DATA"<<std::endl;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_SUBTITLE"<<std::endl;
            break;
        case AVMEDIA_TYPE_ATTACHMENT:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_ATTACHMENT"<<std::endl;
            break;
        case AVMEDIA_TYPE_NB:
            std::cout << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_NB"<<std::endl;
            break;
        default:
            break;
        }
        // 已经找打视频品流
        if(audioIndex != -1)
        {
            break;
        }
    }

    if(audioIndex == -1 || !pAVCodecContext)
    {
        std::cout << "Failed to find video stream"<<std::endl;
        release_and_recycle_resources();
        return 0;
    }
    // 步骤五：对找到的音频流寻解码器
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if(!pAVCodec)
    {
        std::cout << "Fialed to avcodec_find_decoder(pAVCodecContext->codec_id):"
            << pAVCodecContext->codec_id << std::endl ;
        release_and_recycle_resources();
        return 0;
    }
#if 0
    pAVCodecContext = avcodec_alloc_context3(pAVCodec);
    // 填充CodecContext信息
    if (avcodec_parameters_to_context(pAVCodecContext,
                                      pAVFormatContext->streams[audioIndex]->codecpar) < 0)
    {
        printf("Failed to copy codec parameters to decoder context!\n");
        release_and_recycle_resources();
        return 0;
    }
#endif

    // 步骤六：打开解码器
    ret = avcodec_open2(pAVCodecContext, pAVCodec, NULL);
    if(ret)
    {
        std::cout << "Failed to avcodec_open2(pAVCodecContext, pAVCodec, pAVDictionary)"<<std::endl;
        release_and_recycle_resources();
        return 0;
    }

    // 打印
    std::cout << "解码器名称：" <<pAVCodec->name<<std::endl
        << "通道数：" << pAVCodecContext->channels<<std::endl
        << "采样率：" << pAVCodecContext->sample_rate<<std::endl
        << "采样格式：" << pAVCodecContext->sample_fmt<<std::endl;

    // file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    
    ffread_start_capture(sample_buf);
    
    
}


void release_and_recycle_resources()
{
    std::cout << "释放回收资源"<<std::endl;
    if(pAVFrame)
    {
        av_frame_free(&pAVFrame);
        pAVFrame = 0;
        std::cout << "av_frame_free(pAVFrame)"<<std::endl;
    }
    if(pAVPacket)
    {
        av_free_packet(pAVPacket);
        pAVPacket = 0;
        std::cout << "av_free_packet(pAVPacket)"<<std::endl;
    }
    if(pAVCodecContext)
    {
        avcodec_close(pAVCodecContext);
        pAVCodecContext = 0;
        std::cout << "avcodec_close(pAVCodecContext);"<<std::endl;
    }
    if(pAVFormatContext)
    {
        avformat_close_input(&pAVFormatContext);
        avformat_free_context(pAVFormatContext);
        pAVFormatContext = 0;
        std::cout << "avformat_free_context(pAVFormatContext)"<<std::endl;
    }
}

// int main()
// {
//     // init_ffmpeg_audio_read_from_local();


//     return 0;
// }