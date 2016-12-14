#ifndef _CLIENT_H
#define _CLIENT_H
#include<iostream>
#include<string>

struct clientInfo {
    std::string account;
    std::string password;
    int winNum;
    int loseNum;
    double winRate;
    int numMedal;
    int levelMedal;

};

struct pokemonInfo {
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
};
#endif