    #include <module/mod_echo.h>
    #include <speex/speex_echo.h>
    #include <speex/speex_preprocess.h>
    #include <utils/pcm.h>

    #define NN 128

    mod_echo_obj * mod_echo_construct(const mod_echo_cfg * mod_echo_config, const msg_hops_cfg * msg_hops_config) {
        mod_echo_obj * obj = (mod_echo_obj *) malloc(sizeof(mod_echo_obj));

        obj->in = (msg_hops_obj *) NULL;
        obj->out = (msg_hops_obj *) NULL;

        obj->enabled = 0;
        obj->frameSize = mod_echo_config->frameSize;

        return obj;
    }

    void mod_echo_destroy(mod_echo_obj * obj) {
        free((void *) obj);
    }

    int mod_echo_process(mod_echo_obj * obj) {
        // if the module is disable, the input is directly connected to the output. 
        if (obj->enabled == 0) {
            obj->out = obj->in;
            return 0;
        }

        printf("start echo process.\n");

        const unsigned int nSignals = obj->in->hops->nSignals;
        const unsigned int nBytes = 4;
        const unsigned int hopSize = obj->in->hops->hopSize;
        unsigned int sampleRate = obj->in->fS;
        
        // contains input signal converted in int16.
        short * inBuffer = (short *) malloc(sizeof(char) * nSignals * hopSize * nBytes);
        memset(inBuffer, 0x00, sizeof(char) * nSignals * hopSize * nBytes);
        
        // contains output signal in int16.
        short * outBuffer = (short *) malloc(sizeof(char) * nSignals * hopSize * nBytes);
        memset(outBuffer, 0x00, sizeof(char) * nSignals * hopSize * nBytes);
        
        char bytes[4];
        memset(bytes, 0x00, 4 * sizeof(char));
        
        // Convert floats from the input to int16 bytes.
        unsigned int nBytesTotal = 0;
        for (unsigned int iSample = 0; iSample < hopSize; iSample++) {
            for (unsigned int iChannel = 0; iChannel < nSignals; iChannel++) {
                // extract the float sample
                float sample = obj->in->hops->array[iChannel][iSample];
                // convert the float to a byte array of 4 chars.
                pcm_normalized2signedXXbits(sample, nBytes, bytes);
                // copy the 4 chars in the inBuffer.
                memcpy(&(inBuffer[nBytesTotal]), &(bytes[4-nBytes]), sizeof(char) * nBytes);
                
                nBytesTotal += nBytes;
            }
        }

        // Process the echo cancelling on the int16 bytes.
        for (unsigned int iSignal = 0; iSignal < nSignals; iSignal++) {
            
            // chunk of int16 to send to speex.
            short * chunk = (short *) malloc(sizeof(char) * NN * nBytes);
            memset(chunk, 0x00, sizeof(char) * NN * nBytes);
            
            // Processed int16 signal.
            short * outChunk = (short *) malloc(sizeof(char) * NN * nBytes);
            memset(outChunk, 0x00, sizeof(char) * NN * nBytes);

            for (unsigned int currentChunkID = 0; currentChunkID < hopSize; currentChunkID += NN) {
                
                // take a chunk of bytes (size of hopSize) from inBuffer and put them in the chunk buffer. 
                memcpy(&(chunk[0]), &(inBuffer[currentChunkID]), sizeof(char) * NN * nBytes);

                SpeexEchoState * st;
                SpeexPreprocessState * den;
                st = speex_echo_state_init(NN, hopSize);
                den = speex_preprocess_state_init(NN, sampleRate);

                // Change some variables in speex.
                speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
                speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);

                // Echo process.
                speex_echo_capture(st, chunk, outChunk);
                speex_preprocess_run(den, chunk);

                speex_echo_state_destroy(st);
                speex_preprocess_state_destroy(den);
                
                // Copy Speex output in the outBuffer.
                memcpy(&(outBuffer[currentChunkID]), &(outChunk[0]), sizeof(char) * NN * nBytes);
            }
            free(chunk);
            free(outChunk);
        }

        // Convert back the int16 processed by speex to floats.
        for (unsigned int iSample = 0; iSample < hopSize; iSample++) {

            for (unsigned int iChannel = 0; iChannel < nSignals; iChannel++) {

                memcpy(&(bytes[4-nBytes]),
                       &(outBuffer[(iSample * nSignals + iChannel) * nBytes]),
                       sizeof(char) * nBytes);

                float sample = pcm_signedXXbits2normalized(bytes, nBytes);

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
        mod_echo_cfg * cfg = (mod_echo_cfg *) malloc(sizeof(mod_echo_cfg));
        return cfg;
    }

    void mod_echo_cfg_destroy(mod_echo_cfg * cfg) {

        free((void *) cfg);
    }

    void mod_echo_cfg_printf(const mod_echo_cfg * cfg) {
    }
