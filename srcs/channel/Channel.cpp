/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:27 by agusheredia       #+#    #+#             */
/*   Updated: 2025/02/04 20:43:31 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>
#include <sys/socket.h>

void Channel::addClient(Client& client) {
    clients.push_back(&client);
}

void Channel::removeClient(Client& client) {
    clients.erase(std::remove(clients.begin(), clients.end(), &client), clients.end());
}

void Channel::broadcast(const std::string& message) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client* client = *it;
        send(client->getFd(), message.c_str(), message.size(), 0);
    }
}
