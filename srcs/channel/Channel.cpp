/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:27 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 20:57:51 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <sys/socket.h>

Channel::Channel(const std::string &name)
    : name(name), inviteOnly(false), topicRestricted(false), userLimit(0) {}

Channel::~Channel() {}

void Channel::addClient(Client& client) {
    clients.push_back(&client);
    std::string welcome_msg = "Bienvenido al canal " + name + ", " + client.getNickname() + "!\n";
    send(client.getFd(), welcome_msg.c_str(), welcome_msg.length(), 0);
}

void Channel::removeClient(Client &client) {
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

bool Channel::isClientInChannel(const Client &client) const {
    return std::find(clients.begin(), clients.end(), &client) != clients.end();
}

bool Channel::isEmpty() const {
    return clients.empty();
}

bool Channel::isOperator(const Client &client) const {
    return std::find(operators.begin(), operators.end(), &client) != operators.end();
}

void Channel::addOperator(Client& client) {
    if (!isOperator(client)) {
        operators.push_back(&client);
    }
}

void Channel::removeOperator(Client& client) {
    operators.erase(std::remove(operators.begin(), operators.end(), &client), operators.end());
}

void Channel::setInviteOnly(bool state) {
    inviteOnly = state;
}

bool Channel::isInviteOnly() const {
    return inviteOnly;
}

void Channel::setTopicRestricted(bool state) {
    topicRestricted = state;
}

bool Channel::isTopicRestricted() const {
    return topicRestricted;
}

void Channel::setKey(const std::string& newKey) {
    key = newKey;
}

void Channel::removeKey() {
    key.clear();
}

std::string Channel::getKey() const {
    return key;
}

void Channel::setUserLimit(int limit) {
    userLimit = limit;
}

void Channel::removeUserLimit() {
    userLimit = 0;
}

int Channel::getUserLimit() const {
    return userLimit;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

std::string Channel::getTopic() const {
    return topic.empty() ? "No hay un tema establecido." : topic;
}
