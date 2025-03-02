/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:31 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 12:04:12 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//representará a un usuario conectado

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include "../utils/utils.hpp"

class Client {
private:
    int client_fd;
    sockaddr_in client_addr;
    bool connected;
    pthread_t client_thread;
	std::string nickname;
    std::string username;
    bool authenticated;

    static void* threadFunc(void* arg);
public:
    Client(int fd, sockaddr_in addr);
    ~Client();
    void start();
    void disconnect();
    void handleMessages();
    int getFd() const;
	std::string getNickname() const;
    std::string getUsername() const;
    bool isAuthenticated() const;

    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);

};

#endif
