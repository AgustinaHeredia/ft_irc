/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHO.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:26:06 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 16:36:09 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleWho(Server& srv, Client& client, const std::string& message) {
    std::istringstream iss(message);
    std::string channel_name;
    iss >> channel_name;

    std::cout << "[DEBUG] WHO command received: " << message << std::endl;

    //  Verificar autenticación del usuario
	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Authentication is missing.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Unauthenticated client attempted WHO " << std::endl;
        return;
    }

    //  WHO sin canal → Listar todos los usuarios del servidor
    if (channel_name.empty()) { 
        std::string response = "Connected users:\r\n";
        std::vector<Client*> clients = srv.getClientManager().getAllClients();
        for (size_t i = 0; i < clients.size(); ++i) {
            if (clients[i]->isConnected()) {
                response += clients[i]->getNickname() + "\r\n";
            }
        }
        send(client.getFd(), response.c_str(), response.size(), 0);
        return;
    }

    //  Buscar el canal
    if (channel_name[0] != '#') {
        const char* error_msg = "ERROR: Invalid channel name.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Channel not found.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Validar si el canal es privado y el usuario no es miembro
    if (channel->isInviteOnly() && !channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: You do not have permission to view this channel's list..\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Obtener la lista de usuarios del canal
    std::string response = "Users in " + channel_name + ":\r\n";
    std::vector<Client*> clientsInChannel = channel->getClients();
    for (size_t i = 0; i < clientsInChannel.size(); ++i) {
        if (clientsInChannel[i]->isConnected()) { 
            response += clientsInChannel[i]->getNickname();
            if (channel->isOperator(*clientsInChannel[i])) {
                response += " (Operator)";
            }
            response += "\r\n";
        }
    }

    send(client.getFd(), response.c_str(), response.size(), 0);
}
