#include <iostream>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>

int main()
{
    nng_socket sub_sock;
    nng_sub0_open(&sub_sock);

    // Connect to broker's subscriber port
    nng_dial(sub_sock, "tcp://localhost:5556", NULL, 0);

    // Subscribe to all messages (empty subscription)
    nng_socket_set(sub_sock, NNG_OPT_SUB_SUBSCRIBE, "", 0);

    // Receive messages
    while (true) {
        char *msg = nullptr;
        size_t msg_len = 0;

        int rv = nng_recv(sub_sock, &msg, &msg_len, NNG_FLAG_ALLOC);
        if (rv == 0) {
            std::cout << "Received: " << std::string(msg, msg_len) << std::endl;
            nng_free(msg, msg_len);
        }
    }

    nng_close(sub_sock);
    return 0;
}