/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:53 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 14:49:21 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include "../server/Reply.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

void CommandHandler::handlePrivmsg(Server &srv, Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string target, msg;

    // Extraer target y mensaje
    iss >> target;
    std::getline(iss, msg);

    // Eliminar espacios en blanco iniciales y el ':' inicial del mensaje
    size_t colon_pos = msg.find(':');
    if (colon_pos != std::string::npos) {
        msg = msg.substr(colon_pos + 1);
    }
    msg = trim(msg);

    // Manejo de errores estándar IRC
    if (target.empty()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_NORECIPIENT(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    if (msg.empty()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_NOTEXTTOSEND(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Manejar múltiples targets
    std::vector<std::string> targets = split(target, ',');
    
    for (size_t i = 0; i < targets.size(); ++i) {
        std::string current_target = trim(targets[i]);
        
        if (current_target[0] == '#') {
            // Mensaje a canal
            Channel* channel = srv.getChannelManager().getChannelByName(current_target);
            if (channel && channel->isClientInChannel(client)) {
                std::string formatted_msg = ":" + client.getFullIdentifier() + " PRIVMSG " + current_target + " :" + msg + "\r\n";
                channel->broadcast(formatted_msg);
            } else {
                std::vector<std::string> params;
                params.push_back(srv.getServerName());
                params.push_back(client.getNickname());
                params.push_back(current_target);
                std::string error = Reply::r_ERR_CANNOTSENDTOCHAN(params);
                send(client.getFd(), error.c_str(), error.size(), 0);
            }
        } else {
            // Mensaje a usuario
            Client* recipient = srv.getClientManager().getClientByNickname(current_target);
            if (recipient) {
                std::string formatted_msg = ":" + client.getFullIdentifier() + " PRIVMSG " + current_target + " :" + msg + "\r\n";
                send(recipient->getFd(), formatted_msg.c_str(), formatted_msg.size(), 0);
            } else {
                std::vector<std::string> params;
                params.push_back(srv.getServerName());
                params.push_back(client.getNickname());
                params.push_back(current_target);
                std::string error = Reply::r_ERR_NOSUCHNICK(params);
                send(client.getFd(), error.c_str(), error.size(), 0);
            }
        }
    }
}
