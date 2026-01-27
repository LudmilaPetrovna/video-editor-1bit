#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define IN_WIDTH 128
#define IN_HEIGHT 64

#define SAMPLERATE 8000
#define CHANNELS 1
#define SOUND_FILE_SIZE 50000000


void putpixel(uint8_t *in, int ox, int oy, int color) {
if(ox<0 || oy<0 || ox>=IN_WIDTH || oy>=IN_HEIGHT){return;}
in[ox+oy*IN_WIDTH]=color;
}

void draw_line(uint8_t *in, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1) {
        putpixel(in, x0, y0, ((x0^y0)&1)*255);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}


void draw_sound(uint8_t *in, int16_t *sound, double fps, double current_time, double pre_time, double post_time){
double duration=1.0/fps+pre_time+post_time;
double scale=duration/(double)IN_WIDTH;

int p=(int)((double)(current_time-pre_time)*SAMPLERATE+.5)*CHANNELS*2;
int ox=0;
int oy=sound[p]/1000+32;
int q;

// draw borders
for(q=0;q<IN_HEIGHT;q+=3){
oy=q;
ox=IN_WIDTH/2-pre_time/scale;
//fprintf(stderr,"border left: %d, scale:%f, dur:%f, pre:%f, post:%f\n",ox,scale,duration,pre_time,post_time);
if(ox<0 || oy<0 || ox>=IN_WIDTH || oy>=IN_HEIGHT){continue;}
in[ox+oy*IN_WIDTH]=255;
ox=IN_WIDTH/2+post_time/scale;
//fprintf(stderr,"border right: %d\n",ox);
if(ox<0 || oy<0 || ox>=IN_WIDTH || oy>=IN_HEIGHT){continue;}
in[ox+oy*IN_WIDTH]=255;
}

int ooy,noy;
int oox,nox;
// draw wave
for(q=0;q<IN_WIDTH;q+=1){
p=(int)((double)(current_time-pre_time+scale*q)*SAMPLERATE+.5)*CHANNELS*2;

nox=q;
noy=sound[p]/1000+32;
if(q){
draw_line(in,oox,ooy,nox,noy);
}
oox=nox;
ooy=noy;
}
}


void draw_string(uint8_t *in, uint8_t *font, int offset_x, int offset_y, char *text, int text_len){
fprintf(stderr,"draw string \"%*s\" at %dx%d\n",text_len,text,offset_x,offset_y);
int e,ox=offset_x,oy=offset_y,q,w,g,p,a,s;
ox=offset_x,oy=offset_y;
// first pass: draw shadow
for(e=0;e<text_len;e++){
g=(text[e]-32)*8;
for(w=0;w<8;w++){
for(q=0;q<8;q++){
if((font[g+w]>>(7-q))&1){
p=ox+q+(oy+w)*IN_WIDTH;
for(s=-1;s<2;s++){
for(a=-1;a<2;a++){
in[p+a+s*IN_WIDTH]=0;
}
}
}
}
}
ox+=8;
}

// second pass: draw main color
ox=offset_x,oy=offset_y;
for(e=0;e<text_len;e++){
g=(text[e]-32)*8;
for(w=0;w<8;w++){
for(q=0;q<8;q++){
if((font[g+w]>>(7-q))&1){
p=ox+q+(oy+w)*IN_WIDTH;
in[p]=255;
}
}
}
ox+=8;
}


}


int main(int argc, char **argv){
int in_bytes=IN_WIDTH*IN_HEIGHT;
double rate=1.0;
double current_time=0.0;

rate=atof(argv[1]);
current_time=atof(argv[2]);

uint8_t *font=malloc(768);
FILE *ff=fopen(argv[3],"rb");
fread(font,1,768,ff);
fclose(ff);

int16_t *sound=malloc(SOUND_FILE_SIZE);
FILE *sf=fopen("sound.raw","rb");
fread(sound,1,SOUND_FILE_SIZE,sf);
fclose(sf);

uint8_t *in=malloc(in_bytes);
int in_size;

while(1){
in_size=fread(in,1,in_bytes,stdin);
if(in_size<=0){break;}

static char timecode[32];
int hours=current_time/3600;
int minutes=current_time/60-hours*60;
int seconds=current_time-hours*60*60-minutes*60;
int mseconds=(current_time-(int)(current_time))*1000;
sprintf(timecode,"%02d:%02d:%02d.%03d",hours,minutes,seconds,mseconds);
int timecode_len=strlen(timecode);
int offset_x=(IN_WIDTH-timecode_len*8)/2;
int offset_y=2;
draw_string(in,font,offset_x,offset_y,timecode,timecode_len);

draw_sound(in,sound,rate,current_time,.5,0.5);


current_time+=1.0/rate;

fwrite(in,1,in_bytes,stdout);
}

return 0;
}
