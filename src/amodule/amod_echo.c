    #include <amodule/amod_echo.h>

    amod_echo_obj * amod_echo_construct(const mod_echo_cfg * mod_echo_config, const msg_spectra_cfg * msg_spectra_config, const msg_powers_cfg * msg_powers_config) {

        amod_echo_obj * obj;

        obj = (amod_echo_obj *) malloc(sizeof(amod_echo_obj));

        obj->mod_echo = mod_echo_construct(mod_echo_config, msg_spectra_config, msg_powers_config);
        
        obj->in = (amsg_spectra_obj *) NULL;
        obj->out = (amsg_powers_obj *) NULL;

        obj->thread = thread_construct(&amod_echo_thread, (void *) obj);

        mod_echo_disable(obj->mod_echo);

        return obj;

    }

    void amod_echo_destroy(amod_echo_obj * obj) {

        mod_echo_destroy(obj->mod_echo);
        thread_destroy(obj->thread);

        free((void *) obj);     

    }

    void amod_echo_connect(amod_echo_obj * obj, amsg_spectra_obj * in, amsg_powers_obj * out) {

        obj->in = in;
        obj->out = out;

    }

    void amod_echo_disconnect(amod_echo_obj * obj) {

        obj->in = (amsg_spectra_obj *) NULL;
        obj->out = (amsg_powers_obj *) NULL;

    }

    void amod_echo_enable(amod_echo_obj * obj) {

        mod_echo_enable(obj->mod_echo);

    }

    void amod_echo_disable(amod_echo_obj * obj) {

        mod_echo_disable(obj->mod_echo);

    }

    void * amod_echo_thread(void * ptr) {

        amod_echo_obj * obj;
        msg_spectra_obj * msg_spectra_in;
        msg_powers_obj * msg_powers_out;
        int rtnValue;

        obj = (amod_echo_obj *) ptr;

        while(1) {

            // Pop a message, process, and push back
            msg_spectra_in = amsg_spectra_filled_pop(obj->in);
            msg_powers_out = amsg_powers_empty_pop(obj->out);
            mod_echo_connect(obj->mod_echo, msg_spectra_in, msg_powers_out);
            printf("abc\n");
            rtnValue = mod_echo_process(obj->mod_echo);
            printf("abc");
            mod_echo_disconnect(obj->mod_echo);
            printf("abc");
            amsg_spectra_empty_push(obj->in, msg_spectra_in);
            printf("abc");
            amsg_powers_filled_push(obj->out, msg_powers_out);
            printf("abc");
            // If this is the last frame, rtnValue = -1
            if (rtnValue == -1) {
                break;
            }

        }

    }
