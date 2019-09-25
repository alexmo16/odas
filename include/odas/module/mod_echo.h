#ifndef __ODAS_MODULE_ECHO
#define __ODAS_MODULE_ECHO

    #include "../message/msg_hops.h"

    typedef struct mod_echo_obj {
        msg_hops_obj * in;
        msg_hops_obj * out;        

        char enabled;

    } mod_echo_obj;

    typedef struct mod_echo_cfg {
        
        unsigned int bSize;
        float alphaS;
        unsigned int L;
        float delta;
        float alphaD;

    } mod_echo_cfg;

    mod_echo_obj * mod_echo_construct(const mod_echo_cfg * mod_noise_config, const msg_hops_cfg * msg_hops_config);

    void mod_echo_destroy(mod_echo_obj * obj);

    int mod_echo_process(mod_echo_obj * obj);

    void mod_echo_connect(mod_echo_obj * obj, msg_hops_obj * in, msg_hops_obj * out);

    void mod_echo_disconnect(mod_echo_obj * obj);

    void mod_echo_enable(mod_echo_obj * obj);

    void mod_echo_disable(mod_echo_obj * obj);

    mod_echo_cfg * mod_echo_cfg_construct(void);

    void mod_echo_cfg_destroy(mod_echo_cfg * cfg);

    void mod_echo_cfg_printf(const mod_echo_cfg * cfg);

#endif