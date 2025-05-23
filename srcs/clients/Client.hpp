/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:31 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/03 16:15:38 by pquintan         ###   ########.fr       */
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
public:
    enum AuthState {
        AUTH_NONE,
        AUTH_PASS_OK,
        AUTH_NICK_OK,
        AUTH_COMPLETE
    };

private:
    int client_fd;
    sockaddr_in client_addr;
    bool connected;
	std::string nickname;
    std::string username;
	std::string realname;
	std::string hostname;
    bool authenticated;
	std::string partialCommand;
    AuthState authState;
    int authAttempts;

    bool expectingPasswordContinuation;
    time_t lastDataTime;
    bool _zombie;
    bool invisible;

public:
    Client(int fd, sockaddr_in addr);
    ~Client();
    void disconnect();
	bool isConnected();
    void handleMessages();
    int getFd() const;
	std::string getNickname() const;
    std::string getUsername() const;
    bool isAuthenticated() const;
	void authenticate();
    void markAsAuthenticated(bool auth);

    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);

	std::string getRealname() const;
	void setRealname(const std::string &name);

	std::string &getPartialCommand();

    //nuevo para que el flujo pueda trabajar con varios clientes a la vez
    AuthState getAuthState() const;
    void setAuthState(AuthState state);
    int getAuthAttempts() const;
    void incrementAuthAttempt();
    void resetAuthAttempts();
    std::string getHostname() const;

    void setExpectingContinuation(bool state);
    bool isExpectingContinuation() const;
    void updateLastActivity();
    bool isBufferStale() const;
    bool isZombie() const;
    void setZombie(bool state);
    std::string getFullIdentifier() const;
    bool isInvisible() const;
    void setInvisible(bool state);

};

#endif
