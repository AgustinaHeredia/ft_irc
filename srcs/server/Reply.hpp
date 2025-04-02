/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 18:54:51 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 16:19:15 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLY_HPP
# define REPLY_HPP

#include <string>
#include <vector>

namespace Reply {
    // Códigos de respuesta
    static const std::string RPL_WELCOME = "001";
    static const std::string RPL_NOTOPIC = "331";
    static const std::string RPL_TOPIC = "332";
    static const std::string RPL_NAMREPLY = "353";
    static const std::string RPL_ENDOFNAMES = "366";
    static const std::string ERR_UNKNOWNCOMMAND = "421";
    static const std::string ERR_NEEDMOREPARAMS = "461";
    static const std::string ERR_INVALIDNICK = "432";
    static const std::string ERR_NICKNAMEINUSE = "433";
    static const std::string ERR_NOTREGISTERED = "451";
    static const std::string ERR_NOSUCHCHANNEL = "403";
    static const std::string ERR_CHANOPRIVSNEEDED = "482";
    static const std::string ERR_UMODEUNKNOWNFLAG = "501";
    static const std::string ERR_USERSDONTMATCH = "502";
    static const std::string ERR_UNKNOWNMODE = "472";
    static const std::string ERR_NORECIPIENT = "411";
    static const std::string ERR_NOTEXTTOSEND = "412";
    static const std::string ERR_CANNOTSENDTOCHAN = "404";
    static const std::string ERR_NOSUCHNICK = "401";
    static const std::string ERR_WHOREPLY = "352";
    static const std::string ERR_ENDOFWHO = "315";

    // Funciones para generar respuestas
    std::string r_RPL_WELCOME(const std::vector<std::string> &av);
    std::string r_RPL_NOTOPIC(const std::vector<std::string> &av);
    std::string r_RPL_TOPIC(const std::vector<std::string> &av);
    std::string r_RPL_NAMREPLY(const std::vector<std::string> &av);
    std::string r_RPL_ENDOFNAMES(const std::vector<std::string> &av);
    std::string r_ERR_UNKNOWNCOMMAND(const std::vector<std::string> &av);
    std::string r_ERR_NEEDMOREPARAMS(const std::vector<std::string> &av);
    std::string r_ERR_INVALIDNICK(const std::vector<std::string> &av);
    std::string r_ERR_NICKNAMEINUSE(const std::vector<std::string> &av);
    std::string r_ERR_NOTREGISTERED(const std::vector<std::string> &av);
    std::string r_ERR_NOSUCHCHANNEL(const std::vector<std::string> &av);
    std::string r_ERR_CHANOPRIVSNEEDED(const std::vector<std::string> &av);
    std::string r_ERR_UMODEUNKNOWNFLAG(const std::vector<std::string> &av);
    std::string r_ERR_USERSDONTMATCH(const std::vector<std::string> &av);
    std::string r_ERR_UNKNOWNMODE(const std::vector<std::string> &av);
    std::string r_ERR_NORECIPIENT(const std::vector<std::string> &av);
    std::string r_ERR_NOTEXTTOSEND(const std::vector<std::string> &av);
    std::string r_ERR_CANNOTSENDTOCHAN(const std::vector<std::string> &av);
    std::string r_ERR_NOSUCHNICK(const std::vector<std::string> &av);
    std::string r_RPL_WHOREPLY(const std::vector<std::string>& params);
    std::string r_RPL_ENDOFWHO(const std::vector<std::string>& params);
}
#endif