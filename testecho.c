#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"
 
#define NN 128
#define TAIL 1024
 
int main(int argc, char **argv)
{
   FILE *mic_fd, *speaker_fd, *out_fd;
   short mic_buf[NN], speaker_buf[NN], out_buf[NN];
   SpeexEchoState *st;
   SpeexPreprocessState *den;
 
   if (argc != 5)
   {
      fprintf(stderr, "testecho 8000 mic_signal.sw speaker_signal.sw output.sw\n");
      exit(1);
   }
   char* sample_rate_ch = argv[1];
   mic_fd = fopen(argv[2], "rb");
   speaker_fd  = fopen(argv[3],  "rb");
   out_fd    = fopen(argv[4], "wb");
   
   int sampleRate = atoi(sample_rate_ch);
   st = speex_echo_state_init(NN, TAIL);
   den = speex_preprocess_state_init(NN, sampleRate);
 
   speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
   speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
 
   while (!feof(mic_fd) && !feof(speaker_fd))
   {
      fread(mic_buf, sizeof(short), NN, mic_fd);
      fread(speaker_buf, sizeof(short), NN, speaker_fd);

      speex_echo_cancellation(st, mic_buf, speaker_buf, out_buf);
      speex_preprocess_run(den, out_buf);
      fwrite(out_buf, sizeof(short), NN, out_fd);
   }
 
   speex_echo_state_destroy(st);
   speex_preprocess_state_destroy(den);
   fclose(mic_fd);
   fclose(speaker_fd);
   fclose(out_fd);
   return 0;
}
