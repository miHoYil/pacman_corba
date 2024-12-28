#include <Ghost.h>


void Ghost::update(const std::vector<PacmanServer::Vec2>& walls)
{

	std::map<PacmanServer::Movement, bool> collisions;
	std::vector<std::pair<PacmanServer::Movement,PacmanServer::Vec2>> tempPositions(directions.size());

	int idx = 0;
	for (auto& dir : directions)
	{
		tempPositions[idx].first = dir.first;
		collisions[dir.first] = false;

		tempPositions[idx].second.x = _data.position.x + dir.second.x;
		tempPositions[idx].second.y = _data.position.y + dir.second.y;
		idx++;
	}

	for (auto& wall : walls)
	{
		for (size_t i = 0; i < tempPositions.size(); i++)
		{
			if ((wall.x + 1 > tempPositions[i].second.x) && (tempPositions[i].second.x > wall.x - 1) 
				&& (wall.y + 1 > tempPositions[i].second.y) && (tempPositions[i].second.y > wall.y - 1))
			{
				collisions[tempPositions[i].first] = true;
			}
		}
	}

	std::vector<std::pair<PacmanServer::Movement, PacmanServer::Vec2>> avalPositions;
	for (int i = 0; i < tempPositions.size(); i++) 
	{
		if (!collisions[tempPositions[i].first])
			avalPositions.push_back(tempPositions[i]);
	}

	bool randomizing = collisions[PacmanServer::Movement(_data.direction)] || !(avalPositions.size() <= 2);

	if(randomizing)
	{
		std::uniform_int_distribution<> distr(0, 100);
		int ver = distr(eng_ver);

		if (ver >= 60) //ver >= 0 && avalPositions.size() > 2 ||
		{
			std::uniform_int_distribution<> distr(0, avalPositions.size() - 1); // Define the range 
			int random_value = distr(eng);

			// Generate a random number
			while (avalPositions[random_value].first == _data.direction)
			{
				random_value = distr(eng);
			}

			_data.direction = avalPositions[random_value].first;
			_data.position = avalPositions[random_value].second;
		}
	}
	else
	{
		for (size_t i = 0; i < tempPositions.size(); i++) 
		{
			if (tempPositions[i].first == _data.direction)
				_data.position = tempPositions[i].second;
		}
	}

	//if (_data.type == 0) 
	//{
	//	for (auto& col : collisions)
	//		printf("(%d %d) ", col.first, col.second);

	//	printf("\nrand: %d\nafter: pos: %f,%f dir: %ld \n", randomizing, _data.position.x, _data.position.y, _data.direction);
	//}


	//# teleporting to the other side of the map

	if (_data.position.x < 0)
		_data.position.x = MAP_WIDTH - move_speed;
	else
		if (_data.position.x > MAP_WIDTH)
			_data.position.x = 0 + move_speed;

	if (_data.position.y < 0)
		_data.position.y = MAP_HEIGHT - move_speed;
	else
		if (_data.position.y > MAP_HEIGHT)
			_data.position.y = 0 + move_speed;

}
