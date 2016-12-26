#include "dispatcher.h"
#include "ClientStruct.h"
#include "mySocket.h"
using namespace BOT_ORM;
using namespace MyServer;
namespace MyServer {
    //const std::string pokemonName[8] = { "Æ¤¿¨Çð","À×¹«","¶¾´ÌË®Ä¸","Ë®¾ý","Åç»ðÁú","Ñ×µÛ","ÃîÍÜ»¨","ÏòÈÕ»¨" };
    const std::string pokemonType[8] = { "Pikachu","Raikou","Tentacruel","Suicune","Charizard","Entei","Venusaur","Sunflora" };

}
std::string dispatcher::dispatch(const json & request)
{
    json response;
    switch (request["type"].get<int>()) {
    case SIGN_IN:
        response = signInHandler(request);
        break;
    case SIGN_UP:
        response = signUpHandler(request);
        break;
    case LOG_OUT:
        response = logOutHandler(request);
        break;
    case GET_ONLINE_CLIENT:
        response = getClientHandler(request);
        break;
    case GET_ALL_CLIENT:
        response = getClientHandler(request);
        break;
    case GET_CLIENT_PET:
        response = getPetHandler(request);
        break;
    case SET_PET_NAME:
        response = setPetNameHandler(request);
        break;
    case UPDATE_PET:
        response = updatePetHandler(request);
        break;
    case UPDATE_CLIENT:
        response = updateClientHandler(request);
        break;
    case GET_ONE_MORE_POKEMON:
        response = getOneMoreHandler(request);
        break;
    default:
        response["type"] = SERVER_ERROR;
        std::cout << "[ERROR] Bad request" << std::endl;
        break;
    }
    return std::move(response.dump());
    //return request;
}

json dispatcher::signInHandler(const json &requestInfo)
{
    json resultInfo;
    std::cout << "[Info] Receive a request for sign in!\n";
    if (requestInfo["type"].get<int>() == SIGN_IN) {
        clientInfo clientHelper;
        pokemonInfo pokemonHelper;
        FieldExtractor field{ clientHelper,pokemonHelper };
        ORMapper mapper("data.db");
        auto result = mapper.Query(clientHelper).Where(field(clientHelper.account) == requestInfo["account"].get<std::string>()
            && field(clientHelper.password) == requestInfo["password"].get<std::string>()).ToList();
        if (result.size() != 0) {
            if (isOnline(requestInfo["account"].get<std::string>()))//the account is already online
                resultInfo["type"] = SIGN_IN_FAIL;
            resultInfo["type"] = SIGN_IN_SUCCESS;//log in successfully
            json tmpInfo;
            tmpInfo = clientInfoToJson(result.front());
            resultInfo["clientInfo"] = tmpInfo;
            auto pet = mapper.Query(pokemonHelper).Where(field(pokemonHelper.client) == requestInfo["account"].get<std::string>()).ToList();
            for (auto &i : pet) {//return all the pokemon belonging to the client
                resultInfo["petList"].push_back(pokemonInfoToJson(i));
            }
            resultInfo["pokemonNum"] = pet.size();
            std::cout << "[Info] Sucessfully sign in!\n";
            logInClient(requestInfo["account"].get<std::string>());
        }
        else {
            resultInfo["type"] = SIGN_IN_FAIL;
            resultInfo["reason"] = "Unexisted account OR wrong password!";
            std::cout << "[Info] Fail to sign in!\n";
        }
        return std::move(resultInfo);
    }
    std::cout << "[Info] Unknown error at server side.\n";
    resultInfo["type"] = SERVER_ERROR;
    return std::move(resultInfo);
}

json dispatcher::signUpHandler(const json & requestInfo)
{
    json resultInfo;
    std::cout << "[Info] Receive a request for registering!\n";
    if (requestInfo["type"].get<int>() == SIGN_UP) {
        clientInfo clientHelper;
        pokemonInfo pokemonHelper;
        FieldExtractor field{ clientHelper,pokemonHelper };
        ORMapper mapper("data.db");
        auto result = mapper.Query(clientHelper).Where(field(clientHelper.account) == requestInfo["account"].get<std::string>()
        /*&& field(clientHelper.password) == requestInfo["password"].get<std::string>()*/).ToList();
        if (result.size() == 0) {

            clientHelper.account = requestInfo["account"].get<std::string>();
            clientHelper.password = requestInfo["password"].get<std::string>();
            clientHelper.winRate = 0;
            clientHelper.winNum = 0;
            clientHelper.loseNum = 0;
            clientHelper.numMedal = 0;
            clientHelper.levelMedal = 0;
            mapper.Insert(clientHelper);
            resultInfo["clientInfo"] = clientInfoToJson(clientHelper);
            resultInfo["type"] = SIGN_UP_SUCCESS;
            int index = mapper.Query(pokemonHelper).Where(field(pokemonHelper.pokemonId) != -1).ToList().size();
            for (int i = 0;i <= 2;++i) {//Randomly generating three pokemon
                auto pokemon = randomGeneratePokemon();
                pokemon["client"] = clientHelper.account;
                pokemon["pokemonId"] = index++;
                resultInfo["pokemon"].push_back(pokemon);
                pokemonHelper = pokemonJsonToInfo(pokemon);
                pokemonHelper.client = clientHelper.account;
                mapper.Insert(pokemonHelper);
            }
            resultInfo["pokemonNum"] = 3;
            std::cout << "[Info] Successfully registered!\n";
            //logInClient(requestInfo["account"].get<std::string>());
        }
        else {
            resultInfo["type"] = SIGN_UP_FAIL;
            resultInfo["reason"] = std::string("Duplicate client name!");
            std::cout << "[Info] Fail to register!\n";
        }
        return std::move(resultInfo);
    }
    std::cout << "[Info] Unknown error at server side.\n";
    resultInfo["type"] = SERVER_ERROR;
    return std::move(resultInfo);
}

bool dispatcher::isOnline(std::string client)
{
    auto onlineClient = _server->getOnlineClient();
    for (auto i = onlineClient.begin();i != onlineClient.end();++i) {
        if ((*i).first == client)
            return true;
    }
    return false;
}

json dispatcher::logOutHandler(const json & request)
{
    json resultInfo;
    if (request["type"].get<int>() == LOG_OUT) {
        if (logOutClient(request["account"].get<std::string>()))
            resultInfo["type"] = LOG_OUT_ACK;
    }
    else
        resultInfo["type"] = SERVER_ERROR;
    return std::move(resultInfo);
}

json dispatcher::getClientHandler(const json & requestInfo)
{
    json resultInfo;
    if (requestInfo["type"].get<int>() == GET_ALL_CLIENT) {
        clientInfo helper;
        FieldExtractor field{ helper };
        ORMapper mapper("data.db");
        auto result = mapper.Query(helper).Where(field(helper.account) != std::string()).ToList();
        resultInfo["type"] = ALL_CLIENT_ACK;
        resultInfo["num"] = result.size();
        //resultInfo["client"] = {};
        for (auto i : result) {
            //Serialize the client information
            json tmp;
            tmp = clientInfoToJson(i);
            if (isOnline(i.account))
                tmp["state"] = 1;
            else
                tmp["state"] = 0;
            resultInfo["client"].push_back(tmp);
        }
        return std::move(resultInfo);
    }
    else if (requestInfo["type"].get<int>() == GET_ONLINE_CLIENT) {
        clientInfo helper;
        FieldExtractor field{ helper };
        ORMapper mapper("data.db");
        auto result = mapper.Query(helper).Where(field(helper.account) != std::string()).ToList();
        resultInfo["type"] = ONLINE_CLIENT_ACK;
        resultInfo["num"] = result.size();
        //resultInfo["client"] = {};
        for (auto i : result) {
            //Serialize the client information
            if (!isOnline(i.account))
                continue;
            json tmp;
            tmp = clientInfoToJson(i);
            resultInfo["client"].push_back(tmp);
        }
        return std::move(resultInfo);
    }
    else {
        resultInfo["type"] = SERVER_ERROR;
        return std::move(resultInfo);
    }
}

json dispatcher::randomGeneratePokemon()
{
    pokemonInfo randomAttr;
    int randomIndex = static_cast<int>((10 * randomNum())) % 8;
    //randomAttr._name = pokemonName[randomIndex];
    randomAttr._name = pokemonType[randomIndex];
    randomAttr.type = pokemonType[randomIndex];
    randomAttr.level = 1;
    randomAttr.client = std::string();
    randomAttr.exp = 0;
    randomAttr.healthMax = static_cast<int>(400 * (0.8 + randomNum()));
    randomAttr.expMax = 100;
    randomAttr.health = randomAttr.healthMax;
    randomAttr.physicalAtk = static_cast<int>(60 * (0.5 + randomNum()));
    randomAttr.magicAtk = static_cast<int>(60 * (0.5 + randomNum()));
    randomAttr.physicalDef = static_cast<int>(60 * (0.5 + randomNum()));
    randomAttr.magicDef = static_cast<int>(60 * (0.5 + randomNum()));
    randomAttr.atkSpeed = randomNum() * 2 + 4;
    randomAttr.critRate = randomNum();
    randomAttr.speed = randomNum();
    return std::move(pokemonInfoToJson(randomAttr));
}

json dispatcher::pokemonInfoToJson(const pokemonInfo & info)
{
    json result;
    result["client"] = info.client;
    result["pokemonId"] = info.pokemonId;
    result["_name"] = info._name;
    result["type"] = info.type;
    result["level"] = info.level;
    result["exp"] = info.exp;
    result["healthMax"] = info.healthMax;
    result["expMax"] = info.expMax;
    result["health"] = info.health;
    result["physicalAtk"] = info.physicalAtk;
    result["magicAtk"] = info.magicAtk;
    result["physicalDef"] = info.physicalDef;
    result["magicDef"] = info.magicDef;
    result["atkSpeed"] = info.atkSpeed;
    result["critRate"] = info.critRate;
    result["speed"] = info.speed;
    return std::move(result);
}

pokemonInfo dispatcher::pokemonJsonToInfo(const json & pokemonJson)
{
    pokemonInfo result;
    result.pokemonId = pokemonJson["pokemonId"].get<int>();
    result._name = pokemonJson["_name"].get<std::string>();
    result.type = pokemonJson["type"].get<std::string>();
    result.level = pokemonJson["level"].get<int>();
    result.exp = pokemonJson["exp"].get<int>();
    result.healthMax = pokemonJson["healthMax"].get<int>();
    result.expMax = pokemonJson["expMax"].get<int>();
    result.health = pokemonJson["health"].get<int>();
    result.physicalAtk = pokemonJson["physicalAtk"].get<int>();
    result.magicAtk = pokemonJson["magicAtk"].get<int>();
    result.physicalDef = pokemonJson["physicalDef"].get<int>();
    result.magicDef = pokemonJson["magicDef"].get<int>();
    result.atkSpeed = pokemonJson["atkSpeed"].get<double>();
    result.critRate = pokemonJson["critRate"].get<double>();
    result.speed = pokemonJson["speed"].get<double>();
    result.client = pokemonJson["client"].get<std::string>();
    //result.client = pokemonJson["account"].get<std::string>();
    return std::move(result);
}

json dispatcher::clientInfoToJson(const clientInfo & info) {
    json tmp;
    tmp["account"] = info.account;
    tmp["winRate"] = info.winRate;
    tmp["winNum"] = info.winNum;
    tmp["loseNum"] = info.loseNum;
    tmp["numMedal"] = info.numMedal;
    tmp["levelMedal"] = info.levelMedal;
    tmp["password"] = info.password;
    //std::cout <<"Debug"<< tmp.dump() << std::endl;
    return std::move(tmp);
}

clientInfo dispatcher::clientJsonToInfo(const json& client) {
    clientInfo info;
    info.account = client["account"].get<std::string>();
    info.password = client["password"].get<std::string>();
    info.levelMedal = client["levelMedal"].get<int>();
    info.numMedal = client["numMedal"].get<int>();
    info.winNum = client["winNum"].get<int>();
    info.loseNum = client["loseNum"].get<int>();
    info.winRate = info.winNum*1.0 / (info.winNum + info.loseNum);
    return std::move(info);
}
json dispatcher::getOneMoreHandler(const json & request)
{
    json resultInfo;
    if (request["type"].get<int>() == GET_ONE_MORE_POKEMON) {
        pokemonInfo pokemonHelper;
        FieldExtractor field{ pokemonHelper };
        ORMapper mapper("data.db");
        auto pokemon = randomGeneratePokemon();
        int index = mapper.Query(pokemonHelper).Where(field(pokemonHelper.pokemonId) != -1).ToList().size();
        pokemon["client"] = request["client"].get<std::string>();
        pokemon["pokemonId"] = index++;
        resultInfo["pokemon"].push_back(pokemon);
        pokemonHelper = pokemonJsonToInfo(pokemon);
        pokemonHelper.client = request["client"].get<std::string>();
        mapper.Insert(pokemonHelper);
        resultInfo["type"] = GET_ONE_MORE_POKEMON_SUCCESS;
    }
    else {
        resultInfo["type"] = SERVER_ERROR;
    }
    return std::move(resultInfo);
}
double dispatcher::randomNum()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    return std::generate_canonical<double, 10>(gen);
}

json dispatcher::setPetNameHandler(const json & request)
{
    json resultInfo;
    if (request["type"].get<int>() == SET_PET_NAME) {
        pokemonInfo helper;
        FieldExtractor field{ helper };
        ORMapper mapper("data.db");

        auto result = mapper.Query(helper).Where(field(helper.pokemonId) == request["pokemonId"].get<int>()).ToList();
        if (result.size() == 1) {
            result.front()._name = request["name"].get<std::string>();
            mapper.Update(result.front());
            resultInfo["type"] = SET_PET_NAME_SUCCESS;
        }
        else {
            resultInfo["type"] = SET_PET_NAME_FAIL;
        }
    }
    else {
        resultInfo["type"] = SERVER_ERROR;
    }
    return std::move(resultInfo);
}

json dispatcher::updatePetHandler(const json & request)
{
    json resultInfo;
    if (request["type"].get<int>() == UPDATE_PET) {
        pokemonInfo helper;
        FieldExtractor field{ helper };
        ORMapper mapper("data.db");

        auto result = mapper.Query(helper).Where(field(helper.pokemonId) == request["pokemon"]["pokemonId"].get<int>()).ToList();
        if (result.size() == 1) {
            auto &pet = result.front();
            std::string client = pet.client;
            pet = pokemonJsonToInfo(request["pokemon"]);
            if (pet.client != "Server")
                pet.client = client;
            mapper.Update(pet);
            resultInfo["type"] = UPDATE_PET_SUCCESS;
            resultInfo["pokemonId"] = pet.pokemonId;
        }
        else {
            resultInfo["type"] = UPDATE_PET_FAIL;
            resultInfo["reason"] = "Error in data base!";
        }
    }
    else {
        resultInfo["type"] = SERVER_ERROR;
        resultInfo["reason"] = "Error in server!";
    }
    return std::move(resultInfo);
}

json dispatcher::updateClientHandler(const json & request)
{
    json resultInfo;
    if (request["type"].get<int>() == UPDATE_CLIENT) {
        clientInfo helper;
        FieldExtractor field{ helper };
        ORMapper mapper("data.db");

        auto result = mapper.Query(helper).Where(field(helper.account) == request["client"]["account"].get<std::string>()).ToList();
        if (result.size() == 1) {
            auto &client = result.front();
            std::string tmpPwd = client.password;
            client = clientJsonToInfo(request["client"]);
            if (request["client"]["password"] == std::string())
                client.password = tmpPwd;
            mapper.Update(client);
            resultInfo["type"] = UPDATE_CLIENT_SUCCESS;
            resultInfo["account"] = helper.account;
        }
        else {
            resultInfo["type"] = UPDATE_CLIENT_FAIL;
            resultInfo["reason"] = "Error in data base!";
        }
    }
    else {
        resultInfo["type"] = SERVER_ERROR;
        resultInfo["reason"] = "Error in server!";
    }
    return std::move(resultInfo);
}

json dispatcher::getPetHandler(const json & request)
{
    json resultInfo;
    if (request["type"].get<int>() == GET_CLIENT_PET) {
        pokemonInfo helper;
        FieldExtractor field{ helper };
        ORMapper mapper("data.db");
        auto result = mapper.Query(helper).Where(field(helper.client) == request["client"].get < std::string >()).ToList();
        resultInfo["type"] = CLIENT_PET_ACK;
        resultInfo["num"] = result.size();
        for (auto i : result) {
            resultInfo["pokemon"].push_back(pokemonInfoToJson(i));
        }
        return std::move(resultInfo);
    }
    return json();
}
bool dispatcher::logInClient(std::string client)
{
    auto & onlineClient = _server->getOnlineClient();
    if (!isOnline(client)) {
        onlineClient.insert(std::make_pair(client, _client));
        return true;
    }
    return false;

}
bool dispatcher::logOutClient(std::string client)
{
    auto & onlineClient = _server->getOnlineClient();

    if (isOnline(client)) {
        auto j = onlineClient.end();
        for (auto i = onlineClient.begin();i != onlineClient.end();++i) {
            if ((*i).first == client)
                j = i;
        }
        if (j != onlineClient.end()) {
            onlineClient.erase(j);
            return true;
        }
        else
            return false;
    }
    return false;

}

