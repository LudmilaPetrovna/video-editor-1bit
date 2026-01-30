#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#define SAMPLERATE 48000
#define PERIOD 4800

#define DURATION 120
#define DURATION_SAMPLES (DURATION*SAMPLERATE)

int16_t *sound;

int main(int argc, char **argv){
int q;
int sample;
srand48(time(0));

double pwnph[4]={1,0,-1,0};

sound=malloc(DURATION_SAMPLES*2);
int cur_period=-1;
double freq=100;

int fd;
int ph;
int ph2;
int ph4;
int pwm;

for(q=0;q<DURATION_SAMPLES;q++){
int mode=q/PERIOD;
int mod=mode&3;
int signal=(mode>>2)&3;
int signal2=(mode>>4)&3;
int enable_signal2=(mode>>6)&1;

if(cur_period!=mode){
freq=2500*drand48()*drand48()+440;
freq=440;
fd=SAMPLERATE/freq;
ph2=fd/2;
ph4=fd/4;
cur_period=mode;
}

ph=q%fd;
pwm=ph/ph4;

////////////////////////////////////////////////////////sample=(drand48()*2.0-1.0)*32768.0;

switch(signal){
 case 0:
sample=sin((double)q*freq*2.0*M_PI/SAMPLERATE)*32768.0;
break;

 case 1:
// saw
sample=((double)(q%fd)/(double)fd*2.0-1.0)*32768.0;
break;

 case 2:
// pwm
sample=pwnph[pwm]*32768.0;
break;

 case 3:
// triangle
sample=(fabs(1.0-(double)(q%fd)*2.0/(double)fd)*2.0-1.0)*32768.0;
break;
}

if(mod&2 && sample>0){sample=0;}
if(mod&1){sample=abs(sample);}

if(enable_signal2){
switch(signal2){
 case 0:
sample+=sin((double)q*freq*2.0*M_PI/SAMPLERATE)*32768.0;
break;

 case 1:
// saw
sample+=((double)(q%fd)/(double)fd*2.0-1.0)*32768.0;
break;

 case 2:
// pwm
sample+=pwnph[pwm]*32768.0;
break;

 case 3:
// triangle
sample+=(fabs(1.0-(double)(q%fd)*2.0/(double)fd)*2.0-1.0)*32768.0;
break;
}


if(mod&2 && sample>0){sample=0;}
if(mod&1){sample=abs(sample);}
}


if(sample>=SHRT_MAX){sample=SHRT_MAX;}
if(sample<=SHRT_MIN){sample=SHRT_MIN;}
sound[q]=(int16_t)sample;

}

fwrite(sound,1,DURATION_SAMPLES*2,stdout);

return 0;
}