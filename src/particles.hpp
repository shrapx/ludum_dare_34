#ifndef LD34_PARTICLES_HPP_
#define LD34_PARTICLES_HPP_

template <int max_particles>
class Particles : public sf::Drawable
{
public:

	Particles() {}

	shared_ptr<sf::Texture> m_texture;

	array<sf::Vector2f,max_particles> m_pos;
	array<sf::Vector2f,max_particles> m_vel;
	array<sf::Color,max_particles> m_col;
	array<float,max_particles> m_hue;
	array<float,max_particles> m_size;

	map<int, set<size_t> > sectors;

	size_t m_num = 0;

	void update()
	{
		for ( int i=0; i < m_num; ++i )
		{

			int cmp_num = 0;
			sf::Vector2f net_movement;
			net_movement.x = 0;
			net_movement.y = 0;

			for ( int j=0; j < m_num; ++j )
			{
				if (j != i || cmp_num < 100)
				{

					float hue_diff = m_hue[i] - m_hue[j];

					if (hue_diff < 0.0f)
						hue_diff = hue_diff + 1.0f;

//					hue_diff = (hue_diff / 127.5f) - 1.0f;
					hue_diff = (hue_diff*2.0f) - 1.0f;

					if (hue_diff < 0.5f || hue_diff > -0.5f)
					{
						sf::Vector2f dir = normalize( m_pos[i] - m_pos[j] ) * hue_diff;
						net_movement += dir;
					}

					cmp_num++;
				}

			}

			if (cmp_num == 0) continue;

			net_movement.x /= cmp_num;
			net_movement.y /= cmp_num;

			m_pos[i] += net_movement + net_movement;
			//m_pos[i] += m_vel[i];
		}
	}

	void add()
	{
		if (m_num < max_particles)
		{
			m_pos[m_num].x = rand() % 200;
			m_pos[m_num].y = rand() % 150;

			//int sector = int(m_pos[m_num].x/200) + int(m_pos[m_num].y/150) * 20);
			//m_sector[m_num] = sector;
			//m_sectors[sector].insert( m_num

			m_vel[m_num].x = (rand() % 100) * 0.01f;
			m_vel[m_num].y = (rand() % 100) * 0.01f;
			m_size[m_num] = 0.25f + ((rand() % 100) * 0.0075f);

			//m_hue[m_num] = (rand() % 100) * 0.01f;
			m_hue[m_num] = (1.0f/255)*(rand()%255);

			m_col[m_num] = hsl( m_hue[m_num], 1.0f, 0.8f,1.0f);

			m_num++;
		}
	}

	void rem()
	{
		if (m_num > 0)
			m_num--;
	}

	void wake(size_t id)
	{
		_swap(id, m_num);
		m_num++;
	}

	void kill(size_t id)
	{
		_swap(id, m_num - 1);
		m_num--;
	}

	void _swap( size_t a, size_t b )
	{
		std::swap( m_pos[a], m_pos[b] );
		std::swap( m_vel[a], m_vel[b] );
		std::swap( m_hue[a], m_hue[b] );
		std::swap( m_size[a], m_size[b] );
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.texture = m_texture.get();

		sf::Sprite sprite;
		sprite.setTexture(*m_texture.get());

		for ( int i=0; i < m_num; ++i )
		{
			sprite.setPosition( m_pos[i] );
			sprite.setScale( m_size[i], m_size[i] );

			sprite.setColor( m_col[i] );

			target.draw( sprite, states );
		}
	}

};

#endif
