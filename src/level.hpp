#ifndef LEVEL_HPP_
#define LEVEL_HPP_

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

#include "guy.hpp"

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

class Level
{
public:

	Level(
		sf::RenderWindow& window,
		unordered_map<int, unordered_map<string, bool>>& commands,
		unordered_map<string, shared_ptr<sf::Texture>>& textures,
		shared_ptr<Guy>& guy,
		int level_num)
	:
		m_window(window),
		m_commands(commands),
		m_textures(textures),
		m_guy(guy)
	{

		m_rendertexture.create(ZOOM_W, ZOOM_H, false);

		m_view = m_rendertexture.getDefaultView();

		m_render_sprite.setTexture(m_rendertexture.getTexture());
		m_render_sprite.setScale( float(ZOOM), float(ZOOM) );



		switch ( level_num )
		{

		}

		m_world.m_texture = m_textures.at("world");

		//auto start_loc = sf::Vector2f(ZOOM_W/2, ZOOM_H/2);
		auto start_loc = sf::Vector2f((WORLD_SIZE/2) * TILE_SIZE, (WORLD_SIZE/2) * TILE_SIZE);

		//m_world.generate_room( sf::Vector2f( 180, 90), 5,4 );
		//m_world.generate_room( sf::Vector2f(ZOOM_W/2, ZOOM_W/2), 6,5 );
		m_world.generate_room( start_loc, 6,5 );



		m_sprites.emplace( "torch_icon", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );

		m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24,0, 32, 16) );
		m_sprites.at("torch_icon")->setOrigin(4,16);

		m_sprites.emplace( "torch", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );
		m_sprites.at("torch")->setTextureRect( sf::IntRect(8, 0, 8, 8) );
		m_sprites.at("torch")->setOrigin(4,4);

		m_sprites.emplace( "guy", make_shared<sf::Sprite>( *m_textures.at("guy").get() ) );
		m_sprites.at("guy")->setPosition(start_loc);
		m_sprites.at("guy")->setOrigin(2,2);
	};


  void update()
  {
		auto click_pos = sf::Mouse::getPosition(m_window);

		int ud = int( -get_command(0,"up")  ) + int( get_command(0,"down") );
		int lr = int( -get_command(0,"left")) + int( get_command(0,"right"));

		m_guy->m_velocity.x += lr * m_guy->m_accel;
		m_guy->m_velocity.y += ud * m_guy->m_accel;

		m_guy->m_velocity.x *= m_guy->m_friction;
		m_guy->m_velocity.y *= m_guy->m_friction;

		//m_sprites.at("guy")->setTextureRect( sf::IntRect((lr!=0)*4, 0, 4, 4) );

		auto pos = m_sprites.at("guy")->getPosition();
		pos += m_guy->m_velocity;


  	if (m_guy->m_torches.size())
		{
			m_guy->m_torches.back() -= 0.03f;
			if (m_guy->m_torches.back() < 0.0f)
			{
				m_guy->m_torches.pop_back();
				m_world.set_moving_torch(0, false,  pos);
			}
		}

		if ( !m_world.can_walk(pos) )
		{
			if( fabs(m_guy->m_velocity.x) > fabs(m_guy->m_velocity.y) )
			{
				m_guy->m_velocity.x *= -0.75f;
				m_guy->m_velocity.y *= 0.25f;
			}
			else
			{
				m_guy->m_velocity.y *= -0.75f;
				m_guy->m_velocity.x *= 0.25f;
			}
			//float x = m_guy->m_velocity.x;
			//m_guy->m_velocity.x = m_guy->m_velocity.y;
			//m_guy->m_velocity.y = -x;
		}
		else
		{
			m_view.setCenter( pos );

			m_sprites.at("guy")->setPosition(pos);

			bool take  = get_command(0,"take");
			bool place = get_command(0,"place");

			//bool no_torch = m_guy->num_torches == 0;
			bool no_torch = m_guy->m_torches.size() == 0;

			if ( take )
			{
				if ( m_world.light_take(pos) )
				{
					//m_guy->num_torches++;
					m_guy->m_torches.push_back(m_guy->m_torch_default);

					if (no_torch)
					{
						//m_world.unset_moving_torch(0);
						//m_world.reset_moving_torch(0, pos);
						m_world.set_moving_torch(0, true,  pos);
					}
				}
				//cout << m_guy->num_torches << endl;
			}
			else if ( place && (!no_torch) )
			{

				if ( m_world.light_place(pos) )
				{
					//m_guy->num_torches--;
					m_guy->m_torches.pop_back();

					//if (m_guy->num_torches == 0) m_world.unset_moving_torch(0);
					if (m_guy->m_torches.size() == 0) m_world.set_moving_torch(0, false,  pos);
				}
				//cout << m_guy->num_torches << endl;
			}

			/*if ( take || place)
			{
				m_guy->m_velocity *= 0.5f;
			}*/

			if (m_guy->m_torches.size() > 0) m_world.set_moving_torch(0, true,  pos);


			//int is_right = (dira > (M_PI*0.5f) ) * 4; // left or right
			//if (dira > 2.32 || dira < 0.782) // left or right
			float dir = atan2(m_guy->m_velocity.y, m_guy->m_velocity.x);
			float dira = fabs(dir);

			if (dira != dir) dira = M_PI-dira;

			m_guy->m_tex_rect.left = (int((dira/M_PI) * 4.5) * 4)%16;

			m_sprites.at("guy")->setTextureRect( m_guy->m_tex_rect );
			//m_sprites.at("guy")->setTextureRect( sf::IntRect( rx, ry, 4, 4 ) );
		}

		//m_particles.update();
		//m_sectors.update();
		m_world.update();
  };

  void render()
  {
		m_rendertexture.setView(m_view);
		m_rendertexture.clear(sf::Color(8,8,12));

		//float ipo = timing.get_interpolation();

		//m_rendertexture.draw(sprite);

		//m_rendertexture.draw(m_sprite_bg);
		//m_rendertexture.draw( *m_sprites.at( "bg1").get() );

		// bg

		//m_rendertexture.draw(m_particles);
		//m_rendertexture.draw(m_sectors);
		m_rendertexture.draw(m_world);

		/// fg

		for ( auto& pos : m_world.m_light_positions_placed)
		{
			m_sprites.at("torch")->setPosition( pos );
			m_rendertexture.draw( *m_sprites.at("torch").get() );
		}

		m_rendertexture.draw( *m_sprites.at("guy").get() );

		/// ui

		const sf::Vector2f& center = m_view.getCenter();

		for ( int i=0; i< m_guy->m_torches.size(); ++i )
		{
			if (i == m_guy->m_torches.size()-1)
			{
				int height = (m_guy->m_torches[i] / m_guy->m_torch_default) * 16;
				m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24, 16-height, 32, height));
				m_sprites.at("torch_icon")->setPosition( (center.x  + (ZOOM_W*0.5f)) - 8 - (8*i), (center.y + (ZOOM_H*0.5f)) - 8 + (16-height));
			}
			else
			{
				m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24,0, 32, 16));
				m_sprites.at("torch_icon")->setPosition( (center.x  + (ZOOM_W*0.5f)) - 8 - (8*i), (center.y + (ZOOM_H*0.5f)) - 8);
			}

			//m_sprites.at("torch_icon")->setPosition( center.x, center.y);
			m_rendertexture.draw( *m_sprites.at("torch_icon").get() );
		}

		m_rendertexture.display();
  };

	sf::Sprite& get_sprite()
	{
		return m_render_sprite;
	}

private:

	bool get_command(int id, const string& cmd)
	{
		return m_commands[id][cmd];
	}

	sf::RenderWindow& m_window;

	unordered_map<int, unordered_map<string, bool>>& m_commands;
	unordered_map<string, shared_ptr<sf::Texture>>& m_textures;


	sf::Sprite m_render_sprite;
	sf::RenderTexture m_rendertexture;
	sf::View m_view;

	unordered_map<string, shared_ptr<sf::Sprite>> m_sprites;

	shared_ptr<Guy> m_guy;
	//sf::Sprite m_sprite_bg;

	//Particles<10000> m_particles;
	//Sectors<GRID_N_W,GRID_N_H> m_sectors;

	World<WORLD_SIZE,WORLD_SIZE> m_world;

};

#endif
