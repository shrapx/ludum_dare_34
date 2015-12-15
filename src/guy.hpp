#ifndef GUY_HPP_
#define GUY_HPP_

#include <vector>


class Guy
{
public:
	std::vector<float> m_torches;
	bool m_torch_pickup;
	float m_torch_quantity_start;
	float m_torch_default;
	float m_accel;
	float m_friction;
	sf::Vector2f m_velocity = sf::Vector2f(0.0f, 0.0f);
	sf::IntRect m_tex_rect;
	int m_type = 0;
	int m_level = 0;

	bool draw_portal = false;
	bool draw_red_button = false;
	bool draw_blue_button = false;

	bool red_button_press = false;
	bool blue_button_press = false;

	sf::Vector2f pos_button_red;
	sf::Vector2f pos_button_blue;
	sf::Vector2f pos_portal;

	Guy(int type = 0) : m_type(type)
	{
		m_tex_rect = sf::IntRect( 0, (m_type)*4, 4, 4);

		switch (m_type)
		{

		case 0:
			m_torch_pickup = true;
			m_torch_quantity_start = 0;
			m_torch_default = 30.0f;
			m_torches.resize(m_torch_quantity_start,m_torch_default);

			m_accel = 0.055f;
			m_friction = 0.92f;
			break;

		case 1:
			m_torch_pickup = true;
			m_torch_quantity_start = 3;
			m_torch_default = 15.0f;
			m_torches.resize(m_torch_quantity_start,m_torch_default);

			m_accel = 0.025f;
			m_friction = 0.96f;
			break;

		case 2:
			m_torch_pickup = true;
			m_torch_quantity_start=5;
			m_torch_default = 25.0f;
			m_torches.resize(m_torch_quantity_start,m_torch_default);

			m_accel = 0.035f;
			m_friction = 0.93f;
			break;

		case 3:
			m_torch_pickup = true;
			m_torch_quantity_start=20;
			m_torch_default = 0.0f;
			m_torches.resize(m_torch_quantity_start, 10.0f);

			m_accel = 0.05f;
			m_friction = 0.98f;
			break;
		}
	}

	void level_restart()
	{
		draw_portal = false;
		draw_red_button = false;
		draw_blue_button = false;
		red_button_press = false;
		blue_button_press = false;
	}
};

#endif
