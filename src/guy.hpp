#ifndef GUY_HPP_
#define GUY_HPP_

#include <vector>


class Guy
{
public:
	std::vector<float> m_torches;
	float m_torch_quantity_start = 0.0f;
	float m_torch_default = 30.0f;
	float num_torches = 0;
	float m_accel = 0.035f;
	float m_friction = 0.93f;
	sf::Vector2f m_velocity = sf::Vector2f(0.0f, 0.0f);
	sf::IntRect m_tex_rect;
	int m_type = 0;

	Guy(int type = 0) : m_type(type)
	{
		m_tex_rect = sf::IntRect( 0, (m_type)*4, 4, 4);

		switch (m_type)
		{
		case 0:
			m_torches.resize(m_torch_quantity_start,m_torch_default);
			break;
		case 1:
			m_torch_quantity_start = 6;
			m_torch_default = 10.0f;
			m_torches.resize(m_torch_quantity_start,m_torch_default);
			m_accel = 0.025f;
			m_friction = 0.96f;
			break;
		case 2:
			m_torch_quantity_start=4;
			m_torch_default = 25.0f;
			m_torches.resize(m_torch_quantity_start,m_torch_default);
			break;
		case 3:

			m_torch_quantity_start=1;
			m_torch_default = 0.0f;
			m_torches.resize(m_torch_quantity_start,130.0f);
			m_accel = 0.03f;
			m_friction = 0.97f;
			break;
		}
	}

};

#endif
