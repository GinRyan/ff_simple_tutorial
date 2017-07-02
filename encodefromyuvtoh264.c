#include <stdio.h>
#include <stdlib.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

#include "errflags.h"

static int read_yuv_data(int color);
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
AVPacket pkt;
/**
 * ./encoderYUVtoH264  out.yuv out1.h264 640 416 800000 1600
 *
 * @brief parse_input_parameters
 * @param argc
 * @param argv
 * @return
 */
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
    int got_packet = 0;
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
    printf("Init all codec\n");
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    printf("Find codec\n");
    if (!codec) {
        printf("Find codec error\n");
        return FF_ERROR_INITIAL_FAILED;
    }

    printf("Setting codec context parameters\n");
    //allocate avcodec context instance
    codecCtx =  avcodec_alloc_context3(codec);
    if (!codecCtx) {
        printf("AV codec context alloc Error\n");
        return  FF_ERROR_INITIAL_FAILED;
    }
    codecCtx->width = frameWidth;
    codecCtx->height = frameHeight;
    codecCtx->bit_rate = bitrate;

    codecCtx->time_base = av_make_q(1,25);
    codecCtx->gop_size = 3;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(codecCtx->priv_data,"present","slow",0);

    //ok, parameters settings' ok.
    //open encoder.
    int openCodec = avcodec_open2(codecCtx,codec,NULL);
    if (openCodec<0) {
        printf("Open codec Error\n");
        return FF_ERROR_INITIAL_FAILED;
    }
    //allocate AVFrame and pixel space.
    frame = av_frame_alloc();
    if (!frame) {
        printf("AVFrame alloc error\n");
        return FF_ERROR_INITIAL_FAILED;
    }
    frame->width=codecCtx->width;
    frame->height=codecCtx->height;
    frame->format=codecCtx->pix_fmt;
    int localAv_image_alloc = av_image_alloc(frame->data,frame->linesize,frame->width,frame->height,frame->format,32);
    if (!localAv_image_alloc) {
        printf("AVImage alloc error\n");
        return FF_ERROR_INITIAL_FAILED;
    }

    printf("start encode loop\n");
    //start encode loop
    for (int frameIdx = 0;frameIdx < frameToEncode;frameIdx++) {
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;

        //read data to AVFrame
        read_yuv_data(0);
        read_yuv_data(1);
        read_yuv_data(2);
        frame->pts = frameIdx;

        int localAvcodec_send_frame = avcodec_send_frame(codecCtx,frame);
        if (localAvcodec_send_frame<0) {
            printf("Error: Sending failed!\n");
            return  FF_ERROR_ENCODING_FAILED;
        }

        int localAvcodec_receive_packet = avcodec_receive_packet(codecCtx,&pkt);
        if(localAvcodec_receive_packet>=0){
            printf("write packet to frame:%d, size=%d \n",frameIdx,pkt.size);
            fwrite(pkt.data,1,(unsigned long)pkt.size,pFout);
            av_packet_unref(&pkt);
        }

    }
    //if there's undone frames in cache, do it.
    while (1) {
        int hasReceivedPacket = avcodec_receive_packet(codecCtx,&pkt);
        printf("write cached frame size=%d \n",pkt.size);
        fwrite(pkt.data,1,(unsigned long)pkt.size,pFout);
        av_packet_unref(&pkt);

        if (hasReceivedPacket < 0) {
            break;
        }
    }

    //close file
    fclose(pFin);
    fclose(pFout);
    avcodec_close(codecCtx);
    av_free(codecCtx);
    av_freep(frame->data);
    av_frame_free(&frame);
    printf("Bye!\n");
    return EXIT_SUCCESS;
}

static int read_yuv_data(int color){
    //color = 0 is Y
    //color = 1 is U
    //color = 2 is V
    int color_height = color == 0 ? frameHeight: frameHeight / 2;
    int color_width = color == 0 ? frameWidth:frameWidth / 2;
    int color_size = color_height * color_width;
    int color_stride = frame->linesize[color];
    if (color_width == color_stride) {
        fread(frame->data[color],1,(unsigned long)color_size,pFin);
    } else {
        for (int rowidx = 0;rowidx < color_height;rowidx++) {
            fread(frame->data[color] + rowidx * color_stride,1,(unsigned long)color_width,pFin);
        }
    }
    return color_size;
}























