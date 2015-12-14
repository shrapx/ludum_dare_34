#ifndef LD34_HPP_
#define LD34_HPP_

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <time.h>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Audio.hpp"

#include <jsoncpp/json/json.h>
#include "loader.hpp"

#include "timing.hpp"
#include "input.hpp"

#define M_PI		3.14159265358979323846
#define M_PI_2	1.57079632679489661923

#include "colour.hpp"

#include "guy.hpp"
#include "level.hpp"

using namespace std;
using namespace Json;



class LD34
{
public:

	LD34() : input(m_commands)
	{
		m_window.create(sf::VideoMode(SCREEN_W, SCREEN_H),
			"LD34 - shrapx", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

		m_window.setVerticalSyncEnabled(true);
		m_window.setKeyRepeatEnabled(false);

		srand(time(NULL));
	};

	void config(const string& filename = "data/config.json")
	{

		// load config
		Value config_json;
		Loader::load(filename, config_json);

		// keyboard mouse input
		Value& binds = config_json["binds"];

		for(ValueIterator it=binds.begin(); it != binds.end(); it++ )
		{
			string key = it.key().asString();
			string cmd = (*it).asString();
			for (unsigned short i=0; i<keys.size(); ++i)
			{
				string ikey = keys.at(i);
				if (ikey == key )
				{
					cout << " key found " << key << endl;
					input.key_cmd[i] = cmd;
					m_commands[0][cmd] = 0;
					break;
				}
			}
		}

		/// textures
		for (string tex_name : config_json["textures"].getMemberNames() )
		{
			string tex_file = config_json["textures"][tex_name].asString();

			cout << tex_name << ":" << tex_file << endl;

			m_textures.emplace(tex_name, make_shared<sf::Texture>());

			auto& tex = m_textures[tex_name];

			if ( tex->loadFromFile(tex_file) )
			{
				cout << "texture success." << endl;
			}
			else cout << "texture failed." << endl;
		}
	};

	int run()
	{
		config( "data/config.json" );

		m_guy = make_shared<Guy>(0);
		m_level = make_shared<Level>(m_window, m_commands, m_textures, m_guy, 0);

		bool game_over = false;

		while(!game_over)
		{
			if ( timing.update() )
			{
				m_level->update();
			}
			else
			{
				m_level->render();
				m_window.draw( m_level->get_sprite() );
				m_window.display();
			}

			sf::Event event;
			while(m_window.pollEvent(event))
			{
				switch (event.type)
				{
					case sf::Event::Closed:
					{
						game_over = true;
					}
					default: input.handle_event(event);
				}
			}
			//game_over = input.events(m_window);
		}


		m_window.close();
		return 0;
	}

private:

	bool get_command(int id, const string& cmd)
	{
		return m_commands[id][cmd];
	}

  Input input;
	Timing timing;

	sf::RenderWindow m_window;
	sf::View view;

	float zoom_factor = 1.0f/ZOOM;

	unordered_map<int, unordered_map<string, bool>> m_commands;

	unordered_map<string, shared_ptr<sf::Texture>> m_textures;

	shared_ptr<Guy> m_guy;

	shared_ptr<Level> m_level;

};

#endif
