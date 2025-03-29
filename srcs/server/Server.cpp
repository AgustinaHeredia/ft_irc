/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:07:11 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 12:33:15 by pquintan         ###   ########.fr       */
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
	// 1️ Crear el socket del servidor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		throw std::runtime_error("Error creating socket");
	}

	// 2️ Configurar la opción SO_REUSEADDR para reutilizar la dirección del socket
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Error configuring socket");
    }

	// 3 Configurar la dirección del servidor
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 4 Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        throw std::runtime_error("Error binding socket");
    }

    // 5 Poner el socket en modo escucha
    if (listen(server_fd, BACKLOG) == -1) {
        throw std::runtime_error("Error putting socket in listening mode");
    }

    // 6 Configurar poll() para manejar múltiples conexiones
    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);

	std::cout << "Server started on port " << port << "." << std::endl;
}

void Server::process() {
    int activity = poll(fds.data(), fds.size(), -1);
    if (activity == -1) {
        if (errno == EINTR) {
            std::cerr << "Interrupt signal received, closing server..." << std::endl;
        } else {
            std::cerr << "Error in poll(), closing server..." << std::endl;
        }
        return;
    }

    // 1️ Verificar si hay una nueva conexión entrante
    if (fds[0].revents & POLLIN) {
        acceptClients();
    }

    // 2️ Recorrer los clientes y manejar eventos
    for (size_t i = 1; i < fds.size(); i++) {
        // Cliente envió datos (leer)
        if (fds[i].revents & POLLIN) {
            char buffer[1024] = {0};
            int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

            // Cliente se desconectó
            if (bytes_received <= 0) {
                std::cout << "Client disconnected: " << fds[i].fd << std::endl;
                clientManager.removeNickname(clientManager.getClientByFd(fds[i].fd)->getNickname());
                clientManager.removeClient(clientManager.getClientByFd(fds[i].fd));
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                i--;
                continue;
            }

            buffer[bytes_received] = '\0';  // Asegurar terminación de cadena
            std::cout << "Data received: " << buffer << std::endl;

            // Obtener el cliente
            Client* client = clientManager.getClientByFd(fds[i].fd);
            if (!client) {
                std::cerr << "Error: Client not found for fd " << fds[i].fd << std::endl;
                continue;
            }
            // Acumular datos recibidos en el cliente
            client->getPartialCommand() += buffer;
            std::cout << "Partial cumulative command: " << client->getPartialCommand() << std::endl;

            // Procesar solo líneas completas
            size_t pos;
            while ((pos = client->getPartialCommand().find("\n")) != std::string::npos) {
                std::string single_command = client->getPartialCommand().substr(0, pos);
                client->getPartialCommand().erase(0, pos + 1);  // Eliminar comando procesado

                // Limpiar espacios y saltos de línea al principio y al final del comando
                single_command.erase(0, single_command.find_first_not_of(" \t\r\n"));
                single_command.erase(single_command.find_last_not_of(" \t\r\n") + 1);

                if (!single_command.empty()) {
                    // Manejar el comando
                    std::cout << "Command received: [" << single_command << "]" << std::endl;
                    commandHandler.handleCommand(*client, single_command);
                }
            }
        }

        // Cliente listo para escribir
        if (fds[i].revents & POLLOUT) {
            // Aquí podríamos manejar una cola de mensajes pendientes
        }

        // Manejar errores en el socket
        if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cout << "Client error " << fds[i].fd << ", closing connection." << std::endl;
            clientManager.removeClient(clientManager.getClientByFd(fds[i].fd));
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
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
    
    close(server_fd);  // Cerrar el descriptor del servidor
    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);  // Cerrar los descriptores de cliente
    }
    fds.clear();  // Limpiar el vector de fds
}

void Server::acceptClients()
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd == -1)
    {
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        return;
    }

    if (fds.size() >= MAX_CLIENTS)
    {
        std::cerr << "Connection refused: too many clients connected." << std::endl;
        close(client_fd);
        return;
    }

    std::cout << "New customer connected from "
              << inet_ntoa(client_addr.sin_addr) << ":"
              << ntohs(client_addr.sin_port) << std::endl;

    std::string password_prompt = "Enter the password in the format: PASS <password>\n";
    send(client_fd, password_prompt.c_str(), password_prompt.size(), 0);

    int attempts = 0;
    std::string received_input;
    char buffer[256];

    while (attempts < 3)  // Permitir exactamente 3 intentos
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
        {
            std::cerr << "Error receiving password from client." << std::endl;
            close(client_fd);
            return;
        }

        buffer[bytes_received] = '\0'; // Asegurar terminación de cadena
        received_input = buffer;

        // Limpiar correctamente '\r' y '\n'
        size_t end = received_input.find_last_not_of("\r\n");
        if (end != std::string::npos)
            received_input = received_input.substr(0, end + 1);
        else
            received_input = "";

        // Si la línea está vacía, ignorarla sin contar como intento
        if (received_input.empty())
        {
            continue;
        }

        std::cout << "Received: [" << received_input << "]" << std::endl;

        // Se exige que el comando empiece exactamente con "PASS "
        if (received_input.compare(0, 5, "PASS ") != 0)
        {
            std::stringstream ss;
            ss << "Incorrect format. Remaining attempts: " << (2 - attempts) << "\n";
            std::string error_msg = ss.str();
            send(client_fd, error_msg.c_str(), error_msg.size(), 0);
            std::cerr << error_msg;
            attempts++;
            continue;
        }

        // Extraer la contraseña enviada
        std::string input_password = received_input.substr(5); // Quitar "PASS "

        // Comparar la contraseña exactamente, sin cambios de mayúsculas/minúsculas
        if (input_password == password)
        {
            std::cout << "Password accepted." << std::endl;
            std::string success_msg = "Password accepted. Continue with NICK and then USER.\n";
            send(client_fd, success_msg.c_str(), success_msg.size(), 0);
            break;
        }
        else
        {
            std::stringstream ss;
            ss << "Incorrect password. Attempts remaining.: " << (2 - attempts) << "\n";
            std::string error_msg = ss.str();
            send(client_fd, error_msg.c_str(), error_msg.size(), 0);
            std::cerr << error_msg;
            attempts++;
            continue;
        }
    }

    if (attempts >= 3)  // Se agotaron los intentos
    {
        std::string error_msg = "ERROR: Incorrect password. Connection closed..\n";
        send(client_fd, error_msg.c_str(), error_msg.size(), 0);
        std::cerr << error_msg;
        close(client_fd);
        return;
    }

    // Limpiar cualquier input residual en el buffer
    while (recv(client_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT) > 0)
        ;

    // Agregar cliente a poll()
    pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN;
    fds.push_back(client_pollfd);

    // Crear y almacenar el nuevo cliente
    Client* new_client = new Client(client_fd, client_addr);
    clientManager.addClient(new_client);
}

ClientManager &Server::getClientManager() {
    return clientManager;
}

ChannelManager &Server::getChannelManager() {
    return channelManager;
}
