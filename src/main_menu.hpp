#ifndef MENU_HPP_
#define MENU_HPP_

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

class Menu
{
public:

	Menu(
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


		m_sprites.emplace( "menu_bg", make_shared<sf::Sprite>( *m_textures.at("menu_bg").get() ) );

		m_sprites.emplace( "guy1", make_shared<sf::Sprite>( *m_textures.at("menu_guys").get() ) );
		m_sprites.at("guy1")->setTextureRect( sf::IntRect(0, 0, 16, 16) );
		m_sprites.emplace( "guy2", make_shared<sf::Sprite>( *m_textures.at("menu_guys").get() ) );
		m_sprites.at("guy2")->setTextureRect( sf::IntRect(16, 0, 16, 16) );
		m_sprites.emplace( "guy3", make_shared<sf::Sprite>( *m_textures.at("menu_guys").get() ) );
		m_sprites.at("guy3")->setTextureRect( sf::IntRect(32, 0, 16, 16) );
		m_sprites.emplace( "guy4", make_shared<sf::Sprite>( *m_textures.at("menu_guys").get() ) );
		m_sprites.at("guy4")->setTextureRect( sf::IntRect(48, 0, 16, 16) );
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
			m_start_on_release = true;
		}
		else if (m_start_on_release)
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

		m_rendertexture.draw( *m_sprites.at( "menu_bg").get() );

		/// ui

		const sf::Vector2f& cc = m_view.getCenter();

		//
		//

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
	bool m_start_on_release = false;

};

#endif
