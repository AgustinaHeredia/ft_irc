/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 18:54:48 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 20:25:36 by agusheredia      ###   ########.fr       */
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
        return "331 " + av[0] + " :No topic is set";
    }

    std::string r_RPL_TOPIC(const std::vector<std::string> &av) {
        if (av.size() < 2) return "332 * :No topic available";
        return "332 " + av[0] + " :" + av[1];
    }

    std::string r_RPL_NAMREPLY(const std::vector<std::string> &av) {
        if (av.size() < 2) return "353 * :No users in channel";
        return "353 " + av[0] + " :" + av[1];
    }

    std::string r_RPL_ENDOFNAMES(const std::vector<std::string> &av) {
        if (av.empty()) return "366 * :End of NAMES list";
        return "366 " + av[0] + " :End of NAMES list";
    }

    std::string r_ERR_UNKNOWNCOMMAND(const std::vector<std::string> &av) {
        if (av.empty()) return "421 * :Unknown command";
        return "421 " + av[0] + " :Unknown command";
    }

    std::string r_ERR_NEEDMOREPARAMS(const std::vector<std::string> &av) {
        if (av.empty()) return "461 * :Not enough parameters";
        return "461 " + av[0] + " :Not enough parameters";
    }

    std::string r_ERR_NICKNAMEINUSE(const std::vector<std::string> &av) {
        if (av.empty()) return "433 * :Nickname is already in use";
        return "433 " + av[0] + " :Nickname is already in use";
    }
}
