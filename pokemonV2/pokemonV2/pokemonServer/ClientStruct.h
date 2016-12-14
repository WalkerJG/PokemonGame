#ifndef _CLIENT_H
#define _CLIENT_H
#include<iostream>
#include<string>
#include"include/ORMLite.h"

struct clientInfo {
    std::string account;
    std::string password;
    int winNum;
    int loseNum;
    double winRate;
    int numMedal;
    int levelMedal;
    //Inject ORMLite into this class
    ORMAP("clientInfo", account, password,winNum,loseNum,winRate,numMedal,levelMedal);
};

struct pokemonInfo {
    int pokemonId;
    std::string client;
    std::string _name;
    std::string type;
    int level;
    int exp;
    int healthMax;
    int expMax;
    int health;
    int physicalAtk;//physical attack power
    int magicAtk;//magic attack power
    int physicalDef;//physical defensive
    int magicDef;//magic defensive
    double atkSpeed;
    double critRate;
    double speed;

    ORMAP("pokemonInfo", pokemonId,client,_name, type, level, exp, healthMax, expMax,
        health, physicalAtk, magicAtk, physicalDef, magicDef, atkSpeed, critRate, speed);
};
#endif