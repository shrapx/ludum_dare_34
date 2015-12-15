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

#define SCREEN_W 1440
#define SCREEN_H 720
#define ZOOM 6.0f
#define ZOOM_W SCREEN_W/ZOOM
#define ZOOM_H SCREEN_H/ZOOM

#include "colour.hpp"

#include "guy.hpp"
#include "level.hpp"
#include "main_menu.hpp"
#include "scores.hpp"

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

		/// sounds
		for (string snd_name : config_json["sounds"].getMemberNames() )
		{
			string snd_file = config_json["sounds"][snd_name].asString();

			cout << snd_name << ":" << snd_file << endl;

			m_soundbuffers.emplace(snd_name, make_shared<sf::SoundBuffer>());

			auto& snd = m_soundbuffers[snd_name];

			if ( snd->loadFromFile(snd_file) )
			{
				cout << "sound success." << endl;
			}
			else cout << "sound failed." << endl;
		}

	};

	int run()
	{
		config( "data/config.json" );



		bool retry = true;
		while (retry)
		{

			// show character option

			int option_character = rand()%4;

			m_guy = make_shared<Guy>(option_character);

			m_guy->m_level = 0;

			bool next_level = true;
			while( next_level )
			{
				m_guy->level_restart();
				m_level = make_shared<Level>(m_window, m_commands, m_textures, m_soundbuffers, m_guy);

				// run game loop

				next_level = game_play_loop();

				// fade out
				m_guy->m_level += 1;



				m_level.reset();
			}

			// dead

			// score display

			m_scores = make_shared<Scores>(m_window, m_commands, m_textures, m_soundbuffers, m_guy);

			retry = game_score_loop();

			m_scores.reset();
			m_guy.reset();
		}

		m_window.close();
		return 0;
	}

	bool game_play_loop()
	{
		// fade in

		int game_state = 0;
		while( game_state == 0 )
		{
			if ( timing.update() )
			{
				game_state = m_level->update();
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
						game_state = -1;
					}
					default: input.handle_event(event);
				}
			}
		}

		//if (game_state==1)
			// fade out

		return game_state==1;
	}

	bool game_score_loop()
	{
		int choice_state = 0;
		while( choice_state == 0 )
		{
			if ( timing.update() )
			{
				choice_state = m_scores->update();
			}
			else
			{
				m_scores->render();
				m_window.draw( m_scores->get_sprite() );
				m_window.display();
			}

			sf::Event event;
			while(m_window.pollEvent(event))
			{
				switch (event.type)
				{
					case sf::Event::Closed:
					{
						choice_state = -1;
					}
					default: input.handle_event(event);
				}
			}
		}
		return choice_state==1;
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
	unordered_map<string, shared_ptr<sf::SoundBuffer>> m_soundbuffers;

	shared_ptr<Guy> m_guy;

	shared_ptr<Level> m_level;
	shared_ptr<Scores> m_scores;

};

#endif
