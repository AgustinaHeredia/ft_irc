/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:37 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 15:47:17 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>

Client::Client(int fd, sockaddr_in addr) : client_fd(fd), client_addr(addr), connected(true) {
    pthread_create(&client_thread, NULL, &Client::threadFunc, this);

	// Usar client_addr para mostrar la dirección del cliente
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    std::cout << "Cliente conectado desde " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
}

Client::~Client() {
    if (connected) {
        disconnect();
    }
}

void Client::start() {
    // Implementación de la función start
}

void Client::disconnect() {
    connected = false;
    pthread_join(client_thread, NULL);
}

void Client::handleMessages() {
    char buffer[1024];
    while (connected) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            disconnect();
            break;
        }
        std::string command(buffer);
        // Aquí se puede llamar a CommandHandler para manejar el comando
        // Por ejemplo:
        // commandHandler.handleCommand(*this, command);
	}
}

int Client::getFd() const {
    return client_fd;
}

std::string Client::getNickname() const {
    return nickname;
}

std::string Client::getUsername() const {
    return username;
}

bool Client::isAuthenticated() const {
    return authenticated;
}

void Client::authenticate() {
    authenticated = true;
}

void Client::setNickname(const std::string& nick) {
    nickname = nick;
}

void Client::setUsername(const std::string& user) {
    username = user;
}

void* Client::threadFunc(void* arg) {
    Client* client = static_cast<Client*>(arg);
    client->handleMessages();
    return NULL;
}
