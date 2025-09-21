#include <iostream>
#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>

class Server
{
  public:
    Server()
    {
        int rv = nng_rep0_open(&d_sock);
        if (rv != 0)
        {
            std::cerr << "Failed to open NNG socket: " << nng_strerror(rv) << std::endl;
        }
    }

    int start(const std::string *url)
    {
        nng_listener listener;
        int rv = nng_listener_create(&listener, d_sock, url->c_str());
        if (rv != 0)
        {
            std::cerr << "Failed to create NNG listener: " << nng_strerror(rv) << std::endl;
        }

        nng_socket_set_ms(d_sock, NNG_OPT_REQ_RESENDTIME, 2000);
        nng_listener_start(listener, 0);

        // while (true)
        // {

        // }
        return 0;
    }

  private:
    nng_socket d_sock;
    nng_listener d_listener;
};

int main(int argc, char *argv[])
{
    // Test NNG initialization
    nng_socket sock;
    int rv = nng_req0_open(&sock);
    if (rv != 0)
    {
        std::cerr << "Failed to open NNG socket: " << nng_strerror(rv) << std::endl;
        return 1;
    }

    std::cout << "NNG socket opened successfully!" << std::endl;

    // Clean up
    nng_close(sock);

    return 0;
}
