/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 10:59:09 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:18:21 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleKick(Server &srv, Client &client, const std::string &message) {
    std::cout << "[DEBUG] Received KICK: " << message << std::endl;
    std::istringstream iss(message);
    std::string channel_name, target_nick, reason;

    iss >> channel_name >> target_nick;
    std::getline(iss >> std::ws, reason); // Captura el motivo si existe

    std::cout << "[DEBUG] KICK command received: " << message << std::endl;
    std::cout << "[DEBUG] Channel: " << channel_name << ", User: " << target_nick << ", Reason: " << reason << std::endl;

	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Authentication is missing.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Unauthenticated client attempted KICK " << std::endl;
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

    //  Verificar si el usuario que ejecuta `KICK` está en el canal
    if (!channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: You are not on this channel.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Verificar si el usuario tiene permisos para expulsar (es operador)
    if (!channel->isOperator(client)) {
        const char* error_msg = "ERROR: You do not have permission to ban users.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Buscar al usuario objetivo
    Client *target = srv.getClientManager().getClientByNickname(target_nick);
    if (!target || !channel->isClientInChannel(*target)) {
        const char* error_msg = "ERROR: The user is not on this channel.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Notificar a todos en el canal sobre la expulsión
    std::string kick_msg = ":" + client.getNickname() + " KICK " + channel_name + " " + target_nick + " :" + (reason.empty() ? "Expelled" : reason) + "\n";
    channel->broadcast(kick_msg);

    //  Expulsar al usuario del canal
    std::string target_msg = "You have been expelled from " + channel_name + ". Reason: " + reason + "\n";
    send(target->getFd(), target_msg.c_str(), target_msg.size(), 0);

    channel->removeClient(*target);
    
    std::cout << "[DEBUG] User " << target_nick << " has been expelled from " << channel_name << std::endl;
}
