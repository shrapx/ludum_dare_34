#ifndef LD34_WORLD_HPP_
#define LD34_WORLD_HPP_

struct Tile
{
	bool active = false;
	bool m_assign_type;

	bool m_force_fixed = false;

	bool placed_light = false;
	int walking_light = 0;

	float distance_to_light = LIGHT_DISTANCE_OFF;

	bool is_wall = false;

	int        m_type;

	float      m_hue;
	sf::Color  m_col;

	Tile()
	{
		m_hue = (1.0f/255)*(rand()%255);
	}

	void update()
	{
		bool in_range = distance_to_light < LIGHT_DISTANCE;
		if (in_range)
		{
			if ( !active )
			{
				active = true;

				if (!m_force_fixed) m_assign_type = true;
			}
			else
			{
				m_assign_type = false;
			}
		}
		else
		{
			active = false;
		}
	}

	void assign_type( int a_type = -1 )
	{
		if (!m_assign_type) return;

		if (a_type != -1)
			m_type = a_type;
		else
			m_type = rand()%TILES_MAX;

		is_wall = m_type < 4;
	}

	void set_random_wall()
	{
		active = true;
		m_type = rand()%4;
		is_wall = true;
	}

	void set_random_floor()
	{
		active = true;
		m_type = 4 + (rand()%4);
		is_wall = false;
	}
};

struct MovingTorches
{
	bool active = false;
	bool active_prev = false;
	sf::Vector2i loc;
	sf::Vector2i loc_prev;
};

class World : public sf::Drawable
{

public:

	size_t m_world_size;

	shared_ptr<sf::Texture> m_texture;

	array<array<Tile,WORLD_SIZE>,WORLD_SIZE> m_array;

	array<MovingTorches, 16> m_lights_that_move;

	vector<sf::Vector2f> m_light_positions;
	vector<sf::Vector2f> m_light_positions_placed;

	void generate_room( sf::Vector2f pos, int halfwidth, int halfheight )
	{
		auto ipos = validate( pos );
		return generate_room(ipos, halfwidth, halfheight );
	}

	void generate_room( sf::Vector2i pos, int halfwidth, int halfheight )
	{
		int w_start = max( int(0),          (pos.x - halfwidth) );
		int w_end   = min( int(WORLD_SIZE), (pos.x + halfwidth) );

		int h_start = max( int(0),           (pos.y - halfheight) );
		int h_end   = min( int(WORLD_SIZE), (pos.y + halfheight) );

		for ( int w=w_start; w < w_end; ++w )
		{
			for ( int h=h_start; h < h_end; ++h )
			{
				bool is_corner = ( w==w_start+1   && h==h_start+1 ) || (w==w_end-2 && h==h_end-2)
											|| ( w==w_end-2 && h==h_start+1 ) || (w==w_start+1   && h==h_end-2);
				bool is_edge = w==w_start || w==w_end-1 || h==h_start || h==h_end-1;

				Tile& tile = m_array[w][h];

				tile.active = true;

				if (is_corner && light_can_place(sf::Vector2i( w, h ) ))
				{
					tile.placed_light = true;
				}

				if (is_edge)
				{
					tile.set_random_wall();
				}
				else
				{
					tile.set_random_floor();
				}
			}
		}
	}

	sf::Vector2i validate( sf::Vector2f pos )
	{
		return sf::Vector2i(
			std::max( size_t(0), std::min( size_t(WORLD_SIZE),  size_t(pos.x / TILE_SIZE))),
			std::max( size_t(0), std::min( size_t(WORLD_SIZE), size_t(pos.y / TILE_SIZE))));
	}

	void enforce_floor_at( sf::Vector2f pos )
	{
		auto ipos = validate( pos );
		return enforce_floor_at(ipos);
	}

	void enforce_floor_at( sf::Vector2i pos )
	{

		Tile& tile = m_array[pos.x][pos.y];

		tile.set_random_floor();

		tile.m_force_fixed = true;
	}

	void place_light_near( sf::Vector2f pos, int distance )
	{
		auto ipos = validate( pos );
		return place_light_near(ipos, distance);
	}

	void place_light_near( sf::Vector2i pos, int distance )
	{

		Tile& tile = m_array[pos.x + distance][pos.y];

		tile.set_random_floor();

		tile.placed_light = true;

		tile.m_force_fixed = true;
	}

	bool can_walk( sf::Vector2f pos )
	{
		auto ipos = validate( pos );
		return can_walk(ipos);
	}
	bool light_place( sf::Vector2f pos )
	{
		auto ipos = validate( pos );
		return light_place(ipos);
	}
	bool light_take( sf::Vector2f pos )
	{
		auto ipos = validate( pos );
		return light_take(ipos);
	}

	void set_moving_torch(int id, bool active, sf::Vector2f pos )
	{
		auto ipos = validate( pos );
		set_moving_torch(id, active, ipos);
	}

	bool can_walk( sf::Vector2i pos )
	{
		Tile& tile = m_array[pos.x][pos.y];
		return (tile.active && !tile.is_wall) ;
	}

	bool light_can_place(sf::Vector2i pos)
	{
		if ( pos.x < WORLD_WALL || pos.x > (WORLD_SIZE-WORLD_WALL)
			|| pos.y < WORLD_WALL || pos.y > (WORLD_SIZE-WORLD_WALL) )
			return false;
		return true;
	}

	bool light_place( sf::Vector2i pos )
	{
		// place only if its not near the edge, forced darkness around the edge
		if ( !light_can_place(pos) ) return false;

		Tile& tile = m_array[pos.x][pos.y];

		if ( !tile.placed_light )
		{
			tile.placed_light = true;
			return true;
		}
		return false;
	}

	bool light_take( sf::Vector2i pos )
	{
		Tile& tile = m_array[pos.x][pos.y];

		if ( tile.placed_light )
		{
			tile.placed_light = false;

			return true;
		}
		else
			return false;
	}

	bool light_walking_add( sf::Vector2i pos )
	{
		// place only if its not near the edge, forced darkness around the edge
		if ( !light_can_place(pos) ) return false;

		Tile& tile = m_array[pos.x][pos.y];

		tile.walking_light++;
		return true;
	}

	bool light_walking_remove( sf::Vector2i pos )
	{
		Tile& tile = m_array[pos.x][pos.y];

		if (tile.walking_light > 0);
		{
			tile.walking_light--;
			return true;
		}
		return false;
	}

	void set_moving_torch( int id, bool active, sf::Vector2i pos )
	{
		m_lights_that_move[id].active = active;
		m_lights_that_move[id].loc = pos;
	}

	void update_light_distances()
	{
		m_light_positions.clear();
		m_light_positions_placed.clear();

		for ( int w=0; w < WORLD_SIZE; ++w )
		{
			for ( int h=0; h < WORLD_SIZE; ++h )
			{
				m_array[w][h].distance_to_light = LIGHT_DISTANCE_OFF;

				if (m_array[w][h].placed_light)
				{
					m_light_positions_placed.emplace_back( get_pos(w, h) );
				}
				if (m_array[w][h].placed_light || (m_array[w][h].walking_light > 0) )
				{
					m_light_positions.emplace_back( get_pos(w, h) );
				}
			}
		}

		for ( auto& light : m_light_positions )
		{
			for ( int w=0; w < WORLD_SIZE; ++w )
			{
				for ( int h=0; h < WORLD_SIZE; ++h )
				{
					auto pos_tile = get_pos(w, h);
					auto pos_diff = light - pos_tile;

					float dist = sqrt((pos_diff.x * pos_diff.x) + (pos_diff.y * pos_diff.y));

					Tile& tile = m_array[w][h];
					tile.distance_to_light = min( dist, tile.distance_to_light );

					float lighting = (LIGHT_DISTANCE - tile.distance_to_light) / LIGHT_DISTANCE;
					tile.m_col = hsl( tile.m_hue, 0.125f, lighting, 1.0f);
				}
			}
		}
	}

	void update()
	{
		// handling moving torches
		for ( auto& torch : m_lights_that_move )
		{
			if ( torch.loc != torch.loc_prev);
			{
				if (torch.active_prev) light_walking_remove(torch.loc_prev);

				if (torch.active)      light_walking_add(torch.loc);

				torch.loc_prev = torch.loc;
				torch.active_prev = torch.active;
			}
		}

		update_light_distances();

		for ( int w=0; w < WORLD_SIZE; ++w )
		{
			for ( int h=0; h < WORLD_SIZE; ++h )
			{
					m_array[w][h].update();
			}
		}
	}

	void assign_tiles()
	{
		for ( int w=0; w < WORLD_SIZE; ++w )
		{
			bool edge_case_w = (w < WORLD_WALL || w > (WORLD_SIZE-WORLD_WALL));

			for ( int h=0; h < WORLD_SIZE; ++h )
			{
				bool edge_case_h = (h < WORLD_WALL || h > (WORLD_SIZE-WORLD_WALL));

				// set wall always
				if ( edge_case_w || edge_case_h )
				{
					m_array[w][h].assign_type(0);
				}
				else
				{
					m_array[w][h].assign_type();
				}
			}
		}
	}

	sf::Vector2f get_pos(size_t w, size_t h)
	{
		return sf::Vector2f( w*TILE_SIZE + (TILE_SIZE*0.5f), h*TILE_SIZE + (TILE_SIZE*0.5f) );
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		sf::Sprite sprite;
		sprite.setTexture(*m_texture.get());

		for ( int w=0; w < WORLD_SIZE; ++w )
		{
			for ( int h=0; h < WORLD_SIZE; ++h )
			{
				const Tile& tile = m_array[w][h];
				if (tile.active)
				{
					sf::Vector2f pos = sf::Vector2f(w*TILE_SIZE, h*TILE_SIZE);

					sprite.setPosition( pos );

					int m_tex_x = m_array[w][h].m_type % TILES_SQUARED;
					int m_tex_y = m_array[w][h].m_type / TILES_SQUARED;

					sprite.setColor( tile.m_col );

					sprite.setTextureRect( sf::IntRect( TILE_SIZE * m_tex_x, TILE_SIZE * m_tex_y, TILE_SIZE, TILE_SIZE ) );
					target.draw( sprite, states );
				}
			}
		}
	}
};

#endif
