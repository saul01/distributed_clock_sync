#include "common/audio_node/audio_node.h"

static constexpr uint16_t LEFT_PORT  = 9001;
static constexpr uint16_t RIGHT_PORT = 9002;

int main() {
    audio_node right_node(RIGHT_PORT, LEFT_PORT);
    right_node.wait_for_start();

    while (right_node.is_running()) {
        ;
    }
}
