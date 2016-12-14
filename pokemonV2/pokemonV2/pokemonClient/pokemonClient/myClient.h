#ifndef _MYCLIENT_H
#define _MYCLIENT_H
#include "ClientStruct.h"
#include "mySocket.h"
#include "include/json.hpp"
#include "flags.h"
#include <vector>
#include <list>
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT "5902"
#define BUFSIZE 4096
using json = nlohmann::json;
using namespace mySocket;
namespace MyClient {
    class Client: sockInit {
    public:
        Client();
        bool signIn(json& request);
		bool signUp(json& request);
		bool logOut(); 
        bool getOnlineClient();
        bool getAllClient();
        std::vector<pokemonInfo> getClientPet(const std::string & client);
        json  constructJsonClient();
        json  constructjson_GUI();
		json communicateWithServerTCP(const json & message);
        pokemonInfo constructPokemon(const json & pokemon);
        clientInfo clientJsonToInfo(const json & client);
	private:
		SOCKET _clientSock;
		clientInfo _client;
        std::list<pokemonInfo> _pokemon;
        std::vector<clientInfo> _onlineClient;
        std::vector<clientInfo> _allClient;
    };
}

#endif
