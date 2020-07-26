#include"pch.h"
#include <SFML/Graphics.hpp>
#include <time.h>
#include<SFML/Audio.hpp>
#include<SFML/Network.hpp>
#include<string>
#include <map>
#include<iostream>

using namespace sf;
using namespace std;

struct point
{
	int x, y;
};

int main()
{
	//Client

	char data[100] = "Good";
	//connect server
	sf::IpAddress ip = IpAddress::getLocalAddress();
	sf::TcpSocket socket;
	unsigned int port = 2000;

	string account;
	string status = "connected";
	map<string, int> accounts;

	std::cout << "Please enter your account: ";
	std::cin >> account;

	sf::Socket::Status stat = socket.connect(ip, port);

	if (stat != sf::Socket::Done)
	{
		cout << "Connect To : " << ip << " Failed" << endl;
	}

	sf::Packet packet;

	packet << status << account;
	socket.send(packet);
	socket.setBlocking(false);

	/*if (socket.send(data, 100) != sf::Socket::Done)
	{
		cout << "Send Failed" << endl;
	}*/


	srand(time(0));

	RenderWindow app(VideoMode(400, 533), "Doodle Game");
	app.setFramerateLimit(60);

	Texture t1, t2, t3;
	t1.loadFromFile("images/background.png");
	t2.loadFromFile("images/platform.png");
	t3.loadFromFile("images/doodle.png");

	Font font;						//字体对象
	Text text;						//文本对象

	//加载字体
	if (!font.loadFromFile("Fonts/fangmeng.ttf"))
	{
		std::cout << "字体fangmeng.ttf没有找到" << std::endl;
	}
	text.setFont(font);

	SoundBuffer sbBgm;
	Sound soundBgm;
	bool isPause = false;
	bool isStart = false;

	if (!sbBgm.loadFromFile("Audios/Bgm.ogg"))
	{
		std::cout << "bgm.ogg没有找到" << std::endl;
	}
	soundBgm.setBuffer(sbBgm);
	soundBgm.play();

	Sprite sBackground(t1), sPlat(t2), sPers(t3);

	point plat[20];



	for (int i = 0; i < 10; i++)
	{
		plat[i].x = rand() % 400;
		plat[i].y = rand() % 533;
	}

	struct ClientPkg
	{
		int x, y;
		int height;
	};

	int x = 100, y = 100, h = 200, height=0;
	float dx = 0, dy = 0;

	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
			if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space)
			{
				isPause = !isPause;

			}
		}


		if (!isPause)
		{

			sf::Packet response;
			socket.receive(response);
			while (!response.endOfPacket())
			{
				string account;
				size_t amount;
				response >> account >> height;
				std::cout << account << " height:" << height << endl;
			}

			if (Keyboard::isKeyPressed(Keyboard::Right)) x += 3;
			if (Keyboard::isKeyPressed(Keyboard::Left)) x -= 3;


			dy += 0.2;
			y += dy;
			if (y > 500) {
				dy = -10;
			}

			if (y < h) {
				height -= dy;
				for (int i = 0; i < 10; i++)
				{
					y = h;
					plat[i].y = plat[i].y - dy;
					if (plat[i].y > 533) { plat[i].y = 0; plat[i].x = rand() % 400; }
				}
			}

			for (int i = 0; i < 10; i++)
				if ((x + 50 > plat[i].x) && (x + 20 < plat[i].x + 68)
					&& (y + 70 > plat[i].y) && (y + 70 < plat[i].y + 14) && (dy > 0)) {
					dy = -10;
				}


			//将当前角色位置发给服务器

			/*
			sf::Packet posPack;
			posPack << x << y << account;
			if (socket.send(posPack) != sf::Socket::Done)
			{
				cout << "posPack Send Failed" << endl;
			}
			*/
			sf::Packet sendPacket;
			sendPacket << height; 
			if (socket.send(sendPacket) != sf::Socket::Done)
			{
				cout << "packet send failed" << endl;
			}
			printf("x:%d y:%d height:%d", x, y, height);
			printf("\r");
		}


		sPers.setPosition(x, y);
		app.draw(sBackground);
		app.draw(sPers);
		for (int i = 0; i < 10; i++)
		{
			sPlat.setPosition(plat[i].x, plat[i].y);
			app.draw(sPlat);
		}

		if (isPause)
		{
			text.setCharacterSize(30);
			text.setFillColor(Color(14, 92, 109, 255));
			text.setStyle(Text::Bold);

			text.setPosition(160, 230);
			text.setString(L"暂停中");
			app.draw(text);
		}

		app.display();
	}

	return 0;
}