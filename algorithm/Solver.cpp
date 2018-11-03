#include "Solver.hpp"
#include <fstream>

Solver::Solver(Cube c) {
	Cube tmp;

	for (int i = 0; i < 18; i++)
		allowedMoves[i] = 1;
	for (int i = 1 ; i <= 4; i++)
		phaseGoal[i] = getPhaseId(tmp, i);
}

void	Solver::readData(std::string file,
	unordered_map<int64_t, int> &map){
	std::ifstream input(file);

	int64_t hash;
	int moves;
	while (input >> hash >> moves){
		map[hash] = moves;
	}
}

char moves[6] = {'F','R','U','B','L','D'};

int		max(int64_t a, int64_t b, int64_t c){
	int64_t num = a;
	if (num < b)
		num = b;
	if (num < c)
		num = c;
	return num;
}

int phase = 1;

void Solver::nextPhase(){
	switch (phase){
		case 1:
			allowedMoves[0] = 0;
			allowedMoves[2] = 0;
			allowedMoves[9] = 0;
			allowedMoves[11] = 0;
			break;
		case 2:
			// RL
			allowedMoves[3] = 0;
			allowedMoves[5] = 0;
			allowedMoves[12] = 0;
			allowedMoves[14] = 0;
			break;
		case 3:
			// UD
			allowedMoves[6] = 0;
			allowedMoves[8] = 0;
			allowedMoves[15] = 0;
			allowedMoves[17] = 0;
	}
	phase++;
}



int64_t Solver::getPhaseId(Cube c, int phase){
	int64_t id = 0;
	if (phase == 1){
		for (int i = 0; i < 12; i++)
		{
			id <<= 1;
			id += c.eOri[i];
		}
	}
	else if (phase == 2){
		for (int i = 0; i < 8; i++)
		{
			id <<= 2;
			id += c.cOri[i];
		}
		for (int i = 0; i < 12; i++){
			id <<= 1;
			if (c.ePos[i] < 8)
				id++;
		}
	}
	else if (phase == 3){
		string faces = "FRUBLD";
		for (int i = 0; i < 8; i++){
			for (int j = 0; j < 3; j++){
				id <<= 1;
				char t = c.cornerNames[c.cPos[i]][(c.cOri[i] + j) % 3];
				if (!(t == c.cornerNames[i][j] ||
					t == faces[(faces.find(c.cornerNames[i][j]) + 3) % 6])){
					id++;
				}
			}			
		}
		for (int i = 0; i < 12; i++){
			for (int j = 0; j < 2; j++){
				id <<= 1;
				char t = c.edgeNames[c.ePos[i]][(c.eOri[i] + j) % 2];
				if (!(t == c.edgeNames[i][j] ||
					t == faces[(faces.find(c.edgeNames[i][j]) + 3) % 6])){
					id++;
				}
			}			
		}
		for (int i = 0; i < 8; i++)
		{
			id <<= 1;
			if (c.cPos[i] != i && c.cPos[i] != (i + 4) % 8)
				id++;
		}
		id <<= 1;
		for (int i = 0; i < 8; i++ )
			for( int j = i + 1; j < 8; j++ )
		id ^= c.cPos[i] > c.cPos[j];
	}
	else if (phase == 4){
		string faces = "FRUBLD";
		for (int i = 0; i < 8; i++){
			for (int j = 0; j < 3; j++){
				id <<= 1;
				char t = c.cornerNames[c.cPos[i]][(c.cOri[i] + j) % 3];
				if (t == faces[(faces.find(c.cornerNames[i][j]) + 3) % 6]){
					id++;
				}
			}			
		}
		for (int i = 0; i < 12; i++){
			for (int j = 0; j < 2; j++){
				id <<= 1;
				char t = c.edgeNames[c.ePos[i]][(c.eOri[i] + j) % 2];
				if (t == faces[(faces.find(c.edgeNames[i][j]) + 3) % 6]){
					id++;
				}
			}			
		}
	}
	return id;
}

Cube	Solver::BFS(int step, queue<Cube> level){
	printf("Phase %d, goal: %d\n", phase, phaseGoal[phase]);
	if (step == 0){
		ids.clear();
		int64_t num = getPhaseId(level.front(), phase);
		if (num == phaseGoal[phase]){
			nextPhase();
			return level.front();
		}
	}

	queue<Cube> next;
	Cube cur;
	while (!level.empty()){
		cur = level.front();
		level.pop();
		int count = 0;
		for (int move = 0; move < 6; move++)
		{
			for (int amount = 0; amount < 3; amount++)
			{
				cur.rotCube(moves[move], 1);
				int64_t id;
				if (allowedMoves[count] == 1){
					id = getPhaseId(cur, phase);
					// cout << moves[move] << " " << amount << endl;
					if (ids.find(id) == ids.end())
					{
						if (id == phaseGoal[phase])
						{
							cur.path += moves[move];
							cur.path += (amount + '1');
							nextPhase();
							return cur;
						}
						ids.insert(id);
						cur.path += moves[move];
						cur.path += (amount + '1');
						next.push(cur);
						cur.path = cur.path.substr(0, cur.path.length() - 2);
					}
				}
				count++;
			}
			cur.rotCube(moves[move], 1);
		}
	}
	if (next.empty())
	{
		cout << "Solution not found\n";
		exit(0);
	}
	return BFS(step + 1, next);
}
