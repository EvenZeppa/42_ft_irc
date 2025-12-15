#pragma once

#include <string>
#include <exception>

// code RFC 1459
#define RPL_WELCOME 001
#define RPL_UMODEIS 221
#define RPL_AWAY 301
#define RPL_WHOISUSER 311
#define RPL_ENDOFWHO 315
#define RPL_CHANNELMODEIS 324
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_NAMREPLY 353
#define RPL_INVITING 341
#define RPL_ENDOFNAMES 366
#define RPL_UMODEUNKNOWNFLAG 501

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS 405
#define ERR_TOOMANYTARGETS 407
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_NOTOPLEVEL 413
#define ERR_WILDTOPLEVEL 414
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NICKCOLLISION 436
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_KEYSET 467
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANNELKEY 475
#define ERR_CHANOPRIVSNEEDED 482

#define ERR_USERSDONTMATCH 502

class IrcReplies : public std::exception {
private:   
    int _code;
    std::string _arg;
public:
    IrcReplies(int code, const std::string& arg = "") : _code(code), _arg(arg) {}
    virtual ~IrcReplies() throw() {}

    int Code() const { return _code; } 
    const std::string& Arg() const { return _arg; }

    virtual const char* what() const throw() {
        return _arg.c_str();
    }

};