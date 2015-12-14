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

#define LIGHT_DISTANCE 50
#define LIGHT_DISTANCE_OFF 100000

#define TILE_SIZE 8
#define TILES_MAX 16
#define TILES_SQUARED 4

#define SCREEN_W 1440
#define SCREEN_H 720

#define WORLD_SIZE 64

#define ZOOM 6.0f
#define ZOOM_W SCREEN_W/ZOOM
#define ZOOM_H SCREEN_H/ZOOM

#define SCREEN_GRID_W SCREEN_W/float(TILE_SIZE)
#define SCREEN_GRID_H SCREEN_H/float(TILE_SIZE)

#define ESTATE_TILE_W ZOOM_W/float(TILE_SIZE)
#define ESTATE_TILE_H ZOOM_H/float(TILE_SIZE)

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
//#include "particles.hpp"
//#include "sectors.hpp"
#include "world.hpp"


using namespace std;
using namespace Json;

struct Guy
{
	float num_torches = 3;
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
		render_sprite.setScale( float(ZOOM), float(ZOOM) );

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
		//m_sectors.m_texture_bg = m_textures.at("grid");
		//m_sectors.m_texture_active = m_textures.at("pixel");

		m_world.m_texture = m_textures.at("world");

		//auto start_loc = sf::Vector2f(ZOOM_W/2, ZOOM_H/2);
		auto start_loc = sf::Vector2f((WORLD_SIZE/2) * TILE_SIZE, (WORLD_SIZE/2) * TILE_SIZE);

		//m_world.generate_room( sf::Vector2f( 180, 90), 5,4 );
		//m_world.generate_room( sf::Vector2f(ZOOM_W/2, ZOOM_W/2), 6,5 );
		m_world.generate_room( start_loc, 6,5 );

		m_sprites.emplace( "guy", make_shared<sf::Sprite>( *m_textures.at("guy").get() ) );

		m_sprites.emplace( "torch_icon", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );
		//m_sprites.emplace( "torch", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );

		m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24,0, 32, 16) );
		m_sprites.at("torch_icon")->setOrigin(32+8,16);

		m_sprites.at("guy")->setPosition(start_loc);
		m_sprites.at("guy")->setOrigin(4,4);
		//m_sprites.at("guy")->setPosition(ZOOM_W/2, ZOOM_H/2);

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

		int ud = int( -get_command(0,"up")  ) + int( get_command(0,"down") );
		int lr = int( -get_command(0,"left")) + int( get_command(0,"right"));

		guy.m_velocity.x += lr * guy.m_accel;
		guy.m_velocity.y += ud * guy.m_accel;

		guy.m_velocity.x *= guy.m_friction;
		guy.m_velocity.y *= guy.m_friction;

		auto pos = m_sprites.at("guy")->getPosition();
		pos += guy.m_velocity;

		if ( !m_world.can_walk(pos) )
		{
			if( fabs(guy.m_velocity.x) > fabs(guy.m_velocity.y) )
			{
				guy.m_velocity.x *= -0.75f;
				guy.m_velocity.y *= 0.25f;
			}
			else
			{
				guy.m_velocity.y *= -0.75f;
				guy.m_velocity.x *= 0.25f;
			}
			//float x = guy.m_velocity.x;
			//guy.m_velocity.x = guy.m_velocity.y;
			//guy.m_velocity.y = -x;
		}
		else
		{
			view.setCenter( pos );

			m_sprites.at("guy")->setPosition(pos);

			bool take  = get_command(0,"take");
			bool place = get_command(0,"place");

			if ( take )
			{
				if ( m_world.light_take(pos) );
				{
					guy.num_torches++;

					if (guy.num_torches > 0)
						m_world.reset_moving_torch(0, pos);
				}
			}
			else if ( place && (guy.num_torches > 0) )
			{
				if ( m_world.light_place(pos) )
				{
					guy.num_torches--;

					if (guy.num_torches == 0)
						m_world.unset_moving_torch(0);
				}
			}

			/*if ( take || place)
			{
				guy.m_velocity *= 0.5f;
			}*/

			if (guy.num_torches > 0)
				m_world.set_moving_torch(0, pos);
		}

		//m_particles.update();
		//m_sectors.update();
		m_world.update();
  };

  void render()
  {
		render_texture.setView(view);
		render_texture.clear(sf::Color(8,8,10));

		//float ipo = timing.get_interpolation();

		//render_texture.draw(sprite);

		//render_texture.draw(m_sprite_bg);
		//render_texture.draw( *m_sprites.at( "bg1").get() );

		//render_texture.draw(m_particles);
		//render_texture.draw(m_sectors);
		render_texture.draw(m_world);

		/// draw fg

		render_texture.draw( *m_sprites.at("guy").get() );
		/// draw ui

		const sf::Vector2f& center = view.getCenter();

		for ( int i=0; i< guy.num_torches; ++i )
		{
			m_sprites.at("torch_icon")->setPosition( (center.x  + (ZOOM_W*0.5f)) - 8 - (8*i), (center.y + (ZOOM_H*0.5f)) - 8);
			//m_sprites.at("torch_icon")->setPosition( center.x, center.y);
			render_texture.draw( *m_sprites.at("torch_icon").get() );
		}

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


	float zoom_factor = 1.0f/ZOOM;
	sf::Sprite render_sprite;
	sf::RenderTexture render_texture;

	unordered_map<int, unordered_map<string, bool>> m_commands;

	unordered_map<string, shared_ptr<sf::Texture>> m_textures;

	unordered_map<string, shared_ptr<sf::Sprite>> m_sprites;

	Guy guy;
	//sf::Sprite m_sprite_bg;

	//Particles<10000> m_particles;
	//Sectors<GRID_N_W,GRID_N_H> m_sectors;

	World<WORLD_SIZE,WORLD_SIZE> m_world;

};

#endif
