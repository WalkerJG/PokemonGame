#include "myClient.h"

MyClient::Client::Client():sockInit()
{
    char BUF[BUFSIZE];
    addrinfo hints, *serverinfo, *p;

	//Get the address information of the server
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE;
    if (0 != getaddrinfo(SERVER_ADDR, SERVER_PORT, &hints, &serverinfo)) {
        throw std::runtime_error("Fail at get the address information of the server");
    }

	//socket and connect
    for (p = serverinfo;p != NULL;p = p->ai_next) {
        _clientSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (SOCKET_ERROR == _clientSock)
            continue;
        if (SOCKET_ERROR == connect(_clientSock, p->ai_addr, p->ai_addrlen)) {
            closesocket(_clientSock);
            _clientSock = SOCKET_ERROR;
            continue;
        }
        break;
    }
    if (SOCKET_ERROR == _clientSock) {
        throw std::runtime_error("Fail at socket() and connect()!");
    }
    
    //inet_ntop(p->ai_family, &((sockaddr_in*)p->ai_addr)->sin_addr), BUF, p->ai_addrlen);
    inet_ntop(p->ai_family, &(((sockaddr_in*)((addrinfo*)p->ai_addr))->sin_addr), BUF, p->ai_addrlen);
    std::cout <<"[Info] Connecting to "<<BUF<< std::endl;
	freeaddrinfo(serverinfo);
	
    auto clientJson = constructJsonClient();
    auto s = clientJson.dump();
    signUp(clientJson);
    signIn(clientJson);
    //getOnlineClient();
    //logOut();
    ////logOut();
    //getAllClient();
    getClientPet(_client.account);
	while(true){
		
	}

}

bool MyClient::Client::signIn(json & request)
{
    request["type"] = SIGN_IN;
    auto result = communicateWithServerTCP(request);
    if (result["type"].get<int>() == SIGN_IN_SUCCESS) {
        auto pokemon = result["pokemon"];
        for (auto i : pokemon) {
            std::cout << i.dump() << std::endl;
            _pokemon.push_back(constructPokemon(i));
        }
        auto client = result["clientInfo"];
        _client = clientJsonToInfo(client);
        return true;
    }
    else if (result["type"].get<int>() == SIGN_IN_FAIL) {
        std::cout << result["reason"].get<std::string>() << std::endl;
        return false;
    }
    else {//Error happens at the server side
        std::cout<<result["reason"].get<std::string>() << std::endl;
        return false;
    }
}

bool MyClient::Client::signUp( json & request)
{
    request["type"] = SIGN_UP;
    auto result = communicateWithServerTCP(request);
    if (result["type"].get<int>() == SIGN_UP_SUCCESS) {
        auto pokemon = result["pokemon"];
        //std::cout << pokemon.dump() << std::endl;
        //for (int i = result["pokemonNum"].get<int>();i >= 1;--i) {
        //    auto pet = pokemon[i];
        //    std::cout<<pokemon.front().dump()<<std::endl;
        //    std::cout << pet.dump()<<std::endl;
        //    std::cout << pet.get<std::string>() << std::endl;
        //    _pokemon.push_back(constructPokemon(pet));
        //}
        for (auto i : pokemon) {
            std::cout << i.dump() << std::endl;
            _pokemon.push_back(constructPokemon(i));//Todo:Display three pokemon rather than put them into _pokemon
        }
        auto client = result["clientInfo"];
        _client = clientJsonToInfo(client);
        return true;
    }
    else if (result["type"].get<int>() == SIGN_UP_FAIL) {
        std::cout << result["reason"].get<std::string>() << std::endl;
        return false;
    }
    else {//Error happens at the server side
        std::cout << result["reason"].get<std::string>() << std::endl;
        return false;
    }
    return true;
}

bool MyClient::Client::logOut()
{
    json request;
    if (_client.account == std::string()) {
        throw std::runtime_error("Already in the offline state!\n");
        return false;
    }
    request["account"] = _client.account;
    request["type"] = LOG_OUT;
    auto result = communicateWithServerTCP(request);
    if (result["type"].get<int>() == LOG_OUT_ACK) {
        _client = { std::string(),std::string(),0,0,0,0,0};
        _pokemon.clear();
        return true;
    }
    else {
        std::cout << result["reason"].get<std::string>() << std::endl;
        return false;
    }
    return true;
}

bool MyClient::Client::getOnlineClient()
{
    json request;
    request["type"] = GET_ONLINE_CLIENT;
    auto result = communicateWithServerTCP(request);
    if (result["type"].get<int>() == ONLINE_CLIENT_ACK) {
        auto onlineClient = result["client"];
        for (auto i : result["client"]) {
            _onlineClient.push_back(clientJsonToInfo(i));
        }
        return true;
    }
    else {
        std::cout << result["reason"].get<std::string>() << std::endl;
        return false;
    }
}

bool MyClient::Client::getAllClient()
{
    json request;
    request["type"] = GET_ALL_CLIENT;
    auto result = communicateWithServerTCP(request);
    if (result["type"].get<int>() == ALL_CLIENT_ACK) {
        //Todo:parse the packet and a
        auto allClient = result["client"];
        for (auto i : allClient) {
            _allClient.push_back(clientJsonToInfo(i));
        }
        return true;
    }
    else {
        std::cout << result["reason"].get<std::string>() << std::endl;
        return false;
    }
}

std::vector<pokemonInfo> MyClient::Client::getClientPet(const std::string & client)
{
    json request;
    std::vector<pokemonInfo> pokemonList;
    request["type"] = GET_CLIENT_PET;
    request["client"] = client;
    auto result = communicateWithServerTCP(request);
    if (result["type"].get<int>() == CLIENT_PET_ACK) {
        //Todo:parse the packet and a
        auto pokemon = result["pokemon"];
        for (auto i : pokemon) {
            pokemonList.push_back(constructPokemon(i));
        }
        return std::move(pokemonList);
    }
    else {
        std::cout << result["reason"].get<std::string>() << std::endl;
        return std::move(pokemonList);
    }
}

json  MyClient::Client::constructJsonClient()
{
    std::cout << "Please input you account!\n";
    std::cin >> _client.account;
    std::cout << "Please input you password!\n";
    std::cin >> _client.password;
    json request;
    //request["type"] = SIGN_IN;
    request["account"] = _client.account;
    request["password"] = _client.password;

    return std::move(request);
 
}

json MyClient::Client::communicateWithServerTCP(const json & message)
{
    int recvBufLen = BUFSIZE;
    char recvBuf[BUFSIZE];

    std::string strMessage = message.dump();
    std::cout << "[Send] request: " << strMessage.c_str() << std::endl;
    ;
    if (send(_clientSock, strMessage.c_str(), strMessage.length(), 0) != strMessage.length())
    {
        closesocket(_clientSock);
        throw std::runtime_error("Failed at send message");
    }
    std::cout << "[INFO] send complete" << std::endl;
    int actLen = recv(_clientSock, recvBuf, recvBufLen, 0);
    if (actLen <= 0)
    {
        closesocket(_clientSock);
        throw std::runtime_error("Failed at receive message, the server may be lost!");
    }
    if (actLen > 0) {
        recvBuf[actLen] = 0;
        std::cout << "[Recv] receive: " << recvBuf << std::endl;
    }

    return std::move(json::parse(recvBuf));
}

pokemonInfo MyClient::Client::constructPokemon(const json & pokemonJson)
{
    pokemonInfo pokemon;
    pokemon._name = pokemonJson["_name"].get<std::string>() ;
    pokemon.type = pokemonJson["type"].get<std::string>();
    pokemon.level = pokemonJson["level"].get<int>();
    pokemon.exp = pokemonJson["exp"].get<int>();
    pokemon.healthMax = pokemonJson["healthMax"].get<int>();
    pokemon.expMax = pokemonJson["expMax"].get<int>();
    pokemon.health = pokemonJson["health"].get<int>();
    pokemon.physicalAtk = pokemonJson["physicalAtk"].get<int>();
    pokemon.magicAtk = pokemonJson["magicAtk"].get<int>();
    pokemon.physicalDef = pokemonJson["physicalDef"].get<int>();
    pokemon.magicDef = pokemonJson["magicDef"].get<int>();
    pokemon.atkSpeed = pokemonJson["atkSpeed"].get<double>();
    pokemon.critRate = pokemonJson["critRate"].get<double>();
   pokemon.speed  = pokemonJson["speed"].get<double>();
    return std::move(pokemon);
    
     
}

clientInfo MyClient::Client::clientJsonToInfo(const json & clientJson)
{
    clientInfo client;
    client.account = clientJson["account"].get<std::string>();
    client.winNum = clientJson["winNum"].get<int>();
    client.loseNum = clientJson["loseNum"].get<int>();
    client.winRate = clientJson["winRate"].get<double>();
    client.numMedal = clientJson["numMedal"].get<int>();
    client.levelMedal = clientJson["levelMedal"].get<int>();
    return std::move(client);
}
