#include "videodecodingheader.h"

#define INBUF_SIZE 4096;

FILE* pFin = NULL;
FILE* pFout = NULL;

AVCodec* pCodec = NULL;
AVCodecContext* pContext = NULL;
AVCodecParserContext* pCodecParserCtx = NULL;

AVFrame* frame =NULL;
AVPacket pkt;
/**
 * ./DecodeToYUV out.h264 out2.yuv
 *
 * @brief open_input_output_file
 * @param argv
 * @return
 */
static int open_input_output_file(char **argv){
    int buffS = INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE;
    int8_t inbuf[buffS];
    //buffer size with padding
    const char* inputFileName = argv[1];
    const char* outputFilename= argv[2];
    pFin = fopen(inputFileName,"rb+");
    if (!pFin) {
        printf("Error:open input file failed\n");
        return -1;
    }
    pFout = fopen(outputFilename,"wb+");
    if (!pFout) {
        printf("Error:open output file failed\n");
        return -1;
    }
}

static int open_decoder(){
    avcodec_register_all();
    av_init_packet(&pkt);//init packet
    pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!pCodec) {
        printf("Find codec error\n");
        return -1;
    }
    pContext = avcodec_alloc_context3(pCodec);
    if (!pContext) {
        printf("Error: avcodec_alloc_context3 failed\n");
        return -1;
    }

    if (pCodec->capabilities & AV_CODEC_CAP_TRUNCATED) {
        pContext->flags |= AV_CODEC_CAP_TRUNCATED;//
    }

    pCodecParserCtx = av_parser_init(AV_CODEC_ID_H264);
    if (!pCodecParserCtx) {
        printf("Error: av_parser_init failed\n");
        return -1;
    }

    if (avcodec_open2(pContext,pCodec,NULL) < 0) {
        printf("Error: avcodec_open2 failed\n");
        return -1;
    }

    frame = av_frame_alloc();
    if (!frame) {
        printf("Error: av_frame_alloc failed\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{

    if (open_input_output_file(argv)<0) {
        return -1;
    }else  {
        printf("ok: open_input_output_file ok\n");
    }

    if (open_decoder() < 0) {
        return -1;
    }else  {
        printf("ok: open_decoder ok\n");
    }

    return EXIT_SUCCESS;
}
