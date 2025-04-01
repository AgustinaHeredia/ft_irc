/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 00:05:34 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 16:35:44 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleTopic(Server &srv, Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string channel_name, topic;
    iss >> channel_name;
    std::getline(iss >> std::ws, topic); //  Captura el nuevo tema si existe

    std::cout << "[DEBUG] TOPIC command received: " << message << std::endl;
    std::cout << "[DEBUG] Channel: " << channel_name << ", New topic: " << topic << std::endl;

	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Authentication is missing.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Unauthenticated client attempted TOPIC " << std::endl;
        return;
    }

    //  Verificar si el canal es válido
    if (channel_name.empty() || channel_name[0] != '#') {
		const char* error_msg = "ERROR: Invalid channel name. Must begin with '#'.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Buscar el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    std::cout << "[DEBUG] Received KICK: " << message << std::endl;
	if (!channel) {
        const char* error_msg = "ERROR: Channel not found.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		return;
    }

    //  Si no se especifica un nuevo tema, devolver el tema actual
    if (topic.empty()) {
        std::string topic_msg = "TOPIC " + channel_name + channel->getTopic() + "\r\n";
        send(client.getFd(), topic_msg.c_str(), topic_msg.size(), 0);
        return;
    }

    //  Verificar si el cliente está en el canal
    if (!channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: You are not on this channel.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		return;
    }

    //  Verificar si el cliente es operador
    if (!channel->isOperator(client)) {
        const char* error_msg = "ERROR: You do not have permission to change the theme.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		return;
    }

    //  Cambiar el tema del canal
    channel->setTopic(topic);

    //  Notificar a todos en el canal sobre el cambio
    std::string topic_change_msg = ":" + client.getNickname() + " TOPIC " + channel_name + topic + "\r\n";
    channel->broadcast(topic_change_msg);
}
