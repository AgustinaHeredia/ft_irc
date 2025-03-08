/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:22 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 00:04:52 by agusheredia      ###   ########.fr       */
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
		std::string topic;
		
	public:
		Channel(const std::string &name);
		void addClient(Client &client);
		void removeClient(Client &client);
		void broadcast(const std::string &message);
		std::string getName() const;
		bool isClientInChannel(const Client &client) const;
		bool isEmpty() const;
		bool isOperator(const Client &client) const;
		void setTopic(const std::string& newTopic);
		std::string getTopic() const;

};

#endif
