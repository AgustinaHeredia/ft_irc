/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:14:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 12:50:28 by pquintan         ###   ########.fr       */
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
#include "../server/Reply.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleMode(Server &srv, Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string target, mode, param;
    iss >> target >> mode >> param;

    if (!client.isAuthenticated()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_NOTREGISTERED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Manejo de modos de usuario
    if (target[0] != '#' && target[0] != '&') {
        if (target != client.getNickname()) {
            std::vector<std::string> params;
            params.push_back(srv.getServerName());
            params.push_back(client.getNickname());
            std::string error = Reply::r_ERR_USERSDONTMATCH(params);
            send(client.getFd(), error.c_str(), error.size(), 0);
            return;
        }
        
        if (mode == "+i") {
            std::string response = ":" + srv.getServerName() + " MODE " + target + " +i\r\n";
            send(client.getFd(), response.c_str(), response.size(), 0);
        } else {
            std::vector<std::string> params;
            params.push_back(srv.getServerName());
            params.push_back(client.getNickname());
            std::string error = Reply::r_ERR_UMODEUNKNOWNFLAG(params);
            send(client.getFd(), error.c_str(), error.size(), 0);
        }
        return;
    }

    // Manejo de canales
    Channel* channel = srv.getChannelManager().getChannelByName(target);
    if (!channel) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(target);
        std::string error = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    if (!channel->isOperator(client)) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(target);
        std::string error = Reply::r_ERR_CHANOPRIVSNEEDED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    if (mode == "+o") {
        Client* target_client = srv.getClientManager().getClientByNickname(param);
        if (target_client && channel->isClientInChannel(*target_client)) {
            channel->addOperator(*target_client);
            channel->broadcast(":" + client.getNickname() + " MODE " + target + " +o " + param + "\r\n");
        }
    } else if (mode == "-o") {
        Client* target_client = srv.getClientManager().getClientByNickname(param);
        if (target_client && channel->isClientInChannel(*target_client)) {
            channel->removeOperator(*target_client);
            channel->broadcast(":" + client.getNickname() + " MODE " + target + " -o " + param + "\r\n");
        }
    } else if (mode == "+i") {
        channel->setInviteOnly(true);
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " +i\r\n");
    } else if (mode == "-i") {
        channel->setInviteOnly(false);
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " -i\r\n");
    } else if (mode == "+t") {
        channel->setTopicRestricted(true);
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " +t\r\n");
    } else if (mode == "-t") {
        channel->setTopicRestricted(false);
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " -t\r\n");
    } else if (mode == "+k") {
        channel->setKey(param);
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " +k " + param + "\r\n");
    } else if (mode == "-k") {
        channel->removeKey();
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " -k\r\n");
    } else if (mode == "+l") {
        channel->setUserLimit(atoi(param.c_str()));
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " +l " + param + "\r\n");
    } else if (mode == "-l") {
        channel->removeUserLimit();
        channel->broadcast(":" + client.getNickname() + " MODE " + target + " -l\r\n");
    } else {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(mode);
        std::string error = Reply::r_ERR_UNKNOWNMODE(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
    }
}