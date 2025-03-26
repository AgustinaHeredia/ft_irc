/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHO.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: patri <patri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:26:06 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/26 16:00:57 by patri            ###   ########.fr       */
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

    std::cout << "[DEBUG] Comando WHO recibido: " << message << std::endl;

    //  Verificar autenticación del usuario
	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Falta completar la autenticación.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Cliente no autenticado intentó WHO " << std::endl;
        return;
    }

    //  WHO sin canal → Listar todos los usuarios del servidor
    if (channel_name.empty()) { 
        std::string response = "Usuarios conectados:\n";
        std::vector<Client*> clients = srv.getClientManager().getAllClients();
        for (size_t i = 0; i < clients.size(); ++i) {
            if (clients[i]->isConnected()) {
                response += clients[i]->getNickname() + "\n";
            }
        }
        send(client.getFd(), response.c_str(), response.size(), 0);
        return;
    }

    //  Buscar el canal
    if (channel_name[0] != '#') {
        const char* error_msg = "ERROR: Nombre de canal inválido.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Canal no encontrado.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Validar si el canal es privado y el usuario no es miembro
    if (channel->isInviteOnly() && !channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: No tienes permiso para ver la lista de este canal.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Obtener la lista de usuarios del canal
    std::string response = "Usuarios en " + channel_name + ":\n";
    std::vector<Client*> clientsInChannel = channel->getClients();
    for (size_t i = 0; i < clientsInChannel.size(); ++i) {
        if (clientsInChannel[i]->isConnected()) { 
            response += clientsInChannel[i]->getNickname();
            if (channel->isOperator(*clientsInChannel[i])) {
                response += " (Operador)";
            }
            response += "\n";
        }
    }

    send(client.getFd(), response.c_str(), response.size(), 0);
}
