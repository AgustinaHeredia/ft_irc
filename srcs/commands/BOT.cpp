/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BOT.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/06 10:26:51 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <cstdlib> // rand srand
#include <ctime> // time
#include <sstream>
#include <algorithm>
#include <iomanip>

void CommandHandler::handleBot(Client &client, const std::string &message) 
{
    // Verificar autenticación del usuario
    if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Authentication is missing.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;  // Asegúrate de que la función termine aquí si no está autenticado.
    }

    // Si no hay argumentos, mostrar el menú de opciones disponibles
    if (message.empty()) 
    {
        const char* menu = 
            "BOT COMMANDS:\r\n"
            " - !bot commands : Show all the posible commands\r\n"
            " - !bot time : Show server time\r\n"
            " - !bot coinflip : Flip a coin\r\n"
            " - !bot rps <rock|paper|scissors> : Play Rock-Paper-Scissors\r\n";
        send(client.getFd(), menu, strlen(menu), 0);
        return;
    }

    std::istringstream iss(message);
    std::string subcommand;
    iss >> subcommand;

    // Piedra, Papel o Tijera
    if (subcommand == "rps") 
    {
        std::string user_move;
        iss >> user_move;

        if (user_move.empty()) 
        {
            const char* error_msg = "ERROR: You must specify your move (rock, paper, scissors).\r\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Convertir la jugada del usuario a minúsculas
        std::transform(user_move.begin(), user_move.end(), user_move.begin(), ::tolower);

        // Validar la jugada
        if (user_move != "rock" && user_move != "paper" && user_move != "scissors") 
        {
            const char* error_msg = "ERROR: Invalid move. Use 'rock', 'paper', or 'scissors'.\r\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Generar la jugada del bot (0: piedra, 1: papel, 2: tijera)
        std::string options[3] = { "rock", "paper", "scissors" };
        std::string bot_move = options[rand() % 3];

        // Determinar el resultado
        std::string result;
        if (user_move == bot_move) 
            result = "Draw!";
        else if ((user_move == "rock" && bot_move == "scissors") ||
                 (user_move == "paper" && bot_move == "rock") ||
                 (user_move == "scissors" && bot_move == "paper")) 
            result = "You won!";
        else 
            result = "You lost!";

        // Enviar respuesta al cliente
        std::string response = "Bot: I chose " + bot_move + ". " + result + "\r\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
    } 
    // Coinflip (cara o cruz)
    else if (subcommand == "coinflip") 
    {
        std::string result = (rand() % 2 == 0) ? "It came up heads!" : "It came up tails!";
        std::string response = "Bot: " + result + "\r\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
    }
    else if (subcommand == "time") 
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "[%Y-%m-%d] %H:%M:%S UTC", timeinfo);
        
        std::string response = "Server time: ";
        response += buffer;
        response += "\r\n";
        send(client.getFd(), response.c_str(), response.size(), 0);
    }
    else if (subcommand == "commands") {
        const char* menu1 =
            "IRC COMMANDS:\r\n"
            " - NICK <newnick>               : Change your nickname.\r\n"
            " - USER <username> <hostname> <server> <realname> : Set your user info.\r\n"
            " - QUIT [reason]                : Disconnect from the server.\r\n"
            " - PRIVMSG <target> : <message>  : Send a private or channel message.\r\n"
            " - NOTICE <target> : <message>   : Send a notice.\r\n"
            " - JOIN <#channel>              : Join a channel.\r\n"
            " - PART <#channel>              : Leave a channel.\r\n"
            " - KICK <#channel> <user> [reason] : Kick a user from a channel.\r\n"
            " - TOPIC <#channel> [new topic] : Set or view the channel topic.\r\n"
            " - MODE <target> <modes> [params] : Change modes for channels/users.\r\n"
            " - INVITE <user> <#channel>     : Invite a user to a channel.\r\n"
            " - WHO [#channel]             : List users on the server or in a channel.\r\n"
            " - !BOT <subcommand>          : Bot commands (time, coinflip, rps).\r\n";
        send(client.getFd(), menu1, strlen(menu1), 0);
    }
    // Subcomando no reconocido
    else
    {
        const char* error_msg = "ERROR: Unrecognized subcommand. Use !bot to view options.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
    }
}
