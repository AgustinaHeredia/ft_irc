/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:27 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/08 10:06:37 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <sys/socket.h>

Channel::Channel(const std::string& name) : name(name) {}

void Channel::addClient(Client& client) {
    clients.push_back(&client);
	std::string welcome_msg = "Bienvenido al canal " + name + ", " + client.getNickname() + "!\n";
    send(client.getFd(), welcome_msg.c_str(), welcome_msg.length(), 0);
}

void Channel::removeClient(Client& client) {
    clients.erase(std::remove(clients.begin(), clients.end(), &client), clients.end());
	std::string goodbye_msg = client.getNickname() + " ha salido del canal " + name + ".\n";
    broadcast(goodbye_msg);
}

void Channel::broadcast(const std::string& message) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client* client = *it;
        send(client->getFd(), message.c_str(), message.size(), 0);
    }
}

std::string Channel::getName() const {
    return name;
}

bool Channel::isClientInChannel(const Client& client) const {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i] == &client) { // Comparación de punteros
            return true;
        }
    }
    return false;
}

bool Channel::isEmpty() const {
    return clients.empty();
}
