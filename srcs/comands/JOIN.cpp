/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 19:44:39 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/socket.h>

void CommandHandler::handleJoin(Server& srv, Client& client, const std::string& message) {
    std::istringstream iss(message);
    std::string command, channel_name;
    iss >> command >> channel_name;

    if (channel_name.empty()) {
        send(client.getFd(), "ERROR: Nombre de canal inválido.\n", 35, 0);
        return;
    }

    //  Verificar si el canal ya existe
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);

    //  Si el canal no existe, crearlo
    if (!channel) {
        srv.getChannelManager().addChannel(Channel(channel_name));
        channel = srv.getChannelManager().getChannelByName(channel_name);
    }

    //  Agregar al cliente al canal
    channel->addClient(client);
    std::string join_msg = client.getNickname() + " se ha unido al canal " + channel_name + "\n";
    send(client.getFd(), join_msg.c_str(), join_msg.size(), 0);
}
