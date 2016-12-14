#include "myServer.h"
int main() {
    std::cout << "Pokemon Server Running" << std::endl;
    MyServer::Server server;
    return 0;
}
//#include "include/json.hpp"
//using json = nlohmann::json;
//int main() {
//    json tmp,resultInfo;
//    tmp["account"] = "asdasad";
//    tmp["winRate"] = 0.64;
//    resultInfo["user"] = {};
//    resultInfo["user"].push_back(tmp);
//    tmp["account"] = "zxczxczx";
//    tmp["winRate"] = 0.1231;
//    resultInfo["user"].push_back(tmp);
//    std::cout<<resultInfo.dump();
//    struct clientasd { std::string s;double a; };
//    auto k = resultInfo["user"];
//    std::cout << k.dump()<<std::endl;
//    std::cout << k[0].dump()<<std::endl;
//    //for (auto i : k) {
//    //std::cout << k["s"] << " " << k["a"] << std::endl;
//    //}
//}