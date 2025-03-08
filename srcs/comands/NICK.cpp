/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:38:59 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 00:01:06 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>

void CommandHandler::handleNickCommand(Server &srv, Client &client, const std::string &nick) {
    if (nick.empty()) {
        send(client.getFd(), "ERROR: Nickname inválido.\n", 25, 0);
        return;
    }

    if (server.getClientManager().isNicknameInUse(nick)) {
        send(client.getFd(), "ERROR: Nickname en uso.\n", 25, 0);
        return;
    }

    if (!client.getNickname().empty()) {
        srv.getClientManager().removeNickname(client.getNickname()); // Liberar el antiguo NICK
    }

    client.setNickname(nick);
    srv.getClientManager().addNickname(nick, &client);
	send(client.getFd(), "NICK registrado con éxito.\n", 28, 0);
    std::cout << "Nickname establecido a: " << nick << " para el cliente: " << client.getFd() << std::endl;
}
