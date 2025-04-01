/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:20:12 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 18:02:45 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManager.hpp"

void ClientManager::addClient(Client* client){
	clients.push_back(client);
}

void ClientManager::removeClient(Client* client){

    if (!client->getNickname().empty()) {
        nicknames.erase(client->getNickname());
    }
	client->disconnect();
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    delete client;
}

bool ClientManager::isNicknameInUse(const std::string &nick){
	return nicknames.find(nick) != nicknames.end();
}

void ClientManager::addNickname(const std::string &nick, Client* client){
	nicknames[nick] = client;
}

void ClientManager::removeNickname(const std::string &nick){
	if (!nick.empty()) {
        nicknames.erase(nick);
        std::cout << "[DEBUG] Nickname eliminado: " << nick << std::endl;
    }
}

Client* ClientManager::getClientByNickname(const std::string &nick){
	std::map<std::string, Client*>::const_iterator it = nicknames.find(nick);
    if (it != nicknames.end()) {
        return it->second;
    }
    return NULL;
}

Client* ClientManager::getClientByFd(int fd) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->getFd() == fd) {
            return *it;  // Devolvemos el puntero al cliente
        }
    }
    return NULL;
}

std::vector<Client*> ClientManager::getAllClients() const {
    return clients;
}

void ClientManager::clearAllClients() {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete *it;  // Liberar el cliente
    }
    clients.clear();
	nicknames.clear();
}

bool ClientManager::isClientValid(Client* client) const {
    return std::find(clients.begin(), clients.end(), client) != clients.end();
}
