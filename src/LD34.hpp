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

#define SCREEN_W 1440
#define SCREEN_H 720

#define GRID_W 20
#define GRID_H 20

#define ZOOM_W SCREEN_W
#define ZOOM_H SCREEN_H

#define GRID_N_W SCREEN_W/GRID_W
#define GRID_N_H SCREEN_H/GRID_H

template<typename T>
sf::Vector2<T> normalize(const sf::Vector2<T>& source)
{
	T length = sqrt((source.x * source.x) + (source.y * source.y));
	if (length != 0)
		return sf::Vector2<T>(source.x / length, source.y / length);
	else
		return source;
}

#include "colour.hpp"
#include "sectors.hpp"
//#include "particles.hpp"

#define M_PI		3.14159265358979323846
#define M_PI_2	1.57079632679489661923


using namespace std;
using namespace Json;

struct Guy
{
	float m_accel = 0.09f;
	float m_friction = 0.97f;
	sf::Vector2f m_velocity = sf::Vector2f(0.0f, 0.0f);
};

class LD34
{
public:

	LD34() : input(m_commands)
	{

		window.create(sf::VideoMode(SCREEN_W, SCREEN_H),
			"LD34 - shrapx", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

		window.setVerticalSyncEnabled(true);
		window.setKeyRepeatEnabled(false);

		render_texture.create(ZOOM_W, ZOOM_H, false);

		render_sprite.setTexture(render_texture.getTexture());
		render_sprite.setScale( SCREEN_W/ZOOM_W, SCREEN_H/ZOOM_H );

		//background_sprite.setTexture(background_texture.getTexture());
		//background_sprite.setScale(1/zoom_factor,1/zoom_factor);

		view = render_texture.getDefaultView();

		//view.setCenter(0,0);

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

		//m_particles.m_texture = m_textures.at("particle");
		m_sectors.m_texture_bg = m_textures.at("grid");
		m_sectors.m_texture_active = m_textures.at("pixel");

		m_sprites.emplace( "guy", make_shared<sf::Sprite>( *m_textures.at("guy").get() ) );
		m_sprites.at("guy")->setPosition(SCREEN_W/2, SCREEN_H/2);

		m_sprites.emplace( "bg1", make_shared<sf::Sprite>( *m_textures.at("bg1").get() ) );

		//m_sprites.at("bg1").setScale(screen_w, screen_h);

	};

	int run()
	{
		config( "data/config.json" );

		bool game_over = false;

		while(!game_over)
		{
			if ( timing.update() )
			{
				// events here?
				update();
			}
			else
			{
				render();
			}

			sf::Event event;
			while(window.pollEvent(event))
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
			//game_over = input.events(window);
		}

		window.close();
		return 0;
	}

private:

	bool get_command(int id, const string& cmd)
	{
		return m_commands[id][cmd];
	}

  void update()
  {
		auto click_pos = sf::Mouse::getPosition(window);




		float ud = int( -get_command(0,"up")  ) + int( get_command(0,"down") );
		float lr = int( -get_command(0,"left")) + int( get_command(0,"right"));


		guy.m_velocity.x += lr * guy.m_accel;
		guy.m_velocity.y += ud * guy.m_accel;

		guy.m_velocity.x *= guy.m_friction;
		guy.m_velocity.y *= guy.m_friction;

		m_sprites.at("guy")->move(guy.m_velocity);

		//m_sprites.at("guy")->move( lr * 0.5f, ud * 0.5f);

		auto pos = m_sprites.at("guy")->getPosition();

		m_sectors.apply_force( pos.x, pos.y, 250, 3 );

		bool click = get_command(0,"click");

		if (click)
		{
			//m_sectors.apply_force(click_pos.x, click_pos.y, 50, 1);
		}
		m_sectors.update();
		//m_particles.update();

			//auto winsize = window.getSize();
			//float w = (click_pos.x / float(winsize.x)) * GRID_N_W;
			//float h = (click_pos.y / float(winsize.y)) * GRID_N_H;
			//m_sectors.m_array[w][h].set_pos( sf::Vector2f( ((w-int(w))*2)-1.0f, ((h-int(h))*2)-1.0f ));

			//auto fb_pos = m_sectors.get_pos(w,h);
			//cout << fb_pos.x << endl;
			//cout << fb_pos.x << ":" << fb_pos.y << endl;

			/*if ( click_pos.x < 400 )
			{
				cout << "left" << endl;
			}
			else
			{
				cout << "right" << endl;
			}*/

		/*else if (le)
		{
			m_sectors.add();
		}*/

		/*else if (le)
		{
			for ( int i=0; i<100; ++i)
				m_particles.rem();
		}*/

		/*
		const sf::Vector2f& center = view.getCenter();
		auto cen = sf::Vector2f( center.x + (ri - le)*4, center.y + (dw - up)*4 );
		if (cen.x > WORLD_MAX_X) cen.x = WORLD_MAX_X;
		if (cen.y > WORLD_MAX_Y) cen.y = WORLD_MAX_Y;
		if (cen.x < WORLD_MIN_X) cen.x = WORLD_MIN_X;
		if (cen.y < WORLD_MIN_Y) cen.y = WORLD_MIN_Y;
		view.setCenter( cen );*/
  };

  void render()
  {
		render_texture.setView(view);
		render_texture.clear(sf::Color(8,128, 96));

		//float ipo = timing.get_interpolation();

		//render_texture.draw(sprite);

		//render_texture.draw(m_sprite_bg);
		//render_texture.draw( *m_sprites.at( "bg1").get() );

		//render_texture.draw(m_particles);
		render_texture.draw(m_sectors);

		/// draw fg

		render_texture.draw( *m_sprites.at("guy").get() );
		/// draw ui

		//auto ui = view.getCenter();
		//window.draw(text);
		//window.display();


		render_texture.display();

		window.draw( render_sprite );
		window.display();
  };

  Input input;
	Timing timing;

	sf::RenderWindow window;
	sf::View view;


	float zoom_factor = 1/4.0f;
	sf::Sprite render_sprite;
	sf::RenderTexture render_texture;

	unordered_map<int, unordered_map<string, bool>> m_commands;

	unordered_map<string, shared_ptr<sf::Texture>> m_textures;
	unordered_map<string, shared_ptr<sf::Sprite>> m_sprites;

	Guy guy;
	//sf::Sprite m_sprite_bg;

	//Particles<10000> m_particles;
	Sectors<GRID_N_W,GRID_N_H> m_sectors;

};

#endif
