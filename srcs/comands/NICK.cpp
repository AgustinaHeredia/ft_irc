/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:38:59 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/15 17:27:02 by agusheredia      ###   ########.fr       */
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
        const char* error_msg = "ERROR: Nickname inválido.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Verificar si el nickname ya está en uso por otro usuario
    Client* existingClient = srv.getClientManager().getClientByNickname(nick);
    if (existingClient && existingClient != &client) {
        const char* error_msg = "ERROR: Nickname en uso.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Si el usuario ya tenía un nickname, eliminarlo del registro
    if (!client.getNickname().empty()) {
        srv.getClientManager().removeNickname(client.getNickname());
    }

    //  Asignar el nuevo nickname
    client.setNickname(nick);
    srv.getClientManager().addNickname(nick, &client);
    client.authenticate();

    if (client.isAuthenticated()) {
        const char* success_msg = "Autenticación completa.\n";
        send(client.getFd(), success_msg, strlen(success_msg), 0);
    } else {
        const char* warning_msg = "Warning: Falta el USER para completar la autenticación.\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }

    std::cout << "Nickname cambiado a: " << nick << " para el cliente: " << client.getFd() << std::endl;
}
