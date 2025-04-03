/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 18:54:48 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/03 15:56:06 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Reply.hpp"

namespace Reply {
    std::string r_RPL_WELCOME(const std::vector<std::string> &av) {
        if (av.empty()) return ": 001 * :Welcome to the Internet Relay Network\r\n";
        return ":" + av[0] + " 001 " + av[0] + " :Welcome to the Internet Relay Network\r\n";
    }

    std::string r_RPL_NOTOPIC(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 331 * * :No topic is set\r\n";
        return ":" + av[0] + " 331 " + av[1] + " " + av[2] + " :No topic is set\r\n";
    }

    std::string r_RPL_TOPIC(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 332 * * :No topic available\r\n";
        return ":" + av[0] + " 332 " + av[1] + " " + av[2] + " :" + av[3] + "\r\n";
    }

    std::string r_RPL_NAMREPLY(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 353 * * :No users in channel\r\n";
        return ":" + av[0] + " 353 " + av[1] + " " + av[2] + " :" + av[3] + "\r\n";
    }

    std::string r_RPL_ENDOFNAMES(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 366 * * :End of NAMES list\r\n";
        return ":" + av[0] + " 366 " + av[1] + " " + av[2] + " :End of NAMES list\r\n";
    }

    std::string r_ERR_UNKNOWNCOMMAND(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 421 * :Unknown command\r\n";
        return ":" + av[0] + " 421 " + av[1] + " :Unknown command\r\n";
    }
    
    std::string r_ERR_INVALIDNICK(const std::vector<std::string>& av) {
        if (av.size() < 2) return ": 432 * :No nickname given\r\n";
        return ":" + av[0] + " 432 " + av[1] + " :Invalid nickname (cannot contain spaces)\r\n";
    }
    
    std::string r_ERR_NICKNAMEINUSE(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 433 * :Nickname is already in use\r\n";
        return ":" + av[0] + " 433 " + av[1] + " :Nickname is already in use\r\n";
    }

    std::string r_ERR_NOTREGISTERED(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 451 * :You have not registered (PASS/NICK/USER required)\r\n";
        return ":" + av[0] + " 451 " + av[1] + " :You have not registered (PASS/NICK/USER required)\r\n";
    }
    
    std::string r_ERR_NEEDMOREPARAMS(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 461 * :Not enough parameters\r\n";
        return ":" + av[0] + " 461 " + av[1] + " :Not enough parameters\r\n";
    }

    std::string r_ERR_NOSUCHCHANNEL(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 403 * * :No such channel\r\n";
        return ":" + av[0] + " 403 " + av[1] + " " + av[2] + " :No such channel\r\n";
    }

    std::string r_ERR_CHANOPRIVSNEEDED(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 482 * * :You're not channel operator\r\n";
        return ":" + av[0] + " 482 " + av[1] + " " + av[2] + " :You're not channel operator\r\n";
    }

    std::string r_ERR_UMODEUNKNOWNFLAG(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 501 * :Unknown MODE flag\r\n";
        return ":" + av[0] + " 501 " + av[1] + " :Unknown MODE flag\r\n";
    }

    std::string r_ERR_USERSDONTMATCH(const std::vector<std::string> &av) {
        if (av.size() < 2) return ": 502 * :Cannot change mode for other users\r\n";
        return ":" + av[0] + " 502 " + av[1] + " :Cannot change mode for other users\r\n";
    }

    std::string r_ERR_UNKNOWNMODE(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 472 * * :is unknown mode char to me\r\n";
        return ":" + av[0] + " 472 " + av[1] + " " + av[2] + " :is unknown mode char to me\r\n";
    }
    std::string r_ERR_NORECIPIENT(const std::vector<std::string> &av) {
        return ":" + av[0] + " 411 " + av[1] + " :No recipient given (PRIVMSG)\r\n";
    }

    std::string r_ERR_NOTEXTTOSEND(const std::vector<std::string> &av) {
        return ":" + av[0] + " 412 " + av[1] + " :No text to send\r\n";
    }

    std::string r_ERR_CANNOTSENDTOCHAN(const std::vector<std::string> &av) {
        return ":" + av[0] + " 404 " + av[1] + " " + av[2] + " :Cannot send to channel\r\n";
    }

    std::string r_ERR_NOSUCHNICK(const std::vector<std::string> &av) {
        return ":" + av[0] + " 401 " + av[1] + " " + av[2] + " :No such nick/channel\r\n";
    }
    std::string r_RPL_WHOREPLY(const std::vector<std::string>& params) {
        return ":" + params[0] + " 352 " + params[1] + " " + params[2] + " " + params[3] + " " + 
            params[4] + " " + params[5] + " " + params[6] + " " + params[7] + " :" + params[8] + "\r\n";
    }

    std::string r_RPL_ENDOFWHO(const std::vector<std::string>& params) {
        return ":" + params[0] + " 315 " + params[1] + " " + params[2] + " :End of WHO list\r\n";
    }
    std::string r_ERR_USERNOTINCHANNEL(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 441 * * :They aren't on that channel\r\n";
        return ":" + av[0] + " 441 " + av[1] + " " + av[2] + " :They aren't on that channel\r\n";
    }

    std::string r_RPL_UMODEIS(const std::vector<std::string> &av) {
        if (av.size() < 3) return ": 221 * :No user modes set\r\n";
        return ":" + av[0] + " 221 " + av[1] + " " + av[2] + "\r\n";
    }
}