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

#define LIGHT_DISTANCE 50
#define LIGHT_DISTANCE_OFF 100000

#define TILE_SIZE 8
#define TILES_MAX 16
#define TILES_SQUARED 4

#define WORLD_SIZE 64
#define WORLD_WALL 4

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
		unordered_map<string, shared_ptr<sf::SoundBuffer>>& soundbuffers,
		shared_ptr<Guy>& guy)
	:
		m_window(window),
		m_commands(commands),
		m_textures(textures),
		m_soundbuffers(soundbuffers),
		m_guy(guy)
	{

		m_rendertexture.create(ZOOM_W, ZOOM_H, false);

		m_view = m_rendertexture.getDefaultView();

		m_render_sprite.setTexture(m_rendertexture.getTexture());
		m_render_sprite.setScale( float(ZOOM), float(ZOOM) );

		m_sounds.emplace( "level_complete", make_shared<sf::Sound>( *m_soundbuffers.at("level_complete").get() ) );

		m_sounds.emplace( "button_red", make_shared<sf::Sound>( *m_soundbuffers.at("button_red").get() ) );
		m_sounds.emplace( "button_blue", make_shared<sf::Sound>( *m_soundbuffers.at("button_blue").get() ) );

		m_sounds.emplace( "jingle_0", make_shared<sf::Sound>( *m_soundbuffers.at("jingle_0").get() ) );
		m_sounds.emplace( "jingle_1", make_shared<sf::Sound>( *m_soundbuffers.at("jingle_1").get() ) );
		m_sounds.at("jingle_0")->setVolume(25);
		m_sounds.at("jingle_1")->setVolume(25);


		m_sounds.emplace( "step_0", make_shared<sf::Sound>( *m_soundbuffers.at("step_0").get() ) );
		m_sounds.emplace( "step_1", make_shared<sf::Sound>( *m_soundbuffers.at("step_1").get() ) );
		m_sounds.emplace( "step_2", make_shared<sf::Sound>( *m_soundbuffers.at("step_2").get() ) );
		m_sounds.emplace( "step_3", make_shared<sf::Sound>( *m_soundbuffers.at("step_3").get() ) );
		m_sounds.emplace( "step_4", make_shared<sf::Sound>( *m_soundbuffers.at("step_4").get() ) );
		m_sounds.emplace( "step_5", make_shared<sf::Sound>( *m_soundbuffers.at("step_5").get() ) );
		m_sounds.emplace( "step_6", make_shared<sf::Sound>( *m_soundbuffers.at("step_6").get() ) );
		m_sounds.emplace( "step_7", make_shared<sf::Sound>( *m_soundbuffers.at("step_7").get() ) );

		m_step_array[0] = m_sounds.at("step_0");
		m_step_array[1] = m_sounds.at("step_1");
		m_step_array[2] = m_sounds.at("step_2");
		m_step_array[3] = m_sounds.at("step_3");
		m_step_array[4] = m_sounds.at("step_4");
		m_step_array[5] = m_sounds.at("step_5");
		m_step_array[6] = m_sounds.at("step_6");
		m_step_array[7] = m_sounds.at("step_7");

		for ( auto& snd : m_step_array )
		{
			snd->setVolume(10);
		}

		m_sprites.emplace( "torch_icon", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );
		m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24,0, 32, 16) );
		m_sprites.at("torch_icon")->setOrigin(4,16);

		m_sprites.emplace( "torch", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );
		m_sprites.at("torch")->setTextureRect( sf::IntRect(8, 0, 8, 8) );
		m_sprites.at("torch")->setOrigin(4,4);

		m_sprites.emplace( "guy", make_shared<sf::Sprite>( *m_textures.at("guy").get() ) );
		m_sprites.at("guy")->setOrigin(2,2);

		m_sprites.emplace( "red_button", make_shared<sf::Sprite>( *m_textures.at("buttons").get() ) );
		m_sprites.at("red_button")->setTextureRect( sf::IntRect(0, 0, 8, 8) );

		m_sprites.emplace( "blue_button", make_shared<sf::Sprite>( *m_textures.at("buttons").get() ) );
		m_sprites.at("blue_button")->setTextureRect( sf::IntRect(16, 0, 8, 8) );

		m_sprites.emplace( "portal", make_shared<sf::Sprite>( *m_textures.at("portal").get() ) );
		m_sprites.at("portal")->setTextureRect( sf::IntRect(0, 0, 8, 8) );

		sf::Vector2f world_center = sf::Vector2f((WORLD_SIZE/2) * TILE_SIZE, (WORLD_SIZE/2) * TILE_SIZE);

		//m_guy->m_level = 3;

		switch ( m_guy->m_level )
		{
		case 0:
		{
			m_world.generate_room( world_center, 6, 5);

			m_guy->pos_portal      = world_center + sf::Vector2f( TILE_SIZE, -2*TILE_SIZE);
			m_guy->pos_button_blue = world_center + sf::Vector2f(  2*TILE_SIZE,    TILE_SIZE);
			m_guy->pos_button_red  = world_center + sf::Vector2f( -2*TILE_SIZE,    TILE_SIZE);

			break;
		}
		case 1:
		{
			m_world.generate_room( world_center, 6, 5);

			m_guy->pos_portal      = world_center + sf::Vector2f( TILE_SIZE, -2*TILE_SIZE);
			m_guy->pos_button_blue = world_center + sf::Vector2f(  2*TILE_SIZE,    TILE_SIZE);
			m_guy->pos_button_red  = world_center + sf::Vector2f( -8*TILE_SIZE,    TILE_SIZE);

			break;
		}
		case 2:
		{
			m_guy->pos_portal      = world_center + sf::Vector2f( TILE_SIZE, -2*TILE_SIZE);

			int rand_x = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);
			m_guy->pos_button_blue = world_center + sf::Vector2f( (rand_x+1)*TILE_SIZE, 1*TILE_SIZE);
			m_guy->pos_button_red  = world_center + sf::Vector2f( (rand_x-1)*TILE_SIZE, -1*TILE_SIZE);

			m_world.place_light_near( m_guy->pos_portal, -3 );

			m_world.place_light_near( m_guy->pos_button_blue, -3 );
			m_world.place_light_near( m_guy->pos_button_red, 3 );

			break;
		}
		case 3:
		case 4:
		{
			int _size   = 5 + rand()%6;
			int _offset = (rand()%8)-4;
			m_world.generate_room( world_center, _size+_offset, _size-_offset );

			m_guy->pos_portal      = world_center + sf::Vector2f( TILE_SIZE, -2*TILE_SIZE);
			m_guy->pos_button_blue = sf::Vector2f( ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE, ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE);
			m_guy->pos_button_red  = sf::Vector2f( ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE, ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE);

			m_world.place_light_near( m_guy->pos_button_blue, -3 );
			m_world.place_light_near( m_guy->pos_button_red, 3 );

			break;
		}

		default:
		{
			switch ( rand()%3 )
			{
			case 0:
				{
					m_world.generate_room( world_center, 4+(rand()%4), 4+(rand()%4) );
					break;
				}
			case 1:
				{
					int _size   = 5 + rand()%6;
					int _offset = (rand()%8)-4;
					m_world.generate_room( world_center, _size+_offset, _size-_offset );
					break;
				}
			case 2:
				{
					m_world.place_light_near( world_center, 5 );
					break;
				}
			}

			if (rand()%2)
			{
				int rand_x_r = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);
				int rand_y_r = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);
				int rand_x_b = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);
				int rand_y_b = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);

				int rand_x_p = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);
				int rand_y_p = (rand()%(WORLD_SIZE/4)) + (WORLD_SIZE/4);

				m_guy->pos_portal      = world_center + sf::Vector2f( (rand_x_p)*TILE_SIZE, (rand_y_p)*TILE_SIZE);
				m_guy->pos_button_blue = world_center + sf::Vector2f( (rand_x_b)*TILE_SIZE, (rand_y_b)*TILE_SIZE);
				m_guy->pos_button_red  = world_center + sf::Vector2f( (rand_x_r)*TILE_SIZE, (rand_y_r)*TILE_SIZE);
			}
			else
			{
				m_guy->pos_portal      = sf::Vector2f( ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE, ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE);
				m_guy->pos_button_blue = sf::Vector2f( ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE, ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE);
				m_guy->pos_button_red  = sf::Vector2f( ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE, ((rand()%(WORLD_SIZE-WORLD_WALL))+WORLD_WALL)*TILE_SIZE);
			}

			m_world.place_light_near( m_guy->pos_portal, 3 );
			m_world.place_light_near( m_guy->pos_button_blue, -3 );
			m_world.place_light_near( m_guy->pos_button_red, 3 );
			break;
		}
		}

		m_world.enforce_floor_at(world_center);

		m_world.enforce_floor_at(m_guy->pos_portal);
		m_world.enforce_floor_at(m_guy->pos_button_blue);
		m_world.enforce_floor_at(m_guy->pos_button_red);

		m_world.m_texture = m_textures.at("world");

		m_sprites.at("guy")->setPosition(world_center);
		m_sprites.at("red_button" )->setPosition(m_guy->pos_button_red);
		m_sprites.at("blue_button")->setPosition(m_guy->pos_button_blue);
		m_sprites.at("portal" )->setPosition(m_guy->pos_portal);

	};


  int update() // -1 = game over, 0 = in game, 1 = level completed
  {
  	if ( get_command(0, "quit") )
		{
			m_quit_on_release = true;
		}
		else if (m_quit_on_release)
		{
			return -1;
		}

		auto click_pos = sf::Mouse::getPosition(m_window);

		int ud = int( -get_command(0,"up")  ) + int( get_command(0,"down") );
		int lr = int( -get_command(0,"left")) + int( get_command(0,"right"));

		m_guy->m_velocity.x += lr * m_guy->m_accel;
		m_guy->m_velocity.y += ud * m_guy->m_accel;

		m_guy->m_velocity.x *= m_guy->m_friction;
		m_guy->m_velocity.y *= m_guy->m_friction;

		//m_sprites.at("guy")->setTextureRect( sf::IntRect((lr!=0)*4, 0, 4, 4) );

		auto pos = m_sprites.at("guy")->getPosition();

		if (!m_world.can_walk(pos)) return -1;

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

			if ( take)
			{
				if ( m_world.light_take(pos) )
				{
					// take torch if possible, otherwise this is like an extinguisher
					if ( m_guy->m_torch_pickup )
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
					m_guy->m_torches.pop_back();

					if (m_guy->m_torches.size() == 0) m_world.set_moving_torch(0, false,  pos);
				}

			}

			if ( take || place)
			{
				m_guy->m_velocity *= 0.5f;
			}

			if (m_guy->m_torches.size() > 0) m_world.set_moving_torch(0, true,  pos);


			float dir = atan2(m_guy->m_velocity.y, m_guy->m_velocity.x);
			float dira = fabs(dir);
			if (dira != dir) dira = M_PI-dira;
			m_guy->m_tex_rect.left = (int((dira/M_PI) * 4.5) * 4)%16;
			m_sprites.at("guy")->setTextureRect( m_guy->m_tex_rect );

			if (m_world.can_walk(m_guy->pos_portal))
			{
				if (m_guy->red_button_press && m_guy->blue_button_press &&
								m_world.validate(pos) == m_world.validate(m_guy->pos_portal) )
				{
					/// level complete
					m_sounds.at("level_complete")->play();

					while ( m_sounds.at("level_complete")->getStatus() == sf::SoundSource::Playing)
					{
					}

					return 1;
				}


				m_guy->draw_portal = true;
				m_sprites.at("portal")->setTextureRect( sf::IntRect(m_guy->red_button_press*8 + m_guy->blue_button_press*8, 0, 8, 8) );
			}

			if (m_world.can_walk(m_guy->pos_button_red))
			{
				if (m_world.validate(pos) == m_world.validate(m_guy->pos_button_red) )
				{

					if (!m_guy->red_button_press)
					{
						m_sounds.at("button_red")->play();
						m_guy->red_button_press = true;
					}
				}

				m_guy->draw_red_button = true;
				m_sprites.at("red_button")->setTextureRect( sf::IntRect(m_guy->red_button_press*8, 0, 8, 8) );
			}
			else
			{
				m_guy->draw_red_button = false;
			}

			if (m_world.can_walk(m_guy->pos_button_blue))
			{
				if (m_world.validate(pos) == m_world.validate(m_guy->pos_button_blue) )
				{
					if (!m_guy->blue_button_press)
					{
						m_sounds.at("button_blue")->play();
						m_guy->blue_button_press = true;
					}
				}

				m_guy->draw_blue_button = true;
				m_sprites.at("blue_button")->setTextureRect( sf::IntRect(16 + (m_guy->blue_button_press*8), 0, 8, 8) );
			}
			else
			{
				m_guy->draw_blue_button = false;
			}

		}


		float spd = sqrt((m_guy->m_velocity.x * m_guy->m_velocity.x) + (m_guy->m_velocity.y * m_guy->m_velocity.y));
		m_step_delay -= spd;
		if (m_step_delay < 0.0f)
		{
			m_step_array[rand()%8]->play();

			m_step_delay = m_step_delay_start;
		}

		//m_particles.update();
		//m_sectors.update();
		m_world.update();
		m_world.assign_tiles();

		return 0;
  };

  void render()
  {
		m_rendertexture.setView(m_view);
		m_rendertexture.clear(sf::Color(8,8,12));

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

		if (m_guy->draw_red_button)
		{
			m_rendertexture.draw( *m_sprites.at("red_button").get() );
		}

		if (m_guy->draw_blue_button)
		{
			m_rendertexture.draw( *m_sprites.at("blue_button").get() );
		}
		if (m_guy->draw_portal)
		{
			m_rendertexture.draw( *m_sprites.at("portal").get() );
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
	unordered_map<string, shared_ptr<sf::SoundBuffer>>& m_soundbuffers;


	sf::Sprite m_render_sprite;
	sf::RenderTexture m_rendertexture;
	sf::View m_view;

	unordered_map<string, shared_ptr<sf::Sprite>> m_sprites;
	unordered_map<string, shared_ptr<sf::Sound>> m_sounds;

	array<shared_ptr<sf::Sound>, 8> m_step_array;


	float m_step_delay_start = 5.0f;
	float m_step_delay = m_step_delay_start/2;

	shared_ptr<Guy> m_guy;

	//Particles<10000> m_particles;
	//Sectors<GRID_N_W,GRID_N_H> m_sectors;

	World m_world;

	bool m_quit_on_release = false;

};

#endif
