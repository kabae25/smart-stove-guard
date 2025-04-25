#pragma once

#include <stdlib.h>
#include <stdio.h>

class Fsm {
    public:
        void init();
        void loop();
    private:
        enum CURR_STATE {
            CUR_DEBUG,
            CUR_PAIRING,
            CUR_WAITING_FOR_CREDENTIALS,
            CUR_IDLE,
            CUR_WAITING_FOR_END_OF_MOTION,
            CUR_READING_TEMP,
            CUR_WAITING_FOR_COOLDOWN,
            CUR_ALARMING
        };
        CURR_STATE current_state;

        enum REQ_STATE {
            REQ_DEBUG,
            REQ_PAIRING,
            REQ_WAITING_FOR_CREDENTIALS,
            REQ_IDLE,
            REQ_WAITING_FOR_END_OF_MOTION,
            REQ_READING_TEMP,
            REQ_WAITING_FOR_COOLDOWN,
            REQ_ALARMING            
        };
        REQ_STATE requested_state;
    protected:
};