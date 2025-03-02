/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:07:11 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 13:03:15 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 10

Server::Server() : commandHandler(*this) {
    // 1️ Crear el socket del servidor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Error al crear el socket");
    }

    // 2️ Configurar la dirección del servidor
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3️ Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        throw std::runtime_error("Error al vincular el socket");
    }

    // 4️ Poner el socket en modo escucha
    if (listen(server_fd, BACKLOG) == -1) {
        throw std::runtime_error("Error al poner el socket en modo escucha");
    }

    // 5️ Configurar poll() para manejar múltiples conexiones
    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);
}

Server::~Server() {
	stop();
}

void Server::start() {
    std::cout << "Servidor iniciado en el puerto " << PORT << "." << std::endl;

    while (true) {
        int activity = poll(fds.data(), fds.size(), -1);
        if (activity == -1) {
            std::cerr << "Error en poll(), cerrando servidor..." << std::endl;
            break;
        }

        // 1️ Verificar si hay una nueva conexión entrante
        if (fds[0].revents & POLLIN) {
            acceptClients();
        }

        // 2️ Recorrer los clientes y manejar eventos
        for (size_t i = 1; i < fds.size(); i++) {
            //  Cliente envió datos (leer)
            if (fds[i].revents & POLLIN) {
                char buffer[1024] = {0};
                int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer), 0);

                //  Cliente se desconectó
                if (bytes_received <= 0) {
                    std::cout << "Cliente desconectado: " << fds[i].fd << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                    continue;
                }

                //  Manejar el comando del cliente
                for (size_t j = 0; j < clients.size(); j++) {
                    if (clients[j]->getFd() == fds[i].fd) {
                        handleCommand(*clients[j], std::string(buffer));
                        break;
                    }
                }
            }

            //  Cliente listo para escribir
            if (fds[i].revents & POLLOUT) {
                // Aquí podríamos manejar una cola de mensajes pendientes
            }

            //  Manejar errores en el socket
            if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                std::cout << "Error en cliente " << fds[i].fd << ", cerrando conexión." << std::endl;
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                i--;
            }
        }
    }
}

void Server::acceptClients() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd == -1) {
        std::cerr << "Error al aceptar la conexión" << std::endl;
        return;
    }

    std::cout << "Nuevo cliente conectado desde " 
              << inet_ntoa(client_addr.sin_addr) << ":" 
              << ntohs(client_addr.sin_port) << std::endl;

    //  Agregar cliente a `poll()`
    pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN | POLLOUT; // Leer y escribir
    fds.push_back(client_pollfd);

    //  Crear y almacenar el nuevo cliente
    Client* new_client = new Client(client_fd, client_addr);
    clients.push_back(new_client);

    //  Enviar mensaje de bienvenida
    const char* welcome_msg = "Bienvenido al servidor IRC!\n";
    send(client_fd, welcome_msg, strlen(welcome_msg), 0);
}

void Server::stop() {
    std::cout << "Deteniendo el servidor..." << std::endl;
    close(server_fd);
    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);
    }
    fds.clear();
}

void Server::handleCommand(Client& client, const std::string& command) {
    commandHandler.handleCommand(client, command);
}

void Server::removeClient(Client* client) {
    client->disconnect();
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    delete client;
}

bool Server::isNicknameInUse(const std::string& nick) const {
    return nicknames.find(nick) != nicknames.end();
}

void Server::removeNickname(const std::string& nick) {
    nicknames.erase(nick);
}

void Server::addNickname(const std::string& nick, Client* client) {
    nicknames[nick] = client;
}
