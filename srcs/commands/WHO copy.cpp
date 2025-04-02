/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHO.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:26:06 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 16:47:42 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include "../server/Reply.hpp"

void CommandHandler::handleWho(Server& srv, Client& client, const std::string& message) {
    std::istringstream iss(message);
    std::string channel_name;
    iss >> channel_name;

    std::cout << "[DEBUG] WHO command received: " << message << std::endl;

    // Verificar autenticación del usuario
    if (!client.isAuthenticated()) {
        std::vector<std::string> params;
        params.push_back(client.getNickname());
        std::string error_msg = Reply::r_ERR_NOTREGISTERED(params);
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        std::cout << "[DEBUG] Unauthenticated client attempted WHO " << std::endl;
        return;
    }

    // WHO sin canal → Listar todos los usuarios del servidor
    if (channel_name.empty()) {
        std::string response = "Connected users:\r\n";
        std::vector<Client*> clients = srv.getClientManager().getAllClients();
        for (size_t i = 0; i < clients.size(); ++i) {
            if (clients[i]->isConnected()) {
                // Formato esperado por Irssi
                response += clients[i]->getNickname() + " " +
                            clients[i]->getUsername() + " " +
                            clients[i]->getHostname() + " * * :" +
                            clients[i]->getRealname() + "\r\n";
            }
        }
        send(client.getFd(), response.c_str(), response.size(), 0);
        return;
    }

    // Buscar el canal
    if (channel_name[0] != '#') {
        std::vector<std::string> params;
        params.push_back(client.getNickname());
        params.push_back(channel_name);
        std::string error_msg = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }

    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        std::vector<std::string> params;
        params.push_back(client.getNickname());
        params.push_back(channel_name);
        std::string error_msg = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }

    // Validar si el canal es privado y el usuario no es miembro
    if (channel->isInviteOnly() && !channel->isClientInChannel(client)) {
        std::vector<std::string> params;
        params.push_back(client.getNickname());
        params.push_back(channel_name);
        std::string error_msg = Reply::r_ERR_CHANOPRIVSNEEDED(params);
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }

    // Obtener la lista de usuarios del canal
    std::string response = "Users in " + channel_name + ":\r\n";
    std::vector<Client*> clientsInChannel = channel->getClients();
    for (size_t i = 0; i < clientsInChannel.size(); ++i) {
        if (clientsInChannel[i]->isConnected()) {
            // Formato esperado por Irssi
            response += clientsInChannel[i]->getNickname() + " " + 
                        clientsInChannel[i]->getUsername() + " " + 
                        clientsInChannel[i]->getHostname() + " * " + 
                        channel->getName() + " :" + 
                        clientsInChannel[i]->getRealname();
            
            // Si el cliente es un operador, añadir (o cualquier otra información relevante)
            if (channel->isOperator(*clientsInChannel[i])) {
                response += " (Operator)";
            }
            response += "\r\n";
        }
    }

    send(client.getFd(), response.c_str(), response.size(), 0);
}
