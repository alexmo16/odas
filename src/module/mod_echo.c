    #include <module/mod_echo.h>
    #include <speex/speex_echo.h>
    #include <speex/speex_preprocess.h>
    #include <utils/pcm.h>

    mod_echo_obj * mod_echo_construct(const mod_echo_cfg * mod_echo_config, const msg_hops_cfg * msg_hops_config) {

        mod_echo_obj * obj;

        obj = (mod_echo_obj *) malloc(sizeof(mod_echo_obj));

        obj->in = (msg_hops_obj *) NULL;
        obj->out = (msg_hops_obj *) NULL;

        obj->enabled = 0;

        return obj;
    }

    void mod_echo_destroy(mod_echo_obj * obj) {
        free((void *) obj);
    }

    int mod_echo_process(mod_echo_obj * obj) {
        if (obj->enabled == 0) {
            obj->out = obj->in;
            return 0;
        }

        printf("start echo process.\n");

        const unsigned int nSignals = obj->in->hops->nSignals;
        printf("%d signals\n", nSignals);
        
        unsigned int iChannel;
        unsigned int iSample;
        float sample;
        unsigned int nBytes = 4;
        unsigned int nBytesTotal = 0;
        
        unsigned int hopSize = obj->in->hops->hopSize;
        unsigned int frameSize = hopSize * 2;
        unsigned int sampleRate = obj->in->fS;
        
        printf("hopSize %d\n", hopSize);
        printf("frame %d\n", frameSize);
        short * inBuffer = (short *) malloc(sizeof(char) * nSignals * frameSize * 4);

        printf("inBuffer: %d\n", *inBuffer);
        memset(inBuffer, 0x00, sizeof(char) * nSignals * frameSize * 4);
        
        short * outBuffer = (short *) malloc(sizeof(char) * nSignals * frameSize * 4);
        memset(outBuffer, 0x00, sizeof(char) * nSignals * frameSize * 4);
        
        char bytes[4];
        memset(bytes, 0x00, 4 * sizeof(char));

        for (iSample = 0; iSample < frameSize; iSample++) {

            for (iChannel = 0; iChannel < nSignals; iChannel++) {
                sample = obj->in->hops->array[iChannel][iSample];
                pcm_normalized2signedXXbits(sample, nBytes, bytes);
                memcpy(&(inBuffer[nBytesTotal]), &(bytes[4-nBytes]), sizeof(char) * nBytes);
                
                nBytesTotal += nBytes;
            }
        }

        for (unsigned int iSignal = 0; iSignal < nSignals; iSignal++) {
            //todo lire dans le buffer un size un hopsize
            SpeexEchoState * st;
            SpeexPreprocessState * den;
            st = speex_echo_state_init(hopSize, frameSize);
            den = speex_preprocess_state_init(hopSize, sampleRate);
            
            speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
            speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
            
            speex_echo_capture(st, &inBuffer[iSignal], &outBuffer[iSignal]);
            speex_preprocess_run(den, &outBuffer[iSignal]);

            speex_echo_state_destroy(st);
            speex_preprocess_state_destroy(den);
        }

        for (iSample = 0; iSample < frameSize; iSample++) {

            for (iChannel = 0; iChannel < nSignals; iChannel++) {

                memcpy(&(bytes[4-nBytes]),
                       &(outBuffer[(iSample * nSignals + iChannel) * nBytes]),
                       sizeof(char) * nBytes);

                sample = pcm_signedXXbits2normalized(bytes, nBytes);

                obj->out->hops->array[iChannel][iSample] = sample;
            }
        }

        printf("stop echo process.\n");
        return 0;
    }

    void mod_echo_connect(mod_echo_obj * obj, msg_hops_obj * in, msg_hops_obj * out) {

        obj->in = in;
        obj->out = out;
    }

    void mod_echo_disconnect(mod_echo_obj * obj) {

        obj->in = (msg_hops_obj *) NULL;
        obj->out = (msg_hops_obj *) NULL;
    }

    void mod_echo_enable(mod_echo_obj * obj) {

        obj->enabled = 1;
    }

    void mod_echo_disable(mod_echo_obj * obj) {

        obj->enabled = 0;
    }

    mod_echo_cfg * mod_echo_cfg_construct(void) {

        mod_echo_cfg * cfg;

        cfg = (mod_echo_cfg *) malloc(sizeof(mod_echo_cfg));

        return cfg;
    }

    void mod_echo_cfg_destroy(mod_echo_cfg * cfg) {

        free((void *) cfg);
    }

    void mod_echo_cfg_printf(const mod_echo_cfg * cfg) {
    }
