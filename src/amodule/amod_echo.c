    #include <amodule/amod_echo.h>

    amod_echo_obj * amod_echo_construct(const mod_echo_cfg * mod_echo_config, const msg_hops_cfg * msg_hops_config) {

        amod_echo_obj * obj;

        obj = (amod_echo_obj *) malloc(sizeof(amod_echo_obj));

        obj->mod_echo = mod_echo_construct(mod_echo_config, msg_hops_config);
        
        obj->in = (amsg_hops_obj *) NULL;
        obj->out = (amsg_hops_obj *) NULL;

        obj->thread = thread_construct(&amod_echo_thread, (void *) obj);

        mod_echo_disable(obj->mod_echo);

        return obj;

    }

    void amod_echo_destroy(amod_echo_obj * obj) {

        mod_echo_destroy(obj->mod_echo);
        thread_destroy(obj->thread);

        free((void *) obj);     

    }

    void amod_echo_connect(amod_echo_obj * obj, amsg_hops_obj * in, amsg_hops_obj * out) {

        obj->in = in;
        obj->out = out;

    }

    void amod_echo_disconnect(amod_echo_obj * obj) {

        obj->in = (amsg_hops_obj *) NULL;
        obj->out = (amsg_hops_obj *) NULL;

    }

    void amod_echo_enable(amod_echo_obj * obj) {

        mod_echo_enable(obj->mod_echo);

    }

    void amod_echo_disable(amod_echo_obj * obj) {

        mod_echo_disable(obj->mod_echo);

    }

    void * amod_echo_thread(void * ptr) {

        amod_echo_obj * obj;
        msg_hops_obj * msg_hops_in;
        msg_hops_obj * msg_hops_out;
        int rtnValue;

        obj = (amod_echo_obj *) ptr;

        while(1) {

            // Pop a message, process, and push back
            msg_hops_in = amsg_hops_filled_pop(obj->in);
            msg_hops_out = amsg_hops_empty_pop(obj->out);
            mod_echo_connect(obj->mod_echo, msg_hops_in, msg_hops_out);

            rtnValue = mod_echo_process(obj->mod_echo);
            mod_echo_disconnect(obj->mod_echo);

            amsg_hops_empty_push(obj->in, msg_hops_in);
            amsg_hops_filled_push(obj->out, msg_hops_out);
            // If this is the last frame, rtnValue = -1
            if (rtnValue == -1) {
                break;
            }

        }

    }
