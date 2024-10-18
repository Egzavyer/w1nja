#include "../include/peer.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " [server|client] [server_ip_for_client]\n";
        return 1;
    }
    Peer peer;
    std::string mode = argv[1];

    try
    {
        if (mode == "server")
        {
            peer.runServer();
            std::cout << "Press Enter to stop the server..." << std::endl;
            std::cin.get();
            peer.stop();
        }
        else if (mode == "client")
        {
            if (argc < 3)
            {
                peer.runClient("127.0.0.1");
            }
            else
            {
                peer.runClient(argv[2]);
            }
        }
        else
        {
            std::cout << "Exiting...\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
}