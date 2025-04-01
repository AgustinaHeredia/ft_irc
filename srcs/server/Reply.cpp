/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 18:54:48 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 14:33:17 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Reply.hpp"

namespace Reply {
    std::string r_RPL_WELCOME(const std::vector<std::string> &av) {
        if (av.empty()) return "001 * :Welcome to the Internet Relay Network";
        return "001 " + av[0] + " :Welcome to the Internet Relay Network";
    }

    std::string r_RPL_NOTOPIC(const std::vector<std::string> &av) {
        if (av.empty()) return "331 * :No topic is set";
        return av[0] + " :No topic is set";
    }

    std::string r_RPL_TOPIC(const std::vector<std::string> &av) {
        if (av.size() < 2) return "332 * :No topic available";
        return av[0] + " :" + av[1];
    }

    std::string r_RPL_NAMREPLY(const std::vector<std::string> &av) {
        if (av.size() < 2) return "353 * :No users in channel";
        return av[0] + " :" + av[1];
    }

    std::string r_RPL_ENDOFNAMES(const std::vector<std::string> &av) {
        if (av.empty()) return "366 * :End of NAMES list";
        return av[0] + " :End of NAMES list";
    }

    std::string r_ERR_UNKNOWNCOMMAND(const std::vector<std::string> &av) {
        if (av.empty()) return "421 * :Unknown command";
        return av[0] + " :Unknown command";
    }
    
    std::string r_ERR_INVALIDNICK(const std::vector<std::string>& av) {
        if (av.empty()) return "432 * :No nickname given";
        return av[0] + " :Invalid nickname (cannot contain spaces)";
    }
    
    std::string r_ERR_NICKNAMEINUSE(const std::vector<std::string> &av) {
        if (av.empty()) return "433 * :Nickname is already in use";
        return av[0] + " :Nickname is already in use";
    }

    std::string r_ERR_NOTREGISTERED(const std::vector<std::string> &av) {
        if (av.empty()) return "451 * :You have not registered (PASS/NICK/USER required)";
        return av[0] + " :You have not registered (PASS/NICK/USER required)";
    }
    
    std::string r_ERR_NEEDMOREPARAMS(const std::vector<std::string> &av) {
        if (av.empty()) return "461 * :Not enough parameters";
        return av[0] + " :Not enough parameters";
    }
}
