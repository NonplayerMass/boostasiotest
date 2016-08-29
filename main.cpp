#include <boost/asio.hpp>
#include <iostream>

#include <memory>
#include <utility>
#include <vector>
#include <ctime>

using namespace std;


int vhtmlmain()
{
	try
	{

		namespace ip = asio::ip;
		asio::io_service io_service;

		//TCP�\�P�b�g���쐬����
		ip::tcp::socket sock(io_service);

		//���O����(�z�X�g������IP�A�h���X�ɕϊ�)����
		ip::tcp::resolver resolver(io_service);
		ip::tcp::resolver::query query("www.kumei.ne.jp", "http");

		//�z�X�g����ݒ肷��
		ip::tcp::endpoint endpoint(*resolver.resolve(query));

		//�\�P�b�g�֐ڑ�
		sock.connect(endpoint);

		//���b�Z�[�W�𑗐M
		asio::streambuf request;
		ostream request_ostream(&request);
		request_ostream << "GET /c_lang/index_cp.html HTTP/1.0\r\n\r\n";
		asio::write(sock, request);

		//���b�Z�[�W����M
		asio::streambuf buffer;
		asio::error_code error;
		asio::read(sock, buffer, asio::transfer_all(), error);
		if (error && error != asio::error::eof)
		{
			std::cout << "receive failed: " << error.message() << std::endl;
		}
		else
		{
			cout << &buffer;
		}
	}
	catch (exception& e)
	{
		cout << e.what();
	}
	return 0;
}


class shared_const_buffer
{
public:
    // Construct from a std::string.
    explicit shared_const_buffer(const std::string& data)
            : data_(new std::vector<char>(data.begin(), data.end())),
              buffer_(asio::buffer(*data_))
    {
    }

    // Implement the ConstBufferSequence requirements.
    typedef asio::const_buffer value_type;
    typedef const asio::const_buffer* const_iterator;
    const asio::const_buffer* begin() const { return &buffer_; }
    const asio::const_buffer* end() const { return &buffer_ + 1; }

private:
    std::shared_ptr<std::vector<char> > data_;
    asio::const_buffer buffer_;
};

class session
        : public std::enable_shared_from_this<session>
{
public:
    session(asio::ip::tcp::socket socket)
            : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_write();
    }

private:
    void do_write()
    {
        std::time_t now = std::time(0);
        shared_const_buffer buffer(std::ctime(&now));

        auto self(shared_from_this());
        asio::async_write(socket_, buffer,
                [this, self](asio::error_code /*ec*/, std::size_t /*length*/)
                {
                });
    }

    // The socket used to communicate with the client.
    asio::ip::tcp::socket socket_;
};

class server
{
public:
    server(asio::io_service& io_service, short port)
            : acceptor_(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
              socket_(io_service)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_,
                [this](asio::error_code ec)
                {
                    if (!ec)
                    {
                        std::make_shared<session>(std::move(socket_))->start();
                    }

                    do_accept();
                });
    }

	asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
};

void testmainloop(){
	    try
    {
    /*    if (argc != 2)
        {
            std::cerr << "Usage: reference_counted <port>\n";
            return 1;
        }
*/
        asio::io_service io_service;

        server s(io_service, 53322);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    
}



int client();
int server();

int chatclient()
{
	using namespace std;
	namespace ip = asio::ip;

    char c;
    cout << "�N���C�A���g�H�T�[�o�[�Hc/s\n>";
    cin >> c;
    if (c == 'c')
    {
        client();
    }
    else if (c == 's')
    {
        server();

    }
	return 0;
}

int client(){
		using namespace std;
	namespace ip = asio::ip;

try
{
    asio::io_service io_service;

    //TCP�\�P�b�g���쐬����
    ip::tcp::socket sock(io_service);

    //�z�X�g�����擾
    cout << "�T�[�o�[��\n>";
    string server_name;
    cin >> server_name;
    cout << "�|�[�g�ԍ�\n>";
    u_short port = 0;
    cin >> port;
    sock.connect(ip::tcp::endpoint(ip::address::from_string(server_name), port));

    //���b�Z�[�W�𑗎�M
    string buffer;
    while (true)
    {
        cout << "���M\n>";
        cin >> buffer;
        asio::write(sock, asio::buffer(buffer));
        if (buffer == "end")
        {
            break;
        }
        cout << "�T�[�o�[����Ԏ���҂��Ă��܂�\n";

        asio::streambuf receive_buffer;
        asio::error_code error;
        asio::read(sock, receive_buffer, asio::transfer_at_least(1), error);
        if (error && error != asio::error::eof)
        {
            std::cout << "receive failed: " << error.message() << std::endl;
        }
        else if (asio::buffer_cast<const char*>(receive_buffer.data()) == string("end"))
        {
            cout << "�T�[�o�[���ڑ���؂�܂���\n";
            break;
        }

        cout << "��M:" << &receive_buffer << endl;
    }

    return 0;
}
catch (exception& e)
{
    cout << e.what();
    return 1;
}
}

int server()
{	
	using namespace std;
	namespace ip = asio::ip;

	try
	{
		asio::io_service io_service;

		//TCP�\�P�b�g���쐬����
		ip::tcp::socket sock(io_service);

		//IPv4�̃\�P�b�g�A�h���X����ݒ肷��
		cout << "�|�[�g�ԍ�\n>";
		u_short port = 0;
		cin >> port;
		ip::tcp::acceptor acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), port));

		//�N���C�A���g����̐ڑ����󂯓����
		acceptor.accept(sock);

		//���b�Z�[�W�𑗎�M
		string buffer;
		while (true)
		{
			cout << "�N���C�A���g����̎�M��҂��Ă��܂�\n>";
			asio::streambuf receive_buffer;
			asio::error_code error;
			asio::read(sock, receive_buffer, asio::transfer_at_least(1), error);
			if (error && error != asio::error::eof)
			{
				std::cout << "receive failed: " << error.message() << std::endl;
			}
			else if (asio::buffer_cast<const char*>(receive_buffer.data()) == string("end"))
			{
				cout << "�N���C�A���g���ڑ���؂�܂���\n";
				break;
			}

			cout << "��M:" << &receive_buffer << endl;

			cout << "���M\n>";
			cin >> buffer;
			asio::write(sock, asio::buffer(buffer));
			if (buffer == "end")
			{
				break;
			}
		}
		return 0;
	}
	catch (exception& e)
	{
		cout << e.what();
		return 1;
	}

}


void main(){
	//vhtmlmain();
	//testmainloop();
	chatclient();
}