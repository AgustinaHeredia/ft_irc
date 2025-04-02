/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:37 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 14:50:13 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>

Client::Client(int fd, sockaddr_in addr) : 
    client_fd(fd), 
    client_addr(addr),
    connected(true),
    authState(AUTH_NONE),
    authAttempts(0),
    expectingPasswordContinuation(false),
    lastDataTime(time(0)),
    _zombie(false) {

	// Usar client_addr para mostrar la dirección del cliente
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
	connected = true;
    std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
}

Client::~Client() {
    if (connected) {
        disconnect();
    }
}

bool Client::isConnected() {
	if ((client_fd != -1)) {
		connected = true;
	}
    return connected; 
}

void Client::disconnect() {
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

bool Client::isAuthenticated() const {
    return (authState == AUTH_COMPLETE);
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

std::string Client::getRealname() const {
    return realname;
}

// nuevos indicadores para el flujo

Client::AuthState Client::getAuthState() const {
    return (authState);
}
void Client::setAuthState(Client::AuthState state) {
    authState = state;
    authenticated = (state == AUTH_COMPLETE);
}
int Client::getAuthAttempts() const {
    return (authAttempts);
}
void Client::incrementAuthAttempt() {
    authAttempts++;
}
void Client::resetAuthAttempts() {
    authAttempts = 0;
}

void Client::setExpectingContinuation(bool state) {
    expectingPasswordContinuation = state;
    if (state) updateLastActivity();
}

bool Client::isExpectingContinuation() const {
    return expectingPasswordContinuation;
}

void Client::updateLastActivity() {
    lastDataTime = time(0);
}

bool Client::isBufferStale() const {
    return (time(0) - lastDataTime > 30); // 30 segundos de timeout
}

bool Client::isZombie() const {
    return _zombie;
}

void Client::setZombie(bool state) {
    _zombie = state;
    // Opcional: marcar como desconectado si es zombie
    if (state) {
        connected = false;
        partialCommand.clear();
    }
}

std::string Client::getHostname() const {
    return hostname;
}

std::string Client::getFullIdentifier() const {
    return getNickname() + "!" + getUsername() + "@" + getHostname();
}
