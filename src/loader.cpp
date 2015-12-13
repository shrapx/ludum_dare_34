/*
 * Loader.cpp
 *
 *  Created on: 21 Oct 2015
 *      Author: shrapx
 */

#include "loader.hpp"

int Loader::load(const std::string& filename, Json::Value& content)
{
	std::cout << "Loader: " << filename << std::endl;

	std::ifstream in_file(filename.c_str());
	if ( in_file.fail() )
	{
		std::cout << "Loader: reading the file: failure" << std::endl;
		return LOAD_FAILURE;
	}
	std::cout << "Loader: reading the file: success" << std::endl;

	std::stringstream buffer;
	buffer << in_file.rdbuf();

	//string str;
	//str.assign((std::istreambuf_iterator<char>(in_file) ), (std::istreambuf_iterator<char>()))

	Json::Reader reader;
	bool parsed = reader.parse(buffer.str(), content );
	//bool parsed = reader.parse(str, content );

	if ( !parsed )
	{
		std::cout  << "Loader: parsing the json: failure" << std::endl <<
			reader.getFormattedErrorMessages() << std::endl;

		return PARSE_FAILURE;
	}
	std::cout  << "Loader: parsing the json: success" << std::endl;
	return LOAD_SUCCESS;
};

/*
Json::Value Loader::load(const std::string& filename)
{
	Json::Value content;
	load(filename, content);
	return content;
};*/
