#pragma once

#include "socket.h"

#include <string>
#include <string_view>

namespace net_utils
{ 
struct Connection
{
	Socket socket;
	std::string recv_buf;
	std::string send_buf;
};

bool ReceiveInto(Connection& connection)
{
	char buffer[4096];
	ssize_t bytes_read = recv(connection.socket.fd, buffer, sizeof(buffer));

	if (bytes_read <= 0)
		return false;
	
	connection.recv_buf.append(buffer, bytes_read);
	return true;
}

bool SendAllData(Connection& connection, std::string_view data)
{
	ssize_t total_sent = 0;

	while (total_sent < data.size())
	{
		ssize_t sent = send(connection.fd, data.data() + total_sent, data.size() - total_sent, 0);
		if (sent <= 0)
			return false;

		total_sent += sent;
	}
	return true;
}
}
