/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:22 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 19:38:10 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//representará un canal IRC

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "../clients/Client.hpp"

class Channel {
	private:
		std::string name;
		std::vector<Client*> clients;
		std::vector<Client*> invitedUsers;
		std::string topic;
		std::vector<Client*> operators;
		bool inviteOnly;   //  `+i`: Canal solo por invitación
		bool topicRestricted; // `+t`: Solo operadores pueden cambiar el topic
		std::string key;   // `+k`: Clave del canal
		int userLimit;     // `+l`: Límite de usuarios (0 = sin límite)
		
	public:
		Channel(const std::string &name);
		~Channel();

		void addClient(Client &client);
		void removeClient(Client &client);
		void broadcast(const std::string &message);
		std::string getName() const;
		bool isClientInChannel(const Client &client) const;
		bool isEmpty() const;

		bool isOperator(const Client &client) const;
		void addOperator(Client& client);
		void removeOperator(Client &client);
		void setOperator(Client& client, bool isOperator);


		void setInviteOnly(bool state);
		bool isInviteOnly() const;

		void setTopicRestricted(bool state);
		bool isTopicRestricted() const;

		void setKey(const std::string &newKey);
		void removeKey();
		std::string getKey() const;

		void setUserLimit(int limit);
		void removeUserLimit();
		int getUserLimit() const;

		void setTopic(const std::string &newTopic);
		std::string getTopic() const;

		void inviteUser(Client& client);
		bool isUserInvited(const Client &client) const;
		void removeInvitedUser(Client &client);

		std::vector<Client*> getClients() const;
		void broadcastQuit(const std::string& nickname, const std::string& reason);
		bool hasClient(const std::string& nickname) const;
 	    void removeClient(const std::string& nickname);

};

#endif
