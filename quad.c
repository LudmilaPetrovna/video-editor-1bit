#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define IN_WIDTH 128
#define IN_HEIGHT 64
#define TILE_X 4
#define TILE_Y 4
#define IN_FRAMES (TILE_X*TILE_Y)
#define OUT_WIDTH (IN_WIDTH*TILE_X)
#define OUT_HEIGHT (IN_HEIGHT*TILE_Y)

int main(int argc, char **argv){
uint8_t *in=malloc(OUT_WIDTH*OUT_HEIGHT);
uint8_t *out=malloc(OUT_WIDTH*OUT_HEIGHT);
int in_size,in_frames,f,w;

while(1){
in_size=fread(in,1,OUT_WIDTH*OUT_HEIGHT,stdin);
in_frames=in_size/IN_WIDTH/IN_HEIGHT;
memset(out,0,OUT_WIDTH*OUT_HEIGHT);

for(f=0;f<in_frames;f++){
for(w=0;w<IN_HEIGHT;w++){
int in_offset=(IN_HEIGHT*f+w)*IN_WIDTH;
int out_offset=(w+(f/TILE_X)*IN_HEIGHT)*OUT_WIDTH+(f%TILE_X)*IN_WIDTH;
//fprintf(stderr,"f:%d,w:%d,o:%d,oo:%d\n",f,w,in_offset,out_offset);
void *s=in+in_offset;
void *d=out+out_offset;
memcpy(d,s,IN_WIDTH);
}
}
fwrite(out,1,OUT_WIDTH*OUT_HEIGHT,stdout);
if(in_frames!=IN_FRAMES){break;}
}

return 0;
}
