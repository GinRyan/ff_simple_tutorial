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
/**
 * @brief open_decoder
 * @return
 */
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
/**
 * @brief write_out_yuv_frame
 * @param frame
 */
static void write_out_yuv_frame(const AVFrame* frame){
    uint8_t **pBuf = frame->data;
    int * pStride= frame->linesize;
    for (int color_idx=0;color_idx < 3; color_idx++) {
        int nWidth = color_idx ==0?frame->width:frame->width / 2;
        int nHeight = color_idx ==0? frame->height:frame->height / 2;
        for (int idx=0;idx < nHeight; idx++) {//write by line
            fwrite(pBuf[color_idx],1,nWidth,pFout);
            pBuf[color_idx] += pStride[color_idx];
        }
        fflush(pFout);
    }
}

static void closeAll(){
    fclose(pFin);
    fclose(pFout);
    avcodec_close(pContext);
    av_free(pContext);
    av_frame_free(&frame);

}

int main(int argc, char *argv[]){
    int buffS = INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE;
    uint8_t inbuf[buffS];

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
    int uDataSize = 0;
    int len = 0;
    int got_frame = 0;
    uint8_t* pDataPtr = NULL;
    int buffsize=INBUF_SIZE;

    while (1) {

        uDataSize = fread(inbuf,1,buffsize,pFin);
        printf("ok: fread ok\n");
        if (uDataSize == 0) {
            break;
        }
        pDataPtr = inbuf;
        //parse to pkg
        while (uDataSize > 0) {
            len = av_parser_parse2(pCodecParserCtx,pContext,
                                   &pkt.data,&pkt.size,
                                   pDataPtr,uDataSize,
                                   AV_NOPTS_VALUE,AV_NOPTS_VALUE,AV_NOPTS_VALUE
                                   );
            pDataPtr += len;
            uDataSize -= len;
            if (pkt.size == 0) {
                continue;
            }

            //parse ok
            printf("parse 1 packet\n");

            int ret = avcodec_decode_video2(pContext,frame, &got_frame,&pkt);
            if (ret < 0) {
                printf("Error: decode packet error");
                return -1;
            }

            if (got_frame) {
                //Write
                printf("Decode 1 frame OK~: width:%d  height:%d pts: %ld\n",frame->width,frame->height,frame->pts);
                write_out_yuv_frame(frame);
            }else {
                break;
            }

        }
    }

    pkt.data = NULL;
    pkt.size = 0;
    while (1) {
        int ret = avcodec_decode_video2(pContext,frame, &got_frame,&pkt);
        if (ret < 0) {
            printf("Error: decode packet error");
            return -1;
        }
        if (got_frame) {
            //Write
            printf("Flush: decode 1 frame OK~: width:%d  height:%d  pts: %ld\n",frame->width,frame->height,frame->pts);
            write_out_yuv_frame(frame);

        }else {
            break;
        }
    }

    closeAll();
    return EXIT_SUCCESS;
}
