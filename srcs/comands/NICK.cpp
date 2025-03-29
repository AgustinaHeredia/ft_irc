/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:38:59 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:21:10 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>

void CommandHandler::handleNickCommand(Server& srv, Client& client, const std::string &nick) {
    if (nick.empty()) {
        const char* error_msg = "ERROR: Invalid nickname.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Verificar si el nickname ya está en uso por otro usuario
    Client* existingClient = srv.getClientManager().getClientByNickname(nick);
    if (existingClient && existingClient != &client) {
        const char* error_msg = "ERROR: Nickname in use.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Si el usuario ya tenía un nickname, eliminarlo del registro
    if (!client.getNickname().empty()) {
        srv.getClientManager().removeNickname(client.getNickname());
    }

    // Asignar el nuevo nickname
    client.setNickname(nick);
    srv.getClientManager().addNickname(nick, &client);

    // Comprobar si ya está autenticado
    if (!client.isAuthenticated()) {
        const char* warning_msg = "Remember to send the USER command to complete your authentication.\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }
    std::cout << "Nickname changed to: " << nick << " for the client: " << client.getFd() << std::endl;
}
