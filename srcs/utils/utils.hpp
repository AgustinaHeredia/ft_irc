/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 11:57:53 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 20:00:23 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <cstring>        // Para memset
# include <cstdlib>        // Para exit()
# include <unistd.h>       // Para close()
# include <sys/socket.h>   // Para funciones de socket
# include <netinet/in.h>   // Para sockaddr_in
# include <arpa/inet.h>    // Para inet_ntoa()

std::string intToString(int number);

#endif
