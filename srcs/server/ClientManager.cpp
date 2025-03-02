/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:20:12 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 19:38:18 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManager.hpp"

void ClientManager::addClient(Client* client){
	clients.push_back(client);
}

void ClientManager::removeClient(Client* client){
	client->disconnect();
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    delete client;
}

bool ClientManager::isNicknameInUse(const std::string& nick){
	return nicknames.find(nick) != nicknames.end();
}

void ClientManager::addNickname(const std::string& nick, Client* client){
	nicknames[nick] = client;
}

void ClientManager::removeNickname(const std::string& nick){
	nicknames.erase(nick);
}

Client* ClientManager::getClientByNickname(const std::string& nick){
	std::map<std::string, Client*>::const_iterator it = nicknames.find(nick);
    if (it != nicknames.end()) {
        return it->second;
    }
    return nullptr;
}

Client* ClientManager::getClientByFd(int fd) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->getFd() == fd) {
            return *it;  // Devolvemos el puntero al cliente
        }
    }
    return nullptr;
}
