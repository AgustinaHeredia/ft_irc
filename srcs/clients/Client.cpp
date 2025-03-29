/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:37 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 16:39:02 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

Client::Client(int fd, sockaddr_in addr) : 
    client_fd(fd), 
    client_addr(addr), 
    connected(true),
    authenticated(false) {
    
    // Obtener IP y hostname
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    hostname = client_ip; // Usamos la IP como hostname por defecto
    
    std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
}

Client::~Client() {
    if (connected) {
        disconnect();
    }
}

bool Client::isConnected() {
    if (client_fd != -1) {
        connected = true;
    }
    return connected; 
}

void Client::disconnect() {
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
    connected = false;
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

std::string Client::getRealname() const {
    return realname;
}

std::string Client::getHostname() const {
    return hostname;
}

bool Client::isAuthenticated() const {
    return authenticated;
}

void Client::authenticate() {
    if (!nickname.empty() && !username.empty()) {
        authenticated = true;
    }
}

void Client::markAsAuthenticated(bool auth) {
    authenticated = auth;
}

void Client::setNickname(const std::string &nick) {
    nickname = nick;
}

void Client::setUsername(const std::string &user) {
    username = user;
}

std::string &Client::getPartialCommand() { 
    return partialCommand;
}

void Client::setRealname(const std::string &name) {
    realname = name;
}