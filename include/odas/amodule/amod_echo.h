#ifndef __ODAS_AMODULE_ECHO
#define __ODAS_AMODULE_ECHO

    #include "../module/mod_echo.h"
    #include "../amessage/amsg_spectra.h"
    #include "../amessage/amsg_powers.h"
    #include "../general/thread.h"

    typedef struct amod_echo_obj {

        mod_echo_obj * mod_echo;
        amsg_spectra_obj * in;
        amsg_powers_obj * out;
        thread_obj * thread;    

    } amod_echo_obj;

    amod_echo_obj * amod_echo_construct(const mod_echo_cfg * mod_echo_config, const msg_spectra_cfg * msg_spectra_config, const msg_powers_cfg * msg_powers_config);

    void amod_echo_destroy(amod_echo_obj * obj);

    void amod_echo_connect(amod_echo_obj * obj, amsg_spectra_obj * in, amsg_powers_obj * out);

    void amod_echo_disconnect(amod_echo_obj * obj);

    void amod_echo_enable(amod_echo_obj * obj);

    void amod_echo_disable(amod_echo_obj * obj);

    void * amod_echo_thread(void * ptr);    

#endif