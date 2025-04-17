#include fsm.h

void app_main() {
    fsm = FSM();
    if (fsm.init()) {
        while (1) {
            fsm.update()
        }
    }
}