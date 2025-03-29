/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:07:11 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 15:55:32 by pquintan         ###   ########.fr       */
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
#include <algorithm>

#define BUFFER_SIZE 1024
#define BACKLOG 10

Server::Server(int puerto, const std::string &pwd) : port(puerto), password(pwd), commandHandler(*this)
{}

void Server::start() {
    // 1. Crear el socket del servidor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Error creating socket");
    }

    std::cout << "Server socket created." << std::endl;

    // 2. Configurar la opción SO_REUSEADDR para reutilizar la dirección del socket
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Error configuring socket");
    }

    std::cout << "Socket configured for reuse address." << std::endl;

    // 3. Configurar la dirección del servidor
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 4. Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        throw std::runtime_error("Error binding socket");
    }

    std::cout << "Socket bound to port " << port << "." << std::endl;

    // 5. Poner el socket en modo escucha
    if (listen(server_fd, BACKLOG) == -1) {
        throw std::runtime_error("Error putting socket in listening mode");
    }

    std::cout << "Server listening on port " << port << "." << std::endl;

    // 6. Configurar poll() para manejar múltiples conexiones
    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);

    std::cout << "Server started on port " << port << "." << std::endl;
}

void Server::process() {
    int activity = poll(&fds[0], fds.size(), -1);
    if (activity == -1) {
        if (errno == EINTR) {
            std::cerr << "Interrupt signal received, closing server..." << std::endl;
        } else {
            std::cerr << "Error in poll(), closing server..." << std::endl;
        }
        return;
    }

    // 1. Verificar si hay una nueva conexión entrante
    if (fds[0].revents & POLLIN) {
        std::cout << "New connection attempt detected." << std::endl;
        acceptClients();
    }

    // 2. Recorrer los clientes y manejar eventos
    for (size_t i = 1; i < fds.size(); i++) {
        if (fds[i].revents & POLLIN) {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

            // Cliente se desconectó
            if (bytes_received <= 0) {
                std::cout << "Client disconnected: " << fds[i].fd << std::endl;
                Client* client = clientManager.getClientByFd(fds[i].fd);
                if (client) {
                    clientManager.removeNickname(client->getNickname());
                }
                clientManager.removeClient(client);
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                std::cout << "Client fd " << fds[i].fd << " removed." << std::endl;
                i--;
                continue;
            }

            buffer[bytes_received] = '\0';
            std::cout << "Data received: " << buffer << std::endl;

            // Obtener el cliente y acumular datos
            Client* client = clientManager.getClientByFd(fds[i].fd);
            if (!client) {
                std::cerr << "Error: Client not found for fd " << fds[i].fd << std::endl;
                continue;
            }
            client->getPartialCommand() += buffer;
            std::cout << "Partial cumulative command: " << client->getPartialCommand() << std::endl;

            // Procesar líneas completas
            size_t pos;
            while ((pos = client->getPartialCommand().find("\n")) != std::string::npos) {
                std::string single_command = client->getPartialCommand().substr(0, pos);
                client->getPartialCommand().erase(0, pos + 1);

                // Limpiar espacios y saltos de línea al inicio y final
                single_command.erase(0, single_command.find_first_not_of(" \t\r\n"));
                single_command.erase(single_command.find_last_not_of(" \t\r\n") + 1);

                if (!single_command.empty()) {
                    std::cout << "Command received: [" << single_command << "]" << std::endl;
                    commandHandler.handleCommand(*client, single_command);
                }
            }
        }

        // Cliente listo para escribir (por ahora no se implementa)
        if (fds[i].revents & POLLOUT) {
            // Manejar cola de mensajes pendientes si es necesario
        }

        // Manejar errores en el socket
        if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cout << "Client error " << fds[i].fd << ", closing connection." << std::endl;
            clientManager.removeClient(clientManager.getClientByFd(fds[i].fd));
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
            std::cout << "Client fd " << fds[i].fd << " closed and removed." << std::endl;
            i--;
        }
    }
}

Server::~Server() {
    std::cout << "Calling the Server destructor..." << std::endl;
    stop();
}

void Server::stop() {
    std::cout << "Stopping the server..." << std::endl;
    clientManager.clearAllClients();
    
    close(server_fd);
    std::cout << "Server socket closed." << std::endl;

    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);
        std::cout << "Closed client socket " << fds[i].fd << std::endl;
    }
    fds.clear();
}

void Server::acceptClients()
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd == -1) {
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        return;
    }

    std::cout << "Accepted new client connection from "
              << inet_ntoa(client_addr.sin_addr) << ":"
              << ntohs(client_addr.sin_port) << std::endl;

    if (fds.size() >= MAX_CLIENTS) {
        std::cerr << "Connection refused: too many clients connected." << std::endl;
        close(client_fd);
        return;
    }

    std::string password_prompt = "Enter the password in the format: PASS <password>\n";
    send(client_fd, password_prompt.c_str(), password_prompt.size(), 0);

    int attempts = 0;
    std::string received_input;
    char buffer[256];
    bool password_ok = false;

    while (attempts < 3) { // Permitir 3 intentos
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            std::cerr << "Error receiving password from client." << std::endl;
            close(client_fd);
            return;
        }

        buffer[bytes_received] = '\0';
        received_input = buffer;

        // Procesar línea por línea
        std::stringstream ss_input(received_input);
        std::string line;
        while (std::getline(ss_input, line)) {
            // Limpiar saltos de línea y espacios
            while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
                line.erase(line.size() - 1);
            while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
                line.erase(0, 1);
            if (line.empty())
                continue;

            std::cout << "Received: [" << line << "]" << std::endl;

            // Manejar CAP LS para Irssi
            if (line.compare(0, 6, "CAP LS") == 0) {
                std::string cap_response = "CAP * LS :\r\n";
                send(client_fd, cap_response.c_str(), cap_response.size(), 0);
                std::cout << "Responding to CAP LS" << std::endl;
                continue;
            }

            // Verificar que la línea comience con "PASS "
            if (line.compare(0, 5, "PASS ") != 0) {
                std::stringstream ss_error;
                ss_error << "Incorrect format. Remaining attempts: " << (2 - attempts) << "\n";
                std::string error_msg = ss_error.str();
                send(client_fd, error_msg.c_str(), error_msg.size(), 0);
                std::cerr << error_msg;
                attempts++;
                continue;
            }

            // Extraer la contraseña enviada
            std::string input_password = line.substr(5);
            if (input_password == password) {
                std::cout << "Password accepted." << std::endl;
                std::string success_msg = "Password accepted. Continue with NICK and then USER.\n";
                send(client_fd, success_msg.c_str(), success_msg.size(), 0);
                password_ok = true;
                break;
            } else {
                std::stringstream ss;
                ss << "Incorrect password. Attempts remaining: " << (2 - attempts) << "\n";
                std::string error_msg = ss.str();
                send(client_fd, error_msg.c_str(), error_msg.size(), 0);
                std::cerr << error_msg;
                attempts++;
                continue;
            }
        }
        if (password_ok)
            break;
    }

    if (!password_ok) {
        std::string error_msg = "ERROR: Incorrect password. Connection closed.\n";
        send(client_fd, error_msg.c_str(), error_msg.size(), 0);
        std::cerr << error_msg;
        close(client_fd);
        return;
    }

    // Limpiar input residual
    while (recv(client_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT) > 0)
        ;

    // Crear y almacenar el nuevo cliente, marcándolo como autenticado
    Client* new_client = new Client(client_fd, client_addr);
    new_client->markAsAuthenticated(true);
    pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN;
    fds.push_back(client_pollfd);
    clientManager.addClient(new_client);

    std::cout << "New client authenticated and added to the server." << std::endl;
}

ClientManager &Server::getClientManager() {
    return clientManager;
}

ChannelManager &Server::getChannelManager() {
    return channelManager;
}
