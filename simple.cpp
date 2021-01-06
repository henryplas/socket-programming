#include <iostream>

// #define ASIO_STANDALONE
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <chrono>

using namespace boost;


std::vector<char> vBuffer(20 * 1024);

void grabSomeData(asio::ip::tcp::socket &socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < length; i++)
					std::cout << vBuffer[i];

				grabSomeData(socket);
			}
		}
	);
}

int main()
{
	system::error_code ec;

	asio::io_context context;

	asio::io_context::work idleWork(context);

	std::thread thrContext = std::thread([&]() { context.run(); });

	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	if(!ec)
	{
		std::cout << "Connected!" << std::endl;

	}
	else
	{
		std::cout << "Failed tp connect to address:\n" << ec.message() << std::endl;
	}

	if(socket.is_open())
	{
		grabSomeData(socket);

		std::string sreq = 
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";
		
		socket.write_some(asio::buffer(sreq.data(), sreq.size()), ec);	

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);

		context.stop();

		if (thrContext.joinable()) 
			thrContext.join();
	}


	return 0;
}