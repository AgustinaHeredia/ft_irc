/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:20:02 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 19:32:25 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_MANAGER_HPP
# define CLIENT_MANAGER_HPP

# include <vector>
# include <map>
# include <algorithm>
# include "../clients/Client.hpp"

class ClientManager {
	private:
		std::vector<Client*> clients;
		std::map<std::string, Client*> nicknames;
		std::vector<Client*> removalQueue;

	public:
		void addClient(Client* client);
		void removeClient(Client* client);
		bool isNicknameInUse(const std::string &nick);
		void addNickname(const std::string &nick, Client* client);
		void removeNickname(const std::string &nick);
		Client* getClientByNickname(const std::string &nick);
		Client* getClientByFd(int fd);
		std::vector<Client*> getAllClients() const;
		void clearAllClients();
		bool isClientValid(Client* client) const;
		void processRemovals();
		void queueForRemoval(Client* client);
};

#endif
