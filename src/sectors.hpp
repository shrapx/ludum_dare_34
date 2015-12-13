#ifndef LD34_SECTORS_HPP_
#define LD34_SECTORS_HPP_

#define TRI_RADIANS 2.0943951023931953

struct Sector
{
	//sf::Vector2f m_pos;
	//sf::Vector2f m_vel;


	sf::Vector2f m_pos_global;
	sf::Vector2f m_pos;
	sf::Vector2f m_pos_tile;

	bool active = true;

	float        m_base_size = 5.0f;
	float        m_size;
	float        m_ang  = 0.0f;
	float        m_rot  = 0.0f;
	float        m_heat = 0.0f;
	float        m_hue;
	sf::Color    m_col;
	sf::Vector2i m_tex;

	Sector()
	{
		//m_pos.x = ((rand()%100)-50) * 0.01f;
		//m_pos.y = ((rand()%100)-50) * 0.01f;
		//m_base_size = ((rand() % 100) * 0.01f);
		m_ang = float(rand()) * 0.001f;
		//m_rot = ((rand()%100)-50) * 0.005f;

		m_hue = (1.0f/255)*(rand()%255);
		update();

		//m_tex = sf::Vector2i(rand()%4, rand()%4);
	}

	void update()
	{
		m_size = m_base_size * m_heat;

		m_col = hsl( m_hue, min(1.0f, m_heat), 0.5f, 1.0f);

		// friction / cooling

		set_pos( m_pos * 0.94f );
		m_rot *= 0.94f;
		m_heat *= 0.94f;
	}

	void set_pos( sf::Vector2f ipos )
	{
		//active = true;

		//m_pos.x = (ipos.x*2) - 1.0f;
		//m_pos.y = (ipos.y*2) - 1.0f;

		m_pos.x = ipos.x;
		m_pos.y = ipos.y;

		m_pos_tile.x = GRID_W * (1.0f+m_pos.x) * 0.5f;
		m_pos_tile.y = GRID_H * (1.0f+m_pos.y) * 0.5f;
	}

};

template<size_t size_width, size_t size_height>
class Sectors : public sf::Drawable
{

public:

	shared_ptr<sf::Texture> m_texture_bg;
	shared_ptr<sf::Texture> m_texture_active;

	array<array<Sector,size_height>,size_width> m_array;

	sf::VertexArray m_vertices;

	array<sf::Vector2i, 9> kernel_locs = {{
		{-1,-1}, {-1, 0}, {-1,+1},
		{ 0,-1}, { 0, 0}, { 0,+1},
		{+1,-1}, {+1, 0}, {+1,+1} }};

	array<float, 9> kernel = {{
		{0.015f}, {0.11f}, {0.015f},
		{0.11f},  {0.5f},  {0.11f},
		{0.015f}, {0.11f}, {0.015f} }};

	sf::Vector2f pos_last;

	Sectors() : m_vertices(sf::Triangles, size_width * size_height * 3)
	{

		for ( int w=0; w < size_width; ++w )
			for ( int h=0; h < size_height; ++h )
			{
				//auto& sect = m_array[w][h];

				//sect.m_rot = 0.0025f;
				//sect.m_rot = 0.0f;
				//sect.m_ang = w%2 == 0 ? 0.0f : 3.1415f;
			}

	}

	void apply_force( float px, float py, float distance, float factor )
	{
		auto pos = sf::Vector2f(px,py);
		auto difference = pos - pos_last;
		pos_last = pos;

		float speed = sqrt((difference.x * difference.x) + (difference.y * difference.y));

		int w_start = max( 0.0f,              (px - distance) / GRID_W );
		int w_end   = min( float(size_width), (px + distance) / GRID_W );

		int h_start = max( 0.0f,               (py - distance) / GRID_W );
		int h_end   = min( float(size_height), (py + distance) / GRID_W );

		for ( int w=w_start; w < w_end; ++w )
		{
			for ( int h=h_start; h < h_end; ++h )
			{

				auto& sect = m_array[w][h];

				sf::Vector2f sect_pos = get_pos( w, h);

				sf::Vector2f pos2;
				pos2.x = px - sect_pos.x;
				pos2.y = py - sect_pos.y;
				//pos2.x = sect_pos.x - px;
				//pos2.y = sect_pos.y - py;



				float length = (distance - sqrt((pos2.x * pos2.x) + (pos2.y * pos2.y)) ) / distance;
				if (length < 1.0f && length > 0.0f)
				{
					sf::Vector2f mouse = normalize(pos);
					sf::Vector2f pos3 = normalize(pos2);

					float dot = (mouse.x * pos3.x) + (mouse.y * pos3.y);
					//float ata = atan2(pos2.x, pos2.y) < 0.0f ? 1.0f : -1.0f;
					//sect.m_ang = ata;
					sect.m_rot += -dot * speed * length * factor * 0.0001f;
					//sect.m_rot += dot * speed * 0.0001f;

					sect.m_heat += speed * length * factor * 0.025f;
				}


				//sect.m_rot += atan2(pos2.y, pos2.x) * (length/distance) * 0.001f;
				if (w == 20 && h == 20)
				{
					cout << length << endl;
				}
			}
		}
	}

	void update()
	{
		for ( int w=0; w < size_width; ++w )
		{
			for ( int h=0; h < size_height; ++h )
			{
				//transfer(w, h);
			}
		}

		for ( int w=0; w < size_width; ++w )
		{
			for ( int h=0; h < size_height; ++h )
			{
				m_array[w][h].update();
				update_vertices(w, h);
			}
		}
	}

	sf::Vector2f get_pos(size_t w, size_t h)
	{
		return sf::Vector2f( w*GRID_W + (GRID_W* (h%2==0?1.0f:0.5f) ), h*GRID_H + (GRID_H*0.5f) );
		//return sf::Vector2f( w*GRID_W + (GRID_W*0.5f), h*GRID_H + (GRID_H*0.5f) );

	}

	void update_vertices(int w, int h)
	{
		//bool offset = h%2==0;
		//sf::Vector2f pos = sf::Vector2f( w*GRID_W + (offset?GRID_W*1.0f:0.5f), h*GRID_H + (GRID_H*0.5f) );
		//sf::Vector2f pos = sf::Vector2f( w*GRID_W + (GRID_W*0.5f), h*GRID_H + (offset?GRID_H*1.0f:0.5f) );
		sf::Vector2f pos = get_pos( w, h);

		auto& sect = m_array[w][h];
		sect.m_ang += sect.m_rot * sect.m_size;

		m_vertices[ (w + (h*size_width))*3 + 0 ].position = pos + sect.m_pos_tile + sf::Vector2f( sin(sect.m_ang-TRI_RADIANS) * sect.m_size, cos(sect.m_ang-TRI_RADIANS) * sect.m_size );
		m_vertices[ (w + (h*size_width))*3 + 1 ].position = pos + sect.m_pos_tile + sf::Vector2f( sin(sect.m_ang            ) * sect.m_size, cos(sect.m_ang            ) * sect.m_size );
		m_vertices[ (w + (h*size_width))*3 + 2 ].position = pos + sect.m_pos_tile + sf::Vector2f( sin(sect.m_ang+TRI_RADIANS) * sect.m_size, cos(sect.m_ang+TRI_RADIANS) * sect.m_size );

		m_vertices[ (w + (h*size_width))*3 + 0 ].color = sect.m_col;
		m_vertices[ (w + (h*size_width))*3 + 1 ].color = sect.m_col;
		m_vertices[ (w + (h*size_width))*3 + 2 ].color = sect.m_col;
	}

	void transfer(int w, int h)
	{
		if (w > 0 && w < size_width-1 && h > 0 && h < size_height-1)
		{
			auto& sect = m_array[w][h];

			auto pos_start = sect.m_pos;

			auto sum = sf::Vector2f(0,0);

			float ang_rot_sum = 0.0f;

			for ( int z=0; z < 9; ++z)
			{
				int ow = w + kernel_locs[z].x;
				int oh = h + kernel_locs[z].y;

				auto& other = m_array[ow][oh];

				float size_amt = 1.0f;
				float rot_amt = 1.0f;

				if ( (z != 4) && (sect.m_size > 0.0f) )
				{
					size_amt = other.m_size / sect.m_size;
					rot_amt = fabs(other.m_rot) - fabs(sect.m_rot);
				}

				sum += other.m_pos * size_amt * kernel[z];

				//ang_rot_sum += other.m_rot * rot_amt * kernel[z];
				ang_rot_sum += rot_amt * kernel[z];
			}

			sum.x = min( 1.0f, max( -1.0f, sum.x));
			sum.y = min( 1.0f, max( -1.0f, sum.y));

			sect.set_pos( sum );

			sect.m_rot = min( 1.7f, max( -1.7f, ang_rot_sum));
			sect.m_ang += sect.m_rot;

			//auto source = sum-pos_start;
			//float length = sqrt((source.x * source.x) + (source.y * source.y));
			//sect.m_rot += length;
		}
	}


private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		//states.texture = m_texture.get();

		/*
		sf::Sprite sprite;
		sprite.setTexture(*m_texture_bg.get());

		sf::Sprite sprite2;
		sprite2.setTexture(*m_texture_active.get());

		for ( int w=0; w < size_width; ++w )
			for ( int h=0; h < size_height; ++h )
			{
				sf::Vector2f pos = sf::Vector2f(w*GRID_W, h*GRID_H);

				sprite.setPosition( pos );
				sprite.setColor( m_pos[w][h].m_col );
				//sprite.setTextureRect( sf::IntRect( GRID_W, GRID_H, m_pos[w][h].m_tex_x, m_pos[w][h].m_tex_y ) );
				sprite.setTextureRect( sf::IntRect( GRID_H * m_pos[w][h].m_tex_x, GRID_W * m_pos[w][h].m_tex_y, GRID_W, GRID_H ) );
				target.draw( sprite, states );

				// draw pixel
				sprite2.setPosition( pos + m_pos[w][h].m_pos_tile );
				sprite2.setColor( m_pos[w][h].m_col );
				target.draw( sprite2, states );
			}
		*/

		target.draw(m_vertices, states );
	}
};

#endif
