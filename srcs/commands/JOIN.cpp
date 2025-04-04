/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/04 12:46:57 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleJoin(Server &srv, Client &client, const std::string &message) {

    std::istringstream iss(message);
    std::string channel_name, key;
    iss >> channel_name;
    iss >> channel_name;
    iss >> key;

    // Verificar autenticación
    if (!client.isAuthenticated()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_NOTREGISTERED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Validar nombre del canal
    if (channel_name.empty() || channel_name[0] != '#') {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(channel_name);
        std::string error = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Buscar o crear el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);

    if (!channel) {
        // Crear nuevo canal y asignar operador
        srv.getChannelManager().addChannel(Channel(channel_name));
        channel = srv.getChannelManager().getChannelByName(channel_name);
        channel->addClient(client);
        channel->addOperator(client);
    } else {
        // Comprobar restricciones ANTES de añadir al usuario
        if (channel->isInviteOnly() && !channel->isUserInvited(client)) {
            std::vector<std::string> params;
            params.push_back(srv.getServerName());
            params.push_back(client.getNickname());
            params.push_back(channel_name);
            std::string error = Reply::r_ERR_INVITEONLYCHAN(params);
            send(client.getFd(), error.c_str(), error.size(), 0);
            return;
        }
        
        if (!channel->getKey().empty() && key != channel->getKey()) {
            std::vector<std::string> params;
            params.push_back(srv.getServerName());
            params.push_back(client.getNickname());
            params.push_back(channel_name);
            std::string error = Reply::r_ERR_BADCHANNELKEY(params);
            send(client.getFd(), error.c_str(), error.size(), 0);
            return;
        }

        if (channel->getUserLimit() > 0 && 
            channel->getClients().size() >= static_cast<size_t>(channel->getUserLimit())) {
            std::vector<std::string> params;
            params.push_back(srv.getServerName());
            params.push_back(client.getNickname());
            params.push_back(channel_name);
            std::string error = Reply::r_ERR_CHANNELISFULL(params);
            send(client.getFd(), error.c_str(), error.size(), 0);
            return;
        }

        // Añadir al usuario si pasa las comprobaciones
        if (channel->isClientInChannel(client)) {
            std::vector<std::string> params;
            params.push_back(srv.getServerName());
            params.push_back(client.getNickname());
            params.push_back(channel_name);
            std::string error = Reply::r_ERR_USERONCHANNEL(params);
            send(client.getFd(), error.c_str(), error.size(), 0);
            return;
        }
        channel->addClient(client);
    }

    // Notificar unión al canal
    std::string join_msg = ":" + client.getFullIdentifier() + " JOIN " + channel_name + "\r\n";
    channel->broadcast(join_msg);

    // Limpiar invitación si existía
    channel->removeInvitedUser(client);
}