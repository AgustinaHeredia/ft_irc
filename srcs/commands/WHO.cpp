/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHO.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:26:06 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/06 09:11:03 by pquintan         ###   ########.fr       */
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
        std::vector<Client*> clients = srv.getClientManager().getAllClients();
        for (size_t i = 0; i < clients.size(); ++i) {
            if (clients[i]->isConnected() && !clients[i]->isInvisible()) {
                // Formato estándar: :<servidor> 352 <usuario> * <username> <host> <servidor> <nick> H :0 <realname>
                std::string reply = ":" + srv.getServerName() + " 352 " + client.getNickname() + " * " 
                    + clients[i]->getUsername() + " " + clients[i]->getHostname() + " " 
                    + srv.getServerName() + " " + clients[i]->getNickname() + " H :0 " 
                    + clients[i]->getRealname() + "\r\n";
                send(client.getFd(), reply.c_str(), reply.size(), 0);
            }
        }
        // Fin de la lista
        std::string endReply = ":" + srv.getServerName() + " 315 " + client.getNickname() + " * :End of WHO list\r\n";
        send(client.getFd(), endReply.c_str(), endReply.size(), 0);
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
        // Añadir condición !clientsInChannel[i]->isInvisible()
        if (clientsInChannel[i]->isConnected() && !clientsInChannel[i]->isInvisible()) {
            response += clientsInChannel[i]->getNickname();
            if (channel->isOperator(*clientsInChannel[i])) {
                response += " (Operator)";
            }
            response += "\r\n";
        }
    }

    send(client.getFd(), response.c_str(), response.size(), 0);
}
