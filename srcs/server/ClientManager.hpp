/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:20:02 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 19:36:27 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_MANAGER_HPP
# define CLIENT_MANAGER_HPP

# include <vector>
# include <map>
# include "../clients/Client.hpp"

class ClientManager {
	private:
		std::vector<Client*> clients;
		std::map<std::string, Client*> nicknames;

	public:
		void addClient(Client* client);
		void removeClient(Client* client);
		bool isNicknameInUse(const std::string& nick);
		void addNickname(const std::string& nick, Client* client);
		void removeNickname(const std::string& nick);
		Client* getClientByNickname(const std::string& nick);
		Client* getClientByFd(int fd);
};

#endif
