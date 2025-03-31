/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:38:59 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/31 19:23:21 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>

void CommandHandler::handleNickCommand(Server& srv, Client& client, const std::string &nick) {
    // Eliminar CR/LF y espacios
    std::string clean_nick = nick.substr(0, nick.find_first_of(" \r\n"));

    if (clean_nick.empty()) {
        std::vector<std::string> params;
        params.push_back("NICK");
        srv.sendReply(Reply::ERR_NEEDMOREPARAMS, client, params);
        return;
    }

    // Verificar si el nickname ya está en uso
    Client* existing = srv.getClientManager().getClientByNickname(clean_nick);
    if (existing && existing != &client) {
        std::vector<std::string> params;
        params.push_back(clean_nick);
        srv.sendReply(Reply::ERR_NICKNAMEINUSE, client, params);
        return;
    }

    // Actualizar el nickname
    if (!client.getNickname().empty()) {
        srv.getClientManager().removeNickname(client.getNickname());
    }
    client.setNickname(clean_nick);
    srv.getClientManager().addNickname(clean_nick, &client);
}