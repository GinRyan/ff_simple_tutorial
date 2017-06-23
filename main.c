#include <stdio.h>
#include <string.h>
#include "SDL2/SDL_cpuinfo.h"
#include "libavcodec/avcodec.h"


int main()
{
    printf("FFMPEG!");
    int hasMMX = SDL_HasMMX();
    printf("CPU HasMMX: %i\n",hasMMX);
    avcodec_register_all();
    unsigned int localAvcodec_version = avcodec_version();
    printf("AVCODEC Version:%i\n",localAvcodec_version);
    //printf("Hello World!\n");
    return 0;
}
