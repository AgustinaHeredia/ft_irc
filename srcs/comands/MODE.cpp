/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:14:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:20:07 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
MODE #canal +o usuario   # Dar operador
MODE #canal -o usuario   # Quitar operador
MODE #canal +i           # Hacer privado (solo por invitación)
MODE #canal -i           # Hacer público
MODE #canal +t           # Restringir cambio de tema a operadores
MODE #canal -t           # Permitir cambio de tema a todos
MODE #canal +k clave     # Establecer clave
MODE #canal -k           # Eliminar clave
MODE #canal +l número    # Limitar usuarios a 'número'
MODE #canal -l           # Eliminar límite de usuarios
*/

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleMode(Server &srv, Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string channel_name, mode, param;
    iss >> channel_name >> mode >> param;

    std::cout << "[DEBUG] MODE command received: " << message << std::endl;
    std::cout << "[DEBUG] Channel: " << channel_name << ", Mode: " << mode << ", Parameter: " << param << std::endl;

	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Authentication is missing.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Unauthenticated client attempted MODE " << std::endl;
        return;
    }

    //  Verificar si el canal es válido
    if (channel_name.empty() || channel_name[0] != '#') {
        const char* error_msg = "ERROR: Invalid channel name. Must begin with '#'.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		return;
    }

    //  Buscar el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Channel not found.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		return;
    }

    if (!channel->isOperator(client)) {
        const char* error_msg = "ERROR: You do not have permission to change channel modes.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    if (mode == "+o") {
        Client* target = srv.getClientManager().getClientByNickname(param);
        if (target && channel->isClientInChannel(*target)) {
            channel->addOperator(*target);
            channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " +o " + param + "\n");
        }
    } else if (mode == "-o") {
        Client* target = srv.getClientManager().getClientByNickname(param);
        if (target && channel->isClientInChannel(*target)) {
            channel->removeOperator(*target);
            channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " -o " + param + "\n");
        }
    } else if (mode == "+i") {
        channel->setInviteOnly(true);
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " +i\n");
    } else if (mode == "-i") {
        channel->setInviteOnly(false);
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " -i\n");
    } else if (mode == "+t") {
        channel->setTopicRestricted(true);
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " +t\n");
    } else if (mode == "-t") {
        channel->setTopicRestricted(false);
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " -t\n");
    } else if (mode == "+k") {
        channel->setKey(param);
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " +k\n");
    } else if (mode == "-k") {
        channel->removeKey();
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " -k\n");
    } else if (mode == "+l") {
        channel->setUserLimit(std::atoi(param.c_str()));
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " +l " + param + "\n");
    } else if (mode == "-l") {
        channel->removeUserLimit();
        channel->broadcast(":" + client.getNickname() + " MODE " + channel_name + " -l\n");
    } else {
        const char* error_msg = "ERROR: Mode not recognized.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
    }
}
