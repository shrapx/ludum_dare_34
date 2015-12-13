#ifndef LD34_COLOUR_HPP_
#define LD34_COLOUR_HPP_

float hue_rgb_( float v1,float v2,float vH )
{
	 if ( vH < 0 ) vH += 1;
	 if ( vH > 1 ) vH -= 1;
	 if ((6.0f*vH) < 1) return v1 + (v2-v1) * 6.0f * vH;
	 if ((2.0f*vH) < 1) return v2;
	 if ((3.0f*vH) < 2) return v1 + (v2-v1) * ((2.0f/3.0f) - vH) * 6.0f;
	 return ( v1 );
}

sf::Color hsl(float H, float S, float L, float A)
{
	sf::Color color;

	if ( S == 0 ) //HSL from 0 to 1
	{
		color.r = L * 255;
		color.g = L * 255;
		color.b = L * 255;
	}
	else
	{
		float var_1,var_2;

		if ( L < 0.5 )
			var_2 = L * ( 1 + S );
		else
			var_2 = ( L + S ) - ( S * L );

		var_1 = 2 * L - var_2;

		color.r = 255 * hue_rgb_( var_1, var_2, H + ( 1.0f / 3.0f ) );
		color.g = 255 * hue_rgb_( var_1, var_2, H );
		color.b = 255 * hue_rgb_( var_1, var_2, H - ( 1.0f / 3.0f ) );
	}
	color.a = A * 255;
	return color;
}

sf::Color rand_hue(float S=0.8f, float L=0.5f, float A=1.0f)
{
	float v = (1.0f/255)*(rand()%255);

	return hsl(v,S,L,A);
}

sf::Color rand_gray()
{
	unsigned int v = 64+rand()%192;
	return sf::Color(v,v,v,127);
}

#endif
