/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 19:14:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/04 11:58:11 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
MODE #new +k 22
MODE #canal +o usuario   # Dar operador
MODE #canal -o usuario   # Quitarroperador
MODE #canal +i           # Hacer privado (solo por invitación)
MODE #canal -i           # Hacer público
MODE #canal +t           # Restringir cambio de tema a operadores
MODE #canal -t           # Permitir cambio de tema a todos
MODE #canal +k clave     # Establecer clave
MODE #canal -k           # Eliminar clave
MODE #canal +l número    # Limitar usuarios a 'número'
MODE #canal -l           # Eliminar límite de usuarios
*/

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include "../server/Reply.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleMode(Server &srv, Client &client, const std::string &message) {
    std::istringstream iss(message.c_str());
    std::string target, mode, param;
    iss >> target;
    
    if (target.empty()) {
        // Error: No target specified
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back("MODE");
        std::string error = Reply::r_ERR_NEEDMOREPARAMS(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Verificar autenticación
    if (!client.isAuthenticated()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_NOTREGISTERED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Manejo de modos de usuario
    if (target[0] != '#' && target[0] != '&') {
        handleUserMode(srv, client, target, message);
        return;
    }

    // Manejo de modos de canal
    handleChannelMode(srv, client, target, message);
}

void CommandHandler::handleChannelMode(Server &srv, Client &client, const std::string &target, const std::string &message) {
    std::istringstream iss(message.c_str());
    std::string dummy, mode_str;
    iss >> dummy >> mode_str;

    Channel* channel = srv.getChannelManager().getChannelByName(target);
    if (!channel) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(target);
        std::string error = Reply::r_ERR_NOSUCHCHANNEL(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    if (!channel->isOperator(client)) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(target);
        std::string error = Reply::r_ERR_CHANOPRIVSNEEDED(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    char sign = '+';
    size_t param_index = 0;
    std::vector<std::string> params;
    std::string token;
    
    while (iss >> token)
        params.push_back(token);

    for (size_t i = 0; i < mode_str.size(); ++i) {
        char c = mode_str[i];
        
        if (c == '+' || c == '-') {
            sign = c;
            continue;
        }

        switch (c) {
            case 'o': {
                if (param_index >= params.size()) {
                    std::vector<std::string> err_params;
                    err_params.push_back(srv.getServerName());
                    err_params.push_back(client.getNickname());
                    err_params.push_back(target);
                    err_params.push_back("+o");
                    std::string error = Reply::r_ERR_NEEDMOREPARAMS(err_params);
                    send(client.getFd(), error.c_str(), error.size(), 0);
                    continue;
                }
                
                Client* target_client = srv.getClientManager().getClientByNickname(params[param_index++]);
                if (!target_client || !channel->isClientInChannel(*target_client)) {
                    std::vector<std::string> err_params;
                    err_params.push_back(srv.getServerName());
                    err_params.push_back(client.getNickname());
                    err_params.push_back(params[param_index-1]);
                    std::string error = Reply::r_ERR_USERNOTINCHANNEL(err_params);
                    send(client.getFd(), error.c_str(), error.size(), 0);
                    continue;
                }

                if (sign == '+') {
                    channel->addOperator(*target_client);
                } else {
                    channel->removeOperator(*target_client);
                }
                
                std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " " + sign + c + " " + target_client->getNickname() + "\r\n";
                channel->broadcast(msg);
                break;
            }
            case 'i': {
                channel->setInviteOnly(sign == '+');
                std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " " + sign + c + "\r\n";
                channel->broadcast(msg);
                break;
            }
            case 't': {
                channel->setTopicRestricted(sign == '+');
                std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " " + sign + c + "\r\n";
                channel->broadcast(msg);
                break;
            }
            case 'k': {
                if (sign == '+') {
                    if (param_index >= params.size()) {
                        // Error: Need key parameter
                        std::vector<std::string> err_params;
                        err_params.push_back(srv.getServerName());
                        err_params.push_back(client.getNickname());
                        err_params.push_back(target);
                        err_params.push_back("+k");
                        std::string error = Reply::r_ERR_NEEDMOREPARAMS(err_params);
                        send(client.getFd(), error.c_str(), error.size(), 0);
                        continue;
                    }
                    channel->setKey(params[param_index++]);
                    std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " +k " + channel->getKey() + "\r\n";
                    channel->broadcast(msg);
                } else {
                    channel->removeKey();
                    std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " -k\r\n";
                    channel->broadcast(msg);
                }
                break;
            }
            case 'l': {
                if (sign == '+') {
                    if (param_index >= params.size()) {
                        // Error: Need limit parameter
                        std::vector<std::string> err_params;
                        err_params.push_back(srv.getServerName());
                        err_params.push_back(client.getNickname());
                        err_params.push_back(target);
                        err_params.push_back("+l");
                        std::string error = Reply::r_ERR_NEEDMOREPARAMS(err_params);
                        send(client.getFd(), error.c_str(), error.size(), 0);
                        continue;
                    }
                    int limit = atoi(params[param_index++].c_str());
                    channel->setUserLimit(limit);
                    std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " +l " + params[param_index-1] + "\r\n";
                    channel->broadcast(msg);
                } else {
                    channel->removeUserLimit();
                    std::string msg = ":" + client.getFullIdentifier() + " MODE " + target + " -l\r\n";
                    channel->broadcast(msg);
                }
                break;
            }
            default: {
                std::vector<std::string> err_params;
                err_params.push_back(srv.getServerName());
                err_params.push_back(client.getNickname());
                err_params.push_back(std::string(1, c));
                std::string error = Reply::r_ERR_UNKNOWNMODE(err_params);
                send(client.getFd(), error.c_str(), error.size(), 0);
            }
        }
    }
}

void CommandHandler::handleUserMode(Server &srv, Client &client, const std::string &target, const std::string &message) {
    std::istringstream iss(message.c_str());
    std::string dummy, mode_str;
    iss >> dummy >> mode_str;

    // Verificar que el objetivo sea el propio usuario
    std::cout << "[DEBUG] MODE command received for user: " << target << std::endl;
    std::cout << "[DEBUG] Current user: " << client.getNickname() << std::endl;
    if (target != client.getNickname()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        std::string error = Reply::r_ERR_USERSDONTMATCH(params);
        send(client.getFd(), error.c_str(), error.size(), 0);
        return;
    }

    // Si no se especificó modo, mostrar modos actuales
    if (mode_str.empty()) {
        std::vector<std::string> params;
        params.push_back(srv.getServerName());
        params.push_back(client.getNickname());
        params.push_back(client.getNickname());
        params.push_back("+"); // Modos actuales (aquí solo soportamos +i)
        std::string reply = Reply::r_RPL_UMODEIS(params);
        send(client.getFd(), reply.c_str(), reply.size(), 0);
        return;
    }

    char sign = '+';
    bool changes_made = false;
    std::string modes_changed;
    std::string params_changed;

    for (size_t i = 0; i < mode_str.size(); ++i) {
        char c = mode_str[i];
        
        if (c == '+' || c == '-') {
            sign = c;
            continue;
        }

        // Solo soportamos el modo +i (usuario invisible)
        if (c == 'i') {
            if (sign == '+') {
                if (!client.isInvisible()) {
                    client.setInvisible(true);
                    modes_changed += "+i";
                    changes_made = true;
                }
            } else {
                if (client.isInvisible()) {
                    client.setInvisible(false);
                    modes_changed += "-i";
                    changes_made = true;
                }
            }
        } else {
            // Modo no soportado
            std::vector<std::string> err_params;
            err_params.push_back(srv.getServerName());
            err_params.push_back(client.getNickname());
            err_params.push_back(std::string(1, c));
            std::string error = Reply::r_ERR_UMODEUNKNOWNFLAG(err_params);
            send(client.getFd(), error.c_str(), error.size(), 0);
        }
    }

    // Notificar cambios si los hubo
    if (changes_made) {
        std::string msg = ":" + client.getFullIdentifier() + " MODE " + client.getNickname() + " :" + modes_changed + "\r\n";
        send(client.getFd(), msg.c_str(), msg.size(), 0);
    }
}