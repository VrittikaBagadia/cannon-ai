#include <fstream>
#include <iostream>
#include <stdio.h>      
#include <time.h>   
#include <vector> 
#include <math.h>
#include <algorithm>
#include <random> 
#include <map>
#include <string.h>
#include <unordered_map> 
#include <iomanip>
using namespace std;

#define INT_MAX 100000
#define INT_MIN	-100000
int board[8][8];
int M=8, N=8;
int soldiers1[12];
int soldiers2[12];
int soldiers_number = 12;

// int player_number = 0; 		// -1 for black, 1 for white

void print_board()
{
	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
			cout<<setw(3)<<board[i][j]<<" ";
		cout<<endl;
	}
}

int change_to_int(int x, int y)
{
	return (y*N + x);
}
void initialise()
{
	for (int i=0; i<M; i++)
		for (int j=0; j<N; j++)
			board[i][j] = 0;
	board[7][1]=-1; board[7][3]=-1; board[7][5]=-1; board[7][7]=-1;
	board[0][0]=1; board[0][2]=1; board[0][4]=1; board[0][6]=1;

	int counter = 2;
	for (int i=1; i<N; i+=2)
		for (int j=0; j<3; j++)
		{
			soldiers2[counter-2]=change_to_int(i,j);
			board[j][i] = counter++;
		}
	counter=2;
	for (int i=0; i<N; i+=2)
		for (int j=7; j>=5; j--)
		{
			soldiers1[counter-2]=change_to_int(i,j);
			board[j][i] = -1*counter;
			counter++;
		}
}
void possible_moves2(int soldier_number, int player_number, vector<int> &ans)
{
	ans.clear();
	int soldier_pos;
	soldier_pos = (player_number==-1)? soldiers1[soldier_number] : soldiers2[soldier_number];
	// if (soldier_pos < 0)
	// {
	// 	cout<<"soldier killed"<<endl;
	// 	return;
	// }
	int x = soldier_pos % N;
	int y = soldier_pos / N;
	int captured = 0;
	// forward
	if ((y+player_number < M) && (y+player_number >= 0))
	{
		if ( board[y+player_number][x] * player_number <= 0)		// <=0 ==> not my own soldier/townhall
			ans.push_back( change_to_int(x,y+player_number) );
		if ((x+1<N) && (board[y+player_number][x+1] * player_number <=0 ))
			ans.push_back( change_to_int(x+1,y+player_number));
		if ((x-1)>=0 && (board[y+player_number][x-1]*player_number<=0))
			ans.push_back(change_to_int(x-1,y+player_number));
	}
	// sideways
	if (x+1<N) 
	{ 
		if (board[y][x+1]*player_number < 0)	// opponent's soldier or townhall
		{
			ans.push_back(change_to_int(x+1,y));
			if (board[y][x+1] != -1*player_number)		// opponent's soldier and not townhall
				captured = 1;
		}
	}
	if (x-1 >= 0)
	{
		if (board[y][x-1]*player_number < 0)
		{
			ans.push_back(change_to_int(x-1,y));
			if (board[y][x+1] != -1*player_number)		// opponent's soldier and not townhall
				captured = 1;
		}
	}
	// retreat backwards
	if (captured == 0)
	{
		if ((y+player_number < M) && (y+player_number >=0))
		{
			if (board[y+player_number][x]*player_number<0 && board[y+player_number][x]!=-1*player_number)	// opponent's peice
				captured = 1;
			else if (x+1<N && board[y+player_number][x+1]*player_number<0 && board[y+player_number][x+1]!=-1*player_number)
				captured = 1;
			else if (x-1>=0 && board[y+player_number][x-1]*player_number<0 && board[y+player_number][x-1]!=-1*player_number)
				captured = 1;
		}
	}
	if (captured == 1)
	{
		int y2 = y-2*player_number;
		if (y2>=0 && y2<M)
		{
			if (board[y2][x]*player_number <= 0)		// not my own soldier/townhall
				ans.push_back(change_to_int(x,y2));
			if (x+2<N && board[y2][x+2]*player_number<=0)
				ans.push_back( change_to_int(x+2,y2) );
			if (x-2>=0 && board[y2][x-2]*player_number<=0)
				ans.push_back(change_to_int(x-2,y2));
		}
	}
}
void cannon_related(int player_number, vector<pair<int,int>> &move_pos, vector<pair<int,int>> &fire_at_f)
{
	// move_pos : soldier number to move to which position?
	// fire_at_f : fire at cell position (if not occupied by own soldier/townhall)
	move_pos.clear();
	fire_at_f.clear();
	unordered_map<int,int> fire_at;
	int* my_soldiers = (player_number==-1)? soldiers1:soldiers2;
	for (int i=0; i<soldiers_number; i++)
	{
		// cout<<"iteration(soldier number): "<<i<<endl;
		if (*(my_soldiers + i) == -1)
			continue;

		int x = *(my_soldiers+i)%N;
		int y = *(my_soldiers+i)/N;
		// cout<<"soldier at: "<<x<<" "<<y<<endl;

		// horizontal cannon
		if (x+1<N && x-1>=0 && board[y][x+1]*player_number>0 && board[y][x-1]*player_number>0 && board[y][x+1]!=player_number && board[y][x-1]!=player_number)	// my soldier, not townhall
		{
			if (x+2<N && board[y][x+2]==0)
			{
				move_pos.emplace_back(abs(board[y][x-1])-2, change_to_int(x+2,y));
				if (x+3<N && board[y][x+3]*player_number<=0)	// opponent's soldier/townhall or empty cell
					fire_at[change_to_int(x+3,y)] = i; 

				if (x+4<N && board[y][x+4]*player_number<=0)	// opponent's soldier
					fire_at[change_to_int(x+4,y)] = i;
			}
			if (x-2>=0 && board[y][x-2]==0)
			{
				move_pos.emplace_back(abs(board[y][x+1])-2, change_to_int(x-2,y));
				if (x-3>=0 && board[y][x-3]*player_number<=0)
					fire_at[change_to_int(x-3,y)] = i;

				else if (x-4>=0 && board[y][x-4]*player_number<=0)
					fire_at[change_to_int(x-4,y)] = i;
			}
		}

		if (y+1<M && y-1>=0)
		{
			// vertical cannon
			if (board[y+1][x]!=player_number && board[y-1][x]!=player_number && board[y+1][x]*player_number>0 && board[y-1][x]*player_number>0)
			{
				if (y+2<M && board[y+2][x]==0)
				{
					move_pos.emplace_back(abs(board[y-1][x])-2, change_to_int(x,y+2));
					if (y+3<M && board[y+3][x]*player_number <= 0)	
						fire_at[change_to_int(x,y+3)] = i;
						
					if (y+4<M && board[y+4][x]*player_number <= 0)
						fire_at[change_to_int(x,y+4)];
				}
				if (y-2>=0 && board[y-2][x]==0)
				{
					move_pos.emplace_back(abs(board[y+1][x])-2, change_to_int(x,y-2));
					if (y-3>=0 && board[y-3][x]*player_number < 0)
						fire_at[change_to_int(x,y-3)] = i;
		
					if (y-4<M && board[y-4][x]*player_number < 0)
						fire_at[change_to_int(x,y-4)] = i;
				}
			}
				// NE-SW cannon
			if (x+1<N && x-1>=0 && board[y-1][x+1]!=player_number && board[y-1][x+1]*player_number>0 && board[y+1][x-1]!=player_number && board[y+1][x-1]*player_number>0)
			{
				if (y-2>=0 && x+2<N && board[y-2][x+2]==0)
				{
					move_pos.emplace_back(abs(board[y+1][x-1])-2, change_to_int(x+2,y-2));
					if (y-3>=0 && x+3<N && board[y-3][x+3]*player_number <= 0)
						fire_at[change_to_int(x+3,y-3)] = i;

					if (y-4>=0 && x+4<N && board[y-4][x+4]*player_number <= 0)
						fire_at[change_to_int(x+4,y-4)] = i;
				}
				if (y+2<M && x-2>=0 && board[y+2][x-2]==0)
				{
					move_pos.emplace_back(abs(board[y-1][x+1])-2, change_to_int(x-2,y+2));
					if (y+3<M && x-3>=0 && board[y+3][x-3]*player_number <= 0)
						fire_at[change_to_int(x-3,y+3)] = i;
					if (y+4<M && x-4>=0 && board[y+4][x-4]*player_number <= 0)
						fire_at[change_to_int(x-4,y+4)] = i;
				}
			}
				// NW-SE cannon
			if (x-1>=0 && x+1<N && board[y-1][x-1]!=player_number && board[y-1][x-1]*player_number>0 && board[y+1][x+1]!=player_number && board[y+1][x+1]*player_number>0)
			{
				if (y-2>=0 && x-2>=0 && board[y-2][x-2]==0)
				{
					move_pos.emplace_back(abs(board[y+1][x+1])-2, change_to_int(x-2,y-2));
					if (y-3>=0 && x-3>=0 && board[y-3][x-3]*player_number <= 0)
						fire_at[change_to_int(x-3,y-3)] = i;
							
					if (y-4>=0 && x-4>=0 && board[y-4][x-4]*player_number <= 0)
						fire_at[change_to_int(x-4,y-4)] = i;
				}
				if (y+2<M && x+2<M && board[y+2][x+2]==0)
				{
					move_pos.emplace_back(abs(board[y-1][x-1])-2, change_to_int(x+2,y+2));
					if (y+3>=0 && x+3>=0 && board[y+3][x+3]*player_number <= 0)
						fire_at[change_to_int(x+3,y+3)] = i;
					if (y+4>=0 && x+4>=0 && board[y+4][x+4]*player_number <= 0)
						fire_at[change_to_int(x+4,y+4)] = i;		
				}
			}

		}
	}
	unordered_map<int,int>:: iterator itr; 
	for (itr = fire_at.begin(); itr!= fire_at.end(); itr++)
		fire_at_f.emplace_back(itr->first,itr->second);
}

float evaluation_function()
{
	return 1.0;
}
void play_move(int x1, int y1, char ch2, int x2, int y2, int player_id)		// player_id has received this move from opponent
{
	int *opponent_soldiers = (player_id==-1)? soldiers2:soldiers1;
	int *my_soldiers = (player_id==-1)? soldiers1:soldiers2;
	if (ch2 == 'B')
	{
		int temp = board[y2][x2];
		board[y2][x2] = 0;
		if (temp!=0 && temp!=player_id)		// soldier killed
			*(my_soldiers + temp*player_id - 2) = -1;
	}
	else if (ch2 == 'M')
	{
		int temp = board[y2][x2];

		// board update
		board[y2][x2] = board[y1][x1];
		board[y1][x1] = 0;

		// soldiers update
		int old_pos = change_to_int(x1,y1);
		int new_pos = change_to_int(x2,y2);
		for (int i=0; i<soldiers_number; i++)
			if (*(opponent_soldiers+i) == old_pos)
			{
				*(opponent_soldiers+i) = new_pos;
				break;
			}

		if (temp!=0 && temp != player_id)		// my soldier killed
			*(my_soldiers + temp*player_id-2) = -1;
	}
	else 
		cout<<"invalid format move from opponent"<<endl;
}

float minimax(int player_id, int depth)
{
	float best_val = (player_id == -1)? INT_MIN : INT_MAX; 		// player -1 maximises, +1 minimises
	float temp_val;
	int* my_soldiers = (player_id==-1)? soldiers1 : soldiers2;
	int* opponent_soldiers = (player_id==-1)? soldiers2 : soldiers1;
	vector<int> pos;
	for (int i=0; i<soldiers_number; i++)
	{
		if (*(my_soldiers+i) == -1)
			continue;
		possible_moves2(i, player_id, pos);
		for (int k=0; k<pos.size(); k++)
		{
			// soldier i of player_id moves to i_,j_
			int i_ = pos[k]%N;
			int j_ = pos[k]/N;
			int old_position = *(my_soldiers+i);
			int temp = board[j_][i_];

			// updating the board
			board[j_][i_] = player_id * (i+2);
			board[old_position/N][old_position%N] = 0;

			// updating soldiers
			*(my_soldiers + i) = change_to_int(i_,j_);
			int opp_soldier_killed, opp_old_position, killed=0;
			if (temp*player_id < 0 && temp != -1*player_id)		// opponent's soldier killed
			{
				killed = 1;
				opp_soldier_killed = abs(temp)-2;
				opp_old_position = *(opponent_soldiers + opp_soldier_killed);
				*(opponent_soldiers + opp_soldier_killed) = -1;
			}

			// minimax
			if (depth == 1)
				temp_val = evaluation_function();
			else
				temp_val = minimax(-1*player_id, depth-1);
			if (player_id==-1 && temp_val>best_val)
				best_val = temp_val;
			else if (player_id==1 && temp_val<best_val)
				best_val = temp_val;

			// restoring board
			board[old_position/N][old_position%N] = player_id * (i+2);
			board[j_][i_] = temp;

			// restoring soldiers
			*(my_soldiers + i) = old_position;
			if (killed)
				*(opponent_soldiers + opp_soldier_killed) = opp_old_position;
		}
	}
	vector<pair<int,int>> move_pos;
	vector<pair<int,int>> fire_at;			// hit 1st int cell from second soldier
	cannon_related(player_id, move_pos, fire_at);
	for (int i=0; i<move_pos.size(); i++)
	{
		int soldier_moved = move_pos[i].first;
		int new_pos = move_pos[i].second;
		int old_position = *(my_soldiers + soldier_moved);
		if (board[old_position/N][old_position%N] != (player_id*(soldier_moved+2)))
			cout<<"SOME ERROR"<<endl;

		// updating board
		board[new_pos/N][new_pos%N] = player_id * (soldier_moved + 2);
		board[old_position/N][old_position%N] = 0;

		// updating soldiers
		*(my_soldiers + soldier_moved) = new_pos;

		// minimax
		if (depth == 1)
			temp_val = evaluation_function();
		else
			temp_val = minimax(-1*player_id, depth-1);
		if (player_id==-1 && temp_val>best_val)
			best_val = temp_val;
		else if (player_id==1 && temp_val<best_val)
			best_val = temp_val;

		// restoring board
		board[new_pos/N][new_pos%N] = 0;
		board[old_position/N][old_position%N] = player_id * (soldier_moved + 2);

		// restoring soldiers
		*(my_soldiers + soldier_moved) = old_position;
	}
	for (int i=0; i<fire_at.size(); i++)
	{
		int opp_soldier;
		int bombed = fire_at[i].first;
		// board and soldiers update
		int i_ = bombed % N;
		int j_ = bombed / N;
		int temp = board[j_][i_];
		board[j_][i_] = 0;
		if (temp != -1*player_id && temp!=0)		// opponent's soldier
		{
			opp_soldier = abs(temp)-2;
			if (*(opponent_soldiers + opp_soldier) != bombed)
				cout<<"inconsistency in board and soldiers"<<endl;
			*(opponent_soldiers + opp_soldier) = -1;
		}
		// if (bombed <= 0)	 	// townhall destroyed
		// 	board[(-1*bombed)/ N][(-1*bombed) % N] = 0;
		// else 				// soldier killed
		// {
		// 	opp_position = *(opponent_soldiers + bombed - 2);
		// 	board[opp_position/N][opp_position%N] = 0;
		// 	*(opponent_soldiers + bombed -2) = -1;
		// }

		//minimax
		if (depth == 1)
			temp_val = evaluation_function();
		else
			temp_val = minimax(-1*player_id, depth-1);
		if (player_id==-1 && temp_val>best_val)
			best_val = temp_val;
		else if (player_id==1 && temp_val<best_val)
			best_val = temp_val;		

		// restoration
		board[j_][i_] = temp;
		if (temp != -1*player_id && temp!=0)
			*(opponent_soldiers + opp_soldier) = bombed;
		// if (bombed <= 0)
		// 	board[(-1*bombed)/N][(-1*bombed)%N] = -1 * player_id;
		// else
		// {
		// 	*(opponent_soldiers + bombed -2) = opp_position;
		// 	board[opp_position/N][opp_position%N] = -1 * player_id * bombed;
		// }
	}
	return best_val;

}
string root_minimax(int player_id, int depth)
{
	int soldier_to_move, position_to_fire_or_move;
	char fire_or_move;
	float best_val = (player_id == -1)? INT_MIN : INT_MAX;
	float temp_val;
	int* my_soldiers = (player_id == -1)? soldiers1 : soldiers2;
	int* opponent_soldiers = (player_id == -1)? soldiers2 : soldiers1;
	vector<int> pos;
	for (int i=0; i<soldiers_number; i++)
	{
		if (*(my_soldiers + i) == -1)
			continue;
		possible_moves2(i, player_id, pos);
		for (int k=0; k<pos.size(); k++)
		{
			int i_ = pos[k]%N;
			int j_ = pos[k]/N;
			int old_position = *(my_soldiers + i);
			int temp = board[j_][i_];

			// updating board
			board[j_][i_] = player_id * (i+2);
			board[old_position/N][old_position%N] = 0;

			// updating soldiers
			*(my_soldiers+i) = change_to_int(i_,j_);
			int opp_soldier_killed, opp_old_position, killed=0;
			if (temp*player_id<0 && temp!=-1*player_id)
			{
				killed = 1;
				opp_soldier_killed = abs(temp)-2;
				*(opponent_soldiers+opp_soldier_killed) = -1;
			}

			// minimax
			if (depth == 1)
				temp_val = evaluation_function();
			else
				temp_val = minimax(-1*player_id, depth-1);

			if (player_id==-1 && temp_val>best_val)
				{
					best_val = temp_val;
					soldier_to_move = i;
					position_to_fire_or_move = pos[k];
					fire_or_move = 'M';
				}
			else if (player_id==1 && temp_val<best_val)
				{
					best_val = temp_val;
					soldier_to_move = i;
					position_to_fire_or_move = pos[k];
					fire_or_move = 'M';
				}

			// restoring board
			board[old_position/N][old_position%N] = player_id * (i+2);
			board[j_][i_] = temp;

			// restoring soldiers
			*(my_soldiers + i) = old_position;
			if (killed)
				*(opponent_soldiers + opp_soldier_killed) = change_to_int(i_,j_);

		}
	}
	vector<pair<int,int>> move_pos;
	vector<pair<int,int>> fire_at; 
	cannon_related(player_id, move_pos, fire_at);
	for (int i=0; i<move_pos.size(); i++)
	{
		int soldier_moved = move_pos[i].first;
		int new_pos = move_pos[i].second;
		int old_position = *(my_soldiers + soldier_moved);
		if (board[old_position/N][old_position%N] != (player_id*(soldier_moved+2)))
			cout<<"SOME ERROR"<<endl;

		// updating board
		board[new_pos/N][new_pos%N] = player_id * (soldier_moved + 2);
		board[old_position/N][old_position%N] = 0;

		// updating soldiers
		*(my_soldiers + soldier_moved) = new_pos;

		// minimax
		if (depth == 1)
			temp_val = evaluation_function();
		else
			temp_val = minimax(-1*player_id, depth-1);

		if (player_id==-1 && temp_val>best_val)
		{
			best_val = temp_val;
			soldier_to_move = soldier_moved;
			position_to_fire_or_move = new_pos;
			fire_or_move = 'M';
		}
		else if (player_id==1 && temp_val<best_val)
		{
			best_val = temp_val;
			soldier_to_move = soldier_moved;
			position_to_fire_or_move = new_pos;
			fire_or_move = 'M';
		}

		// restoring board
		board[new_pos/N][new_pos%N] = 0;
		board[old_position/N][old_position%N] = player_id * (soldier_moved + 2);

		// restoring soldiers
		*(my_soldiers + soldier_moved) = old_position;
	}
	for (int i=0; i<fire_at.size(); i++)
	{
		int opp_soldier;
		int bombed = fire_at[i].first;
		// board and soldiers update
		int i_ = bombed % N;
		int j_ = bombed / N;
		int temp = board[j_][i_];
		board[j_][i_] = 0;
		if (temp != -1*player_id && temp!=0)		// opponent's soldier
		{
			opp_soldier = abs(temp)-2;
			if (*(opponent_soldiers + opp_soldier) != bombed)
				cout<<"inconsistency in board and soldiers :("<<endl;
			*(opponent_soldiers + opp_soldier) = -1;
		}

		//minimax
		if (depth == 1)
			temp_val = evaluation_function();
		else
			temp_val = minimax(-1*player_id, depth-1);

		if (player_id==-1 && temp_val>best_val)
		{
			best_val = temp_val;
			soldier_to_move = fire_at[i].second;
			position_to_fire_or_move = bombed;
			fire_or_move = 'F';
		}
		else if (player_id==1 && temp_val<best_val)
		{
			best_val = temp_val;
			soldier_to_move = fire_at[i].second;
			position_to_fire_or_move = bombed;
			fire_or_move = 'F';
		}

		// restoration
		board[j_][i_] = temp;
		if (temp != -1*player_id && temp!=0)
			*(opponent_soldiers + opp_soldier) = bombed;
	}

	// we have the best move
	string best_move = "S " + to_string(*(my_soldiers+soldier_to_move)%N) + " " + to_string(*(my_soldiers+soldier_to_move)/N) + " " + to_string(fire_or_move) + " " + to_string(position_to_fire_or_move%N) + " " + to_string(position_to_fire_or_move/N);
	return best_move;
}

int main(int argc, char const *argv[])
{
	// take opponent move string
	// update soldier if any captured, board position
	// 1. update cannons
	// 2. for all soldiers find possible moves
	// 3. for all cannons find firing positions
	// 4. choose something ---> 1. se opponent soldier killed
	// 5. cout required string
	// 6. update board, soldiers 
	// wait for opponent move

	initialise();
	// print_board();
	int player_id=0, timelimit=10;
	// cin>>player_id>>M>>N>>timelimit;
	if (player_id == 0)
		player_id = -1;
	if (player_id == 1)
	{
		char ch1,ch2;
		int x1,y1,x2,y2;
		cin>>ch1>>x1>>y1>>ch2>>x2>>y2;
		play_move(x1,y1,ch2,x2,y2,player_id);
		print_board();
	}
	// play();
	// vector<int> cells;
	// possible_moves2(2,-1,cells);
	// cout<<"--------------"<<endl;
	// for (int i=0; i<cells.size(); i++)
	// 	cout<<i<<" : "<<cells[i]%N<<" "<<cells[i]/N<<endl;

	// cells.clear();
	// possible_moves2(5,-1,cells);
	// cout<<"--------------"<<endl;
	// for (int i=0; i<cells.size(); i++)
	// 	cout<<i<<" : "<<cells[i]%N<<" "<<cells[i]/N<<endl;

	// vector<pair<int,int>> move_pos;
	// vector<int> fire_at_f;
	// cannon_related(-1, move_pos, fire_at_f);
	// cout<<"cannon movement"<<endl;
	// for (int i=0; i<move_pos.size(); i++)
	// 	cout<<move_pos[i].first<<" moved to: "<<move_pos[i].second%N<<" "<<move_pos[i].second/N<<endl;
	// cout<<"cannon firing"<<endl;
	// for (int i=0; i<fire_at_f.size(); i++)
	// 	cout<<"hit at "<<fire_at_f[i]<<endl;


	return 0;
}