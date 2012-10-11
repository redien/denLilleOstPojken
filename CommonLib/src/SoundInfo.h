#ifndef SOUNDINFO_H
#define SOUNDINFO_H

#include <SFML\Audio.hpp>
#include <string>

using namespace std;

struct SoundData
{
	sf::SoundBuffer buffer;
	string path;
	SoundData(string p_path);
};

struct SoundInfo
{
	bool play;
	string id;
	//This struct will include more info
	//Should contain all relevant information of a 
	//sound instance.
};

#endif