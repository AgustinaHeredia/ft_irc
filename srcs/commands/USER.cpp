/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/31 19:37:27 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <iostream>
#include <sstream>

void CommandHandler::handleUserCommand(Server& srv, Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string username, hostname, servername, realname;

    iss >> username >> hostname >> servername;
    std::getline(iss >> std::ws, realname);

    if (!realname.empty() && realname[0] == ':') {
        realname = realname.substr(1);
    }

    if (username.empty() || realname.empty()) {
        std::vector<std::string> params;
        params.push_back("USER");
        srv.sendReply(Reply::ERR_NEEDMOREPARAMS, client, params);  // Usa server directamente
        return;
    }

    client.setUsername(username);
    client.setRealname(realname);
    client.setAuthState(Client::AUTH_COMPLETE);
}