/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: patri <patri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:07:11 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/26 13:07:10 by patri            ###   ########.fr       */
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
		throw std::runtime_error("Error al crear el socket");
	}

	// 2️ Configurar la opción SO_REUSEADDR para reutilizar la dirección del socket
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Error al configurar el socket");
    }

	// 3 Configurar la dirección del servidor
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 4 Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        throw std::runtime_error("Error al vincular el socket");
    }

    // 5 Poner el socket en modo escucha
    if (listen(server_fd, BACKLOG) == -1) {
        throw std::runtime_error("Error al poner el socket en modo escucha");
    }

    // 6 Configurar poll() para manejar múltiples conexiones
    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);

	std::cout << "Servidor iniciado en el puerto " << port << "." << std::endl;
}

void Server::process() {
    int activity = poll(fds.data(), fds.size(), -1);
    if (activity == -1) {
        if (errno == EINTR) {
            std::cerr << "Señal de interrupción recibida, cerrando servidor..." << std::endl;
        } else {
            std::cerr << "Error en poll(), cerrando servidor..." << std::endl;
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
                std::cout << "Cliente desconectado: " << fds[i].fd << std::endl;
                clientManager.removeClient(clientManager.getClientByFd(fds[i].fd));
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                i--;
                continue;
            }

            buffer[bytes_received] = '\0';  // Asegurar terminación de cadena
            std::cout << "Datos recibidos: " << buffer << std::endl;

            // Obtener el cliente
            Client* client = clientManager.getClientByFd(fds[i].fd);
            if (!client) {
                std::cerr << "Error: Cliente no encontrado para fd " << fds[i].fd << std::endl;
                continue;
            }

            //  Acumular datos recibidos en el cliente
            client->getPartialCommand() += buffer;
            std::cout << "Comando parcial acumulado: " << client->getPartialCommand() << std::endl;

            // Procesar solo líneas completas
            size_t pos;
            while ((pos = client->getPartialCommand().find("\n")) != std::string::npos) {
                std::string single_command = client->getPartialCommand().substr(0, pos);
                client->getPartialCommand().erase(0, pos + 2);  // Eliminar comando procesado

                // Manejar el comando
                std::cout << "Comando recibido: [" << single_command << "]" << std::endl;
                commandHandler.handleCommand(*client, single_command);
            }
        }

        // Cliente listo para escribir
        if (fds[i].revents & POLLOUT) {
            // Aquí podríamos manejar una cola de mensajes pendientes
        }

        // Manejar errores en el socket
        if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cout << "Error en cliente " << fds[i].fd << ", cerrando conexión." << std::endl;
            clientManager.removeClient(clientManager.getClientByFd(fds[i].fd));
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
            i--;
        }
    }
}

Server::~Server() {
	std::cout << "Llamando al destructor de Server..." << std::endl;
	stop();
}

void Server::stop() {
    std::cout << "Deteniendo el servidor..." << std::endl;

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
        std::cerr << "Error al aceptar la conexión: " << strerror(errno) << std::endl;
        return;
    }

    if (fds.size() >= MAX_CLIENTS)
    {
        std::cerr << "Conexión rechazada: demasiados clientes conectados." << std::endl;
        close(client_fd);
        return;
    }

    std::cout << "Nuevo cliente conectado desde "
              << inet_ntoa(client_addr.sin_addr) << ":"
              << ntohs(client_addr.sin_port) << std::endl;

    std::string password_prompt = "Ingrese la contraseña con el formato: PASS <password>\n";
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
            std::cerr << "Error al recibir la contraseña del cliente." << std::endl;
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

        std::cout << "Recibido: [" << received_input << "]" << std::endl;

        // Se exige que el comando empiece exactamente con "PASS "
        if (received_input.compare(0, 5, "PASS ") != 0)
        {
            std::stringstream ss;
            ss << "Formato incorrecto. Intentos restantes: " << (2 - attempts) << "\n";
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
            std::cout << "Cliente autenticado correctamente." << std::endl;
            std::string welcome_msg = "Bienvenido al servidor IRC!\n";
            send(client_fd, welcome_msg.c_str(), welcome_msg.size(), 0);
            break;
        }
        else
        {
            std::stringstream ss;
            ss << "Contraseña incorrecta. Intentos restantes: " << (2 - attempts) << "\n";
            std::string error_msg = ss.str();
            send(client_fd, error_msg.c_str(), error_msg.size(), 0);
            std::cerr << error_msg;
            attempts++;
            continue;
        }
    }

    if (attempts >= 3)  // Se agotaron los intentos
    {
        std::string error_msg = "ERROR: Contraseña incorrecta. Conexión cerrada.\n";
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
