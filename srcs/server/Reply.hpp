/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 18:54:51 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 14:02:16 by pquintan         ###   ########.fr       */
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
}
#endif
