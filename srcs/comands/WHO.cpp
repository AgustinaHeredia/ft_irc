/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHO.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:26:06 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 21:30:10 by agusheredia      ###   ########.fr       */
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
    std::string command, channel_name;
    iss >> command >> channel_name;

    std::cout << "[DEBUG] Comando WHO recibido: " << message << std::endl;

    if (channel_name.empty()) { 
        // 🔹 Si no se especifica canal, listar todos los usuarios en el servidor
        std::string response = "Usuarios conectados:\n";
        std::vector<Client*> clients = srv.getClientManager().getAllClients();
        for (size_t i = 0; i < clients.size(); ++i) {
            response += clients[i]->getNickname() + "\n";
        }
        send(client.getFd(), response.c_str(), response.size(), 0);
        return;
    }

    //  Buscar el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Canal no encontrado.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Si el canal es privado (`+i`), solo los miembros pueden ver la lista
    if (channel->isInviteOnly() && !channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: No tienes permiso para ver la lista de este canal.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Obtener y enviar la lista de usuarios del canal
    std::string response = "Usuarios en " + channel_name + ":\n";
    std::vector<Client*> clients = channel->getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        response += clients[i]->getNickname();
        if (channel->isOperator(*clients[i])) {
            response += " (Operador)";
        }
        response += "\n";
    }

    send(client.getFd(), response.c_str(), response.size(), 0);
}
