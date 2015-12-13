/*
 * Loader.hpp
 *
 *  Created on: 14 Sep 2014
 *      Author: shrapx
 */

#ifndef _LOADER_HPP_
#define _LOADER_HPP_

#include <iostream>
#include <fstream>
#include <sstream>

#include <jsoncpp/json/json.h>

namespace Loader
{

enum {LOAD_SUCCESS=0, LOAD_FAILURE, PARSE_FAILURE};

int load(const std::string& filename, Json::Value& content);

//Json::Value load(const std::string& filename);

};

#endif /* _LOADER_HPP_ */
