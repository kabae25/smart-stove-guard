#include "fsm.h"

void Fsm::init() {
    current_state = CUR_DEBUG;
}

void Fsm::loop() {
    switch (requested_state) {
        case (REQ_DEBUG):
        // adding more here soon
    }
}
