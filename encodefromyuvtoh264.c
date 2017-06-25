#include <stdio.h>
#include <stdlib.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

#include "errflags.h"

/**
* AVCodec 结构保存了一个编解码器的实例。是编解码器。要使用一个指向他的指针。
* AVCodecContext 代表编解码的上下文，用于储存编解码所需要的参数。也是使用指针保存
* AVFrame 结构保存编码前的像素数据，并且作为编码器输入数据。也是指针。
* AVPacket 表示编码后的码流包结构。使用实例来保存。
*/

const char* inputFileName = NULL;

const char* outputFileName = NULL;
int frameWidth = 0;
int frameHeight = 0;
int bitrate = 0;

int frameToEncode = 0;
//input yuv file
FILE* pFin = NULL;
//output encoded file
FILE* pFout=NULL;
//encoder and decoder
AVCodec* codec = NULL;
//codec context with parameters;
AVCodecContext* codecCtx= NULL;
//video frame before encoding
AVFrame* frame = NULL;
//video and audio packet after encoding.
AVPacket* pkt;

static int parse_input_parameters(int argc,char **argv){
    for (int i = 0;i < argc;i++) {
        printf("argv[%d]: %s\n",i,argv[i]);
    }
    //argv[0] is exec self.s
    inputFileName = argv[1];
    outputFileName = argv[2];
    frameWidth = atoi(argv[3]);
    frameHeight = atoi(argv[4]);
    bitrate = atoi(argv[5]);
    frameToEncode = atoi(argv[6]);

    pFin = fopen(inputFileName,"rb+");
    printf("YUV FileIn:%s\n",inputFileName);
    printf("Encode FileOut:%s\n",outputFileName);
    if (!pFin) {
        return IO_FILE_ERROR_OPEN_FAILED;
    }

    pFout = fopen(outputFileName,"wb+");
    if (!pFout) {
        return IO_FILE_ERROR_OPEN_FAILED;
    }
    return 1;
}

/**
* @brief main
* @param argc
* @param argv
* @return
*/
int main(int argc,char **argv){
    if(parse_input_parameters(argc,argv) > 0){
        printf("Input File:\t%s\nOutputFile:\t%s\nFrame resolution:\t%d x %d \nbitrate: \t%d\nFrameNumToEncode:\t%d\n",
               inputFileName,outputFileName,
               frameWidth,frameHeight,
               bitrate,frameToEncode
               );

    }else{
        printf("Error: Command error\n");
        return -1;
    }
    //register all the encoders and decoders.
    avcodec_register_all();
    //find all needed encoder or decoder.
    //There we use h264 encoder.
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        return FF_ERROR_INITIAL_FAILED;
    }

    //allocate avcodec context instance
    codecCtx =  avcodec_alloc_context3(codec);
    if (!codecCtx) {
        return  FF_ERROR_INITIAL_FAILED;
    }
    codecCtx->width = frameWidth;
    codecCtx->height = frameHeight;
    codecCtx->bit_rate = bitrate;
    AVRational r=  {1,25};
    codecCtx->gop_size = 10;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set(codecCtx->priv_data,"present","slow",0);
    //ok, parameters settings' ok.
    //open encoder.
    int openCodec = avcodec_open2(codecCtx,codec,NULL);
    if (!openCodec) {
        return FF_ERROR_INITIAL_FAILED;
    }
    //allocate AVFrame and pixel space.
    frame = av_frame_alloc();
    if (!frame) {
        return FF_ERROR_INITIAL_FAILED;
    }
    frame->width=codecCtx->width;
    frame->height=codecCtx->height;
    frame->format=codecCtx->pix_fmt;
    int localAv_image_alloc = av_image_alloc(frame->data,frame->linesize,frame->width,frame->height,frame->format,32);
    if (!localAv_image_alloc) {
        return FF_ERROR_INITIAL_FAILED;
    }
    return EXIT_SUCCESS;
}

























