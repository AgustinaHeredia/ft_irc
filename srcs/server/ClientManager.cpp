/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:20:12 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 19:50:14 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManager.hpp"

void ClientManager::addClient(Client* client){
	clients.push_back(client);
}

void ClientManager::removeClient(Client* client) {
    if (!client) return;

    if (!client->getNickname().empty()) {
        nicknames.erase(client->getNickname());
    }

    std::vector<Client*>::iterator it = std::find(clients.begin(), clients.end(), client);
    if (it != clients.end()) {
        clients.erase(it);
        delete client;
    }
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

void ClientManager::queueForRemoval(Client* client) {
    if (!client) return;

    // Verificar si el cliente ya está en la cola
    std::vector<Client*>::iterator it = std::find(removalQueue.begin(), removalQueue.end(), client);
    if (it == removalQueue.end()) {
        removalQueue.push_back(client);
        
        // Marcar como zombie inmediatamente
        client->setZombie(true);
    }
}

void ClientManager::processRemovals() {
    for (std::vector<Client*>::iterator it = removalQueue.begin(); it != removalQueue.end(); ) {
        Client* client = *it;
        if (client) {
            // Verificar si ya fue eliminado
            if (std::find(clients.begin(), clients.end(), client) != clients.end()) {
                if (!client->getNickname().empty()) {
                    nicknames.erase(client->getNickname());
                }
                clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
                delete client;
            }
        }
        it = removalQueue.erase(it);
    }
}
