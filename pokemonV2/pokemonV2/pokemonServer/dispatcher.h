#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include"include/json.hpp"
#include "flags.h"
#include "myServer.h"
#include "ClientStruct.h"
#include<random>
using json = nlohmann::json;

class dispatcher {
public:
    dispatcher() {}
    dispatcher(MyServer::Server * server,SOCKET client) :_server(server),_client(client){}

    //Dispatch the request to the corresponding handler
    std::string dispatch(const json &request);


private:
    MyServer::Server * _server;
    SOCKET _client;
    //Handle sign up
    json signUpHandler(const json & request);

    //Handle sign in
    json signInHandler(const json & request);

    //Handle log out
    json logOutHandler(const json & request);

    //Handle the request of getting online client
    json getClientHandler(const json & request);

    //Randomly generate a piece of json representing pokemon
    json randomGeneratePokemon();

    //Construct a piece of json representing pokemon
    json pokemonInfoToJson(const pokemonInfo& info);

    json clientInfoToJson(const clientInfo & info);
    pokemonInfo pokemonJsonToInfo(const json& json);
    //Generate a random number between 0 and 1
    double randomNum();

   
    //Handle the request of getting the pets of one specified client
    json getPetHandler(const json & request);
    bool logInClient(std::string);
    bool logOutClient(std::string);
    bool isOnline(std::string client);
};
#endif