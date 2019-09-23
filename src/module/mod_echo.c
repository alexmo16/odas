    #include <module/mod_echo.h>
    #include <speex/speex_echo.h>
    #include <speex/speex_preprocess.h>

    mod_echo_obj * mod_echo_construct(const mod_echo_cfg * mod_echo_config, const msg_spectra_cfg * msg_spectra_config, const msg_powers_cfg * msg_powers_config) {

        mod_echo_obj * obj;

        obj = (mod_echo_obj *) malloc(sizeof(mod_echo_obj));

        obj->freq2env = freq2env_construct_zero(msg_spectra_config->halfFrameSize);

        obj->envs = envs_construct_zero(msg_powers_config->nChannels,
                                        msg_powers_config->halfFrameSize);

        obj->env2env_mcra = env2env_mcra_construct_zero(msg_powers_config->nChannels, 
                                                        msg_powers_config->halfFrameSize, 
                                                        mod_echo_config->bSize, 
                                                        mod_echo_config->alphaS, 
                                                        mod_echo_config->L, 
                                                        mod_echo_config->delta, 
                                                        mod_echo_config->alphaD);

        obj->in = (msg_spectra_obj *) NULL;
        obj->out = (msg_powers_obj *) NULL;

        obj->enabled = 0;

        return obj;

    }

    void mod_echo_destroy(mod_echo_obj * obj) {

        freq2env_destroy(obj->freq2env);
        envs_destroy(obj->envs);
        env2env_mcra_destroy(obj->env2env_mcra);

        free((void *) obj);

    }

    int mod_echo_process(mod_echo_obj * obj) {
        int NN = 128;
        int TAIL = 1024;
        int sampleRate = 48000;

        printf("start echo process.\n");

        const unsigned int nSignals = obj->out->envs->nSignals;
        printf("%d signals\n", nSignals);

        for (unsigned int iSignal = 0; iSignal < nSignals; iSignal++) {
            printf("1.\n");
            SpeexEchoState *st;
            SpeexPreprocessState *den;
            int speexRtnValue = speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
            if (speexRtnValue != 0) {
                return -1;
            }
            speexRtnValue = speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
            if (speexRtnValue != 0) {
                return -1;
            }
            st = speex_echo_state_init(NN, TAIL);
            den = speex_preprocess_state_init(NN, sampleRate);
            
            speex_echo_capture(st, obj->in->freqs->array, obj->out->envs->array[iSignal]);
            speex_preprocess_run(den, obj->out->envs->array[iSignal]);

            speex_echo_state_destroy(st);
            speex_preprocess_state_destroy(den);
        }

        printf("stop echo process.\n");
        return 0;
    }

    void mod_echo_connect(mod_echo_obj * obj, msg_spectra_obj * in, msg_powers_obj * out) {

        obj->in = in;
        obj->out = out;

    }

    void mod_echo_disconnect(mod_echo_obj * obj) {

        obj->in = (msg_spectra_obj *) NULL;
        obj->out = (msg_powers_obj *) NULL;

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
