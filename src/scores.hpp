#ifndef SCORES_HPP_
#define SCORES_HPP_

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

#include "guy.hpp"

#include "colour.hpp"

using namespace std;
using namespace Json;

class Scores
{
public:

	Scores(
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


		m_sprites.emplace( "score_bg", make_shared<sf::Sprite>( *m_textures.at("score_bg").get() ) );

		m_sprites.emplace( "score_stick_x", make_shared<sf::Sprite>( *m_textures.at("score_sticks").get() ) );
		m_sprites.at("score_stick_x")->setTextureRect( sf::IntRect(0,0, 8, 16) );
		m_sprites.at("score_stick_x")->setOrigin(4,8);

		m_sprites.emplace( "score_stick_y", make_shared<sf::Sprite>( *m_textures.at("score_sticks").get() ) );
		m_sprites.at("score_stick_y")->setTextureRect( sf::IntRect(8,0, 16, 8) );
		m_sprites.at("score_stick_y")->setOrigin(8,4);

		m_sprites.emplace( "torch_icon", make_shared<sf::Sprite>( *m_textures.at("torch").get() ) );
		m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24,0, 32, 16) );
		m_sprites.at("torch_icon")->setOrigin(4,16);

		m_sprites.emplace( "level_star", make_shared<sf::Sprite>( *m_textures.at("particle").get() ) );
	};


  int update()
  {

		if ( get_command(0, "quit") )
		{
			m_quit_on_release = true;
		}
		else if (m_quit_on_release)
		{
			return -1;
		}

		if ( get_command(0, "restart") )
		{
			m_restart_on_release = true;
		}
		else if (m_restart_on_release)
		{
			return 1;
		}

  	return  0;
  };

  void render()
  {
		m_rendertexture.setView(m_view);
		m_rendertexture.clear(sf::Color(8,8,12));

		// bg

		m_rendertexture.draw( *m_sprites.at( "score_bg").get() );

		/// ui

		const sf::Vector2f& cc = m_view.getCenter();

		//testing
		//m_guy->m_level = 67;

		for ( int i=0; i< m_guy->m_level-1; ++i )
		{
			//m_sprites.at("level_star")->setPosition(cc.x  + (8*i), cc.y);
			//m_rendertexture.draw( *m_sprites.at("level_star").get() );

			int ind_x = (i%5)*4;

			int grp5_x = ((i/5)%4) * 19;
			int grp5_y = ((i/5)/4) * 19;

			bool is_ho = (i%5)!=4;
			if ( is_ho )
			{
				m_sprites.at("score_stick_x")->setPosition(cc.x + ind_x + grp5_x, cc.y - (ZOOM_H*0.5f) + 30 + grp5_y);
				m_rendertexture.draw( *m_sprites.at("score_stick_x").get() );
			}
			else
			{
				m_sprites.at("score_stick_y")->setPosition(cc.x + grp5_x + 4, cc.y - (ZOOM_H*0.5f) + 30 + grp5_y);
				m_rendertexture.draw( *m_sprites.at("score_stick_y").get() );
			}
		}

		/*
		for ( int i=0; i< m_guy->m_torches.size(); ++i )
		{
			if (i == m_guy->m_torches.size()-1)
			{
				int height = (m_guy->m_torches[i] / m_guy->m_torch_default) * 16;
				m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24, 16-height, 32, height));
				m_sprites.at("torch_icon")->setPosition( (cc.x  + (ZOOM_W*0.5f)) - 8 - (8*i), (cc.y + (ZOOM_H*0.5f)) - 8 + (16-height));
			}
			else
			{
				m_sprites.at("torch_icon")->setTextureRect( sf::IntRect(24,0, 32, 16));
				m_sprites.at("torch_icon")->setPosition( (cc.x  + (ZOOM_W*0.5f)) - 8 - (8*i), (cc.y + (ZOOM_H*0.5f)) - 8);
			}

			m_rendertexture.draw( *m_sprites.at("torch_icon").get() );
		}*/

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

	shared_ptr<Guy> m_guy;

	bool m_quit_on_release = false;
	bool m_restart_on_release = false;

};

#endif
