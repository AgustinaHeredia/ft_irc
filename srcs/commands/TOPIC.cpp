/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 00:05:34 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/04 12:47:44 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>


// Funciones auxiliares
void sendCurrentTopic(Client &client, Channel &channel, const std::string &serverName) {
    if (channel.getTopic().empty()) {
        std::string msg = ":" + serverName + " 331 " + client.getNickname() + " " + channel.getName() + " :No topic is set\r\n";
        send(client.getFd(), msg.c_str(), msg.size(), 0);
    } else {
        std::string topic_msg = ":" + serverName + " 332 " + client.getNickname() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        send(client.getFd(), topic_msg.c_str(), topic_msg.size(), 0);
        
        std::ostringstream timestamp;
        timestamp << channel.getTopicTime();
        std::string who_msg = ":" + serverName + " 333 " + client.getNickname() + " " + channel.getName() + " " + channel.getTopicSetter() + " " + timestamp.str() + "\r\n";
        send(client.getFd(), who_msg.c_str(), who_msg.size(), 0);
    }
}

void broadcastTopicChange(Client &client, Channel &channel, const std::string &topic) {
    std::string msg = ":" + client.getFullIdentifier() + " TOPIC " + channel.getName() + " :" + topic + "\r\n";
    channel.broadcast(msg);
    channel.setTopicTime(time(NULL));
    channel.setTopicSetter(client.getNickname());
}

void CommandHandler::handleTopic(Server &srv, Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string channel_name, topic;
    iss >> channel_name;

    // 1. Extraer tema 
    std::getline(iss >> std::ws, topic);
    size_t colon_pos = topic.find(':');
    if (colon_pos != std::string::npos) {
        topic = topic.substr(colon_pos + 1);
    }
    topic.erase(0, topic.find_first_not_of(" \t"));
    topic.erase(topic.find_last_not_of(" \t") + 1);

    // 2. Verificación de autenticación
    if (!client.isAuthenticated()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_NOTREGISTERED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // 3. Validación del nombre del canal
    if (channel_name.empty() || channel_name[0] != '#') {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(channel_name);
        std::string error = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // 4. Obtener canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(channel_name);
        std::string error = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // 5. Comando sin tema: enviar tema actual
    if (topic.empty()) {
        sendCurrentTopic(client, *channel, srv.getServerName());
        return;
    }

    // 6. Verificar membresía en el canal
    if (!channel->isClientInChannel(client)) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(channel_name);
        std::string error = Reply::r_ERR_NOTONCHANNEL(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // 7. Validar permisos (modo +t y operador)
    if (channel->isTopicRestricted() && !channel->isOperator(client)) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(channel_name);
        std::string error = Reply::r_ERR_CHANOPRIVSNEEDED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // 8. Actualizar y notificar
    channel->setTopic(topic);
    broadcastTopicChange(client, *channel, topic);
}