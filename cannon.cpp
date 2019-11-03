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
#include <time.h>
using namespace std;

#define MAXI 100000
#define MINI -100000

int board[10][10];
int soldiers1[15];
int soldiers2[15];
int townhalls[3];
int scount[3];

int N, M;		// N is the number of rows and M is the number of columns
int soldiers_number;

// int player_number = 0; 		// -1 for black, 1 for white

void print_board()
{
	for (int i=0; i<M; i++)
	{
		for (int j=0; j<N; j++)
			cerr<<setw(3)<<board[i][j]<<" ";
		cerr<<endl;
	}
}

int change_to_int(int x, int y)
{
	return (y*N + x);
}
bool isEqual(float a, float b)
{
	if (abs(a-b) < 0.00001)
		return true;
	return false;
}
void initialise()
{
	for (int i=0; i<M; i++)
		for (int j=0; j<N; j++)
			board[i][j] = 0;

	// player -1 townhalls
	board[M-1][1]=-1; board[M-1][3]=-1; board[M-1][5]=-1; board[M-1][7]=-1;

	// player +1 townhalls
	board[0][0]=1; board[0][2]=1; board[0][4]=1; board[0][6]=1;

	if (N==10)
	{
		board[M-1][9] = -1;
		board[0][8] = 1;
	}

	// board[7][1]=-1; board[7][3]=-1; board[7][5]=-1; board[7][7]=-1;
	// board[0][0]=1; board[0][2]=1; board[0][4]=1; board[0][6]=1;

	// player +1 soldiers
	int counter = 2;
	for (int i=1; i<N; i+=2)
		for (int j=0; j<3; j++)
		{
			soldiers2[counter-2]=change_to_int(i,j);
			board[j][i] = counter++;
		}

	// player -1 soldiers
	counter=2;
	for (int i=0; i<N; i+=2)
		for (int j=M-1; j>=M-3; j--)
		{
			soldiers1[counter-2]=change_to_int(i,j);
			board[j][i] = -1*counter;
			counter++;
		}

	townhalls[0] = N/2 ; townhalls[2] = N/2;
	scount[0] = soldiers_number ; scount[2] = soldiers_number;
}
void possible_moves2(int soldier_number, int player_number, vector<int> &ans)
{
	ans.clear();
	int soldier_pos;
	soldier_pos = (player_number==-1)? soldiers1[soldier_number] : soldiers2[soldier_number];
	// if (soldier_pos < 0)
	// {
	// 	cerr<<"soldier killed"<<endl;
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
			if (board[y][x-1] != -1*player_number)		// opponent's soldier and not townhall
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
void cannon_related_2(int player_number, int &number_of_cannons, int &number_of_cannons_horiz ,int &townhalls_under_fire, int &soldiers_under_fire)
{
	number_of_cannons = 0;
	number_of_cannons_horiz = 0;
	townhalls_under_fire = 0;

	soldiers_under_fire = 0;
	// int non_cannon_soldiers_under_fire = 0;		// better if that soldier is part of opponent's cannon
	// int cannon_soldier_under_fire = 0;

	unordered_map<int,int> fire_at;
	int* my_soldiers = (player_number==-1)? soldiers1:soldiers2;
	for (int i=0; i<soldiers_number; i++)
	{
		// cerr<<"iteration(soldier number): "<<i<<endl;
		if (*(my_soldiers + i) == -1)
			continue;

		int x = *(my_soldiers+i)%N;
		int y = *(my_soldiers+i)/N;
		// cerr<<"soldier at: "<<x<<" "<<y<<endl;

		// horizontal cannon
		if (x+1<N && x-1>=0 && board[y][x+1]*player_number>0 && board[y][x-1]*player_number>0 && board[y][x+1]!=player_number && board[y][x-1]!=player_number)	// my soldier, not townhall
		{
			number_of_cannons_horiz++;
			if (x+2<N && board[y][x+2]==0)
			{
				if (x+3<N && board[y][x+3]*player_number <= 0)	// opponent's soldier/townhall or empty cell
					fire_at[change_to_int(x+3,y)] = i; 

				if (x+4<N && board[y][x+4]*player_number <= 0)	// opponent's soldier
					fire_at[change_to_int(x+4,y)] = i;
			}
			if (x-2>=0 && board[y][x-2]==0)
			{
				if (x-3>=0 && board[y][x-3]*player_number <= 0)
					fire_at[change_to_int(x-3,y)] = i;

				if (x-4>=0 && board[y][x-4]*player_number <= 0)
					fire_at[change_to_int(x-4,y)] = i;
			}
		}

		if (y+1<M && y-1>=0)
		{
			// vertical cannon
			if (board[y+1][x]!=player_number && board[y-1][x]!=player_number && board[y+1][x]*player_number>0 && board[y-1][x]*player_number>0)
			{
				number_of_cannons++;
				if (y+2<M && board[y+2][x]==0)
				{
					if (y+3<M && board[y+3][x]*player_number <= 0)	
						fire_at[change_to_int(x,y+3)] = i;
						
					if (y+4<M && board[y+4][x]*player_number <= 0)
						fire_at[change_to_int(x,y+4)] = i;
				}
				if (y-2>=0 && board[y-2][x]==0)
				{
					if (y-3>=0 && board[y-3][x]*player_number <= 0)
						fire_at[change_to_int(x,y-3)] = i;
		
					if (y-4>=0 && board[y-4][x]*player_number <= 0)
						fire_at[change_to_int(x,y-4)] = i;
				}
			}
				// NE-SW cannon
			if (x+1<N && x-1>=0 && board[y-1][x+1]!=player_number && board[y-1][x+1]*player_number>0 && board[y+1][x-1]!=player_number && board[y+1][x-1]*player_number>0)
			{
				if (x == 1 && y==1)
					continue;
				if (x==(N-2) && y==(M-2))			// CHECK
					continue;
				number_of_cannons++;
				if (y-2>=0 && x+2<N && board[y-2][x+2]==0)
				{
					if (y-3>=0 && x+3<N && board[y-3][x+3]*player_number <= 0)
						fire_at[change_to_int(x+3,y-3)] = i;

					if (y-4>=0 && x+4<N && board[y-4][x+4]*player_number <= 0)
						fire_at[change_to_int(x+4,y-4)] = i;
				}
				if (y+2<M && x-2>=0 && board[y+2][x-2]==0)
				{
					if (y+3<M && x-3>=0 && board[y+3][x-3]*player_number <= 0)
						fire_at[change_to_int(x-3,y+3)] = i;
					if (y+4<M && x-4>=0 && board[y+4][x-4]*player_number <= 0)
						fire_at[change_to_int(x-4,y+4)] = i;
				}
			}
				// NW-SE cannon
			if (x-1>=0 && x+1<N && board[y-1][x-1]!=player_number && board[y-1][x-1]*player_number>0 && board[y+1][x+1]!=player_number && board[y+1][x+1]*player_number>0)
			{
				if (x==(N-2) && y==1)		// CHECK
					continue;
				if (x==1 && y==(M-2))		// CHECK
					continue;
				number_of_cannons++;
				if (y-2>=0 && x-2>=0 && board[y-2][x-2]==0)
				{
					if (y-3>=0 && x-3>=0 && board[y-3][x-3]*player_number <= 0)
						fire_at[change_to_int(x-3,y-3)] = i;
							
					if (y-4>=0 && x-4>=0 && board[y-4][x-4]*player_number <= 0)
						fire_at[change_to_int(x-4,y-4)] = i;
				}
				if (y+2<M && x+2<N && board[y+2][x+2]==0)
				{
					if (y+3<M && x+3<N && board[y+3][x+3]*player_number <= 0)
						fire_at[change_to_int(x+3,y+3)] = i;
					if (y+4<M && x+4<N && board[y+4][x+4]*player_number <= 0)
						fire_at[change_to_int(x+4,y+4)] = i;		
				}
			}

		}
	}
	unordered_map<int,int>:: iterator itr; 
	for (itr=fire_at.begin(); itr!=fire_at.end(); itr++)
	{
		if (board[(itr->first)/N][(itr->first)%N] == -1*player_number)
			townhalls_under_fire++;
		else if (board[(itr->first)/N][(itr->first)%N] !=0)
			soldiers_under_fire++;
			// {
			// 	if (inCannon(itr->first))
			// 		cannon_soldier_under_fire++;
			// 	else
			// 		non_cannon_soldiers_under_fire++;
			// }
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
		// cerr<<"iteration(soldier number): "<<i<<endl;
		if (*(my_soldiers + i) == -1)
			continue;

		int x = *(my_soldiers+i)%N;
		int y = *(my_soldiers+i)/N;
		// cerr<<"soldier at: "<<x<<" "<<y<<endl;

		// horizontal cannon
		if (x+1<N && x-1>=0 && board[y][x+1]*player_number>0 && board[y][x-1]*player_number>0 && board[y][x+1]!=player_number && board[y][x-1]!=player_number)	// my soldier, not townhall
		{
			if (x+2<N && board[y][x+2]==0)
			{
				move_pos.emplace_back(abs(board[y][x-1])-2, change_to_int(x+2,y));
				if (x+3<N && board[y][x+3]*player_number <= 0)	// opponent's soldier/townhall or empty cell
					fire_at[change_to_int(x+3,y)] = i; 

				if (x+4<N && board[y][x+4]*player_number <= 0)	// opponent's soldier
					fire_at[change_to_int(x+4,y)] = i;
			}
			if (x-2>=0 && board[y][x-2]==0)
			{
				move_pos.emplace_back(abs(board[y][x+1])-2, change_to_int(x-2,y));
				if (x-3>=0 && board[y][x-3]*player_number <= 0)
					fire_at[change_to_int(x-3,y)] = i;

				if (x-4>=0 && board[y][x-4]*player_number <= 0)
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
						fire_at[change_to_int(x,y+4)] = i;
				}
				if (y-2>=0 && board[y-2][x]==0)
				{
					move_pos.emplace_back(abs(board[y+1][x])-2, change_to_int(x,y-2));
					if (y-3>=0 && board[y-3][x]*player_number <= 0)
						fire_at[change_to_int(x,y-3)] = i;
		
					if (y-4>=0 && board[y-4][x]*player_number <= 0)
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
				if (y+2<M && x+2<N && board[y+2][x+2]==0)
				{
					move_pos.emplace_back(abs(board[y-1][x-1])-2, change_to_int(x+2,y+2));
					if (y+3<M && x+3<N && board[y+3][x+3]*player_number <= 0)
						fire_at[change_to_int(x+3,y+3)] = i;
					if (y+4<M && x+4<N && board[y+4][x+4]*player_number <= 0)
						fire_at[change_to_int(x+4,y+4)] = i;		
				}
			}

		}
	}
	unordered_map<int,int>:: iterator itr; 
	for (itr = fire_at.begin(); itr!= fire_at.end(); itr++)
		fire_at_f.emplace_back(itr->first,itr->second);
}

// float evaluation_function(int player_id)
// {
// 	float score;
	

// 	int number_of_cannons0, number_of_cannons2, townhalls_under_fire0, townhalls_under_fire2, soldiers_under_fire0, soldiers_under_fire2, number_of_cannons_horiz0, number_of_cannons_horiz2;
// 	cannon_related_2(1, number_of_cannons2, number_of_cannons_horiz2 ,townhalls_under_fire0, soldiers_under_fire0);
// 	cannon_related_2(-1, number_of_cannons0, number_of_cannons_horiz0, townhalls_under_fire2, soldiers_under_fire2);

// 	score = 100*(townhalls[0]-townhalls[2]) + (scount[0]-scount[2]) + 10*(soldiers_under_fire2 - soldiers_under_fire0) + 20*(townhalls_under_fire2-townhalls_under_fire0) + 10*(number_of_cannons0-number_of_cannons2) + 3*(number_of_cannons_horiz0-number_of_cannons_horiz2);
// 	// if (show==1)
// 	// cerr<<"black_townhalls: "<<townhalls[0]<<" white townhalls: "<<townhalls[2]<<" black soldiers: "<<scount[0]<<" white soldiers: "<<scount[2]<<" score: "<<score<<endl;
// 	return score;
// }
void townhalls_attacked_by_soldiers(int &black_townhalls, int &white_townhalls)			// CHANGE THIS
{
	black_townhalls=0;
	white_townhalls=0;
	for(int i=0;i<=6;i+=2)
	{
		
		bool f=false;
		if(board[0][i]==1)
		{
			if(i-1>=0)
				{
					if(board[0][i-1]<0 || board[1][i-1]<0)
						f=true;
				}
			if(board[1][i]<0 || board[0][i+1]<0 || board[1][i+1]<0)
				f=true;
		}

		if(f)
			white_townhalls++;
	}

	for(int i=1;i<=7;i+=2)
	{
		bool f=false;
		if(board[7][i]==-1)
		{
			if(i+1<8)
				{
					if(board[7][i+1]>0 || board[6][i+1]>0)
						f=true;
				}
			if(board[6][i]>0 || board[7][i-1]>0 || board[6][i-1]>0)
				f=true;
		}

		if(f)
			black_townhalls++;
	}

	// return (white_townhalls - black_townhalls);
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
		{
			*(my_soldiers + temp*player_id - 2) = -1;
			scount[player_id+1]-=1;
		}
		else if (temp==player_id)
			townhalls[player_id+1]-=1;
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
		{
			*(my_soldiers + temp*player_id-2) = -1;
			scount[player_id+1]-=1;
		}
		else if (temp==player_id)
			townhalls[player_id+1]-=1;
	}
	else 
		cerr<<"invalid format move from opponent"<<endl;
}

void play_move2(string s, int player_id)
{
	play_move(s[2]-48, s[4]-48, s[6], s[8]-48, s[10]-48, player_id);
}

float evaluation_function(int player_id)
{
	float score;
	

	int number_of_cannons0, number_of_cannons2, townhalls_under_fire0, townhalls_under_fire2, soldiers_under_fire0, soldiers_under_fire2, number_of_cannons_horiz0, number_of_cannons_horiz2=0;

	
	// if (townhalls[0] <= 2)
	// 	return -1000;
	// if (townhalls[2] <= 2)
	// 	return 1000;

	cannon_related_2(1, number_of_cannons2, number_of_cannons_horiz2 ,townhalls_under_fire0, soldiers_under_fire0);
	cannon_related_2(-1, number_of_cannons0, number_of_cannons_horiz0, townhalls_under_fire2, soldiers_under_fire2);
	int attacked_townhalls_black, attacked_townhalls_white;
	townhalls_attacked_by_soldiers(attacked_townhalls_black, attacked_townhalls_white);

	float black_score = 150*townhalls[0] +  15*scount[0] - 40*townhalls_under_fire0 - 40*attacked_townhalls_black;
	float white_score = 150*townhalls[2] +  15*scount[2] - 40*townhalls_under_fire2 - 40*attacked_townhalls_white;

	// if (player_id == -1)
	// 	score = 0.6*black_score - 0.4*white_score;
	// else
	// 	score = 0.4*black_score - 0.6*white_score;
	
	// score += 6*(number_of_cannons0-number_of_cannons2) + 2*(number_of_cannons_horiz0-number_of_cannons_horiz2)+0.5*(soldiers_under_fire2 - soldiers_under_fire0);

	score = black_score - white_score;
	score += 6*(number_of_cannons0-number_of_cannons2) + 2*(number_of_cannons_horiz0-number_of_cannons_horiz2)+0.5*(soldiers_under_fire2 - soldiers_under_fire0);

	if (min(scount[0],scount[2]) <=4)
		score += 15*(scount[0] - scount[2]);
	
	if (townhalls[0] <= 2)
		return (-1000+score);
	if (townhalls[2] <= 2)
		return (1000+score);
	return score;
}

float minimax(int player_id, int depth , float alpha, float beta)
{
	// cerr<<"depth: "<<depth<<endl;
	float best_val = (player_id == -1)? MINI : MAXI; 		// player -1 maximises, +1 minimises
	float best_val_static = -1*best_val; 	float temp_val;
	int* my_soldiers = (player_id==-1)? soldiers1 : soldiers2;
	int* opponent_soldiers = (player_id==-1)? soldiers2 : soldiers1;
	vector<int> pos;

	clock_t t;
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
			if (temp == -1*player_id)							// opponent's townhall
				townhalls[-1*player_id + 1]-=1;
			// if (temp*player_id < 0 && temp != -1*player_id)		// opponent's soldier killed
			else if (temp*player_id < 0)
			{
				killed = 1;
				opp_soldier_killed = abs(temp)-2;
				opp_old_position = *(opponent_soldiers + opp_soldier_killed);

				if (opp_old_position != pos[k])
				{
					print_board();
					cerr<<"kuch galti at depth "<<depth<<endl;
					cerr<<"player "<<player_id<<" soldier id "<<i<<endl;
					cerr<<"new position: "<<i_<<" "<<j_<<endl;
					cerr<<"soldier killed "<<opp_soldier_killed<<" at position: "<<opp_old_position%N<<" "<<opp_old_position/N<<endl;
					
				}
				*(opponent_soldiers + opp_soldier_killed) = -1;
				scount[-1*player_id + 1]-=1;
			}

			// minimax
			if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

			else if (depth == 0)
				temp_val = evaluation_function(player_id);
			else
				temp_val = minimax(-1*player_id, depth-1, alpha, beta);
			if (player_id==-1 && temp_val>=best_val)
				best_val = temp_val;
			else if (player_id==1 && temp_val<=best_val)
				best_val = temp_val;

			// restoring board
			board[old_position/N][old_position%N] = player_id * (i+2);
			board[j_][i_] = temp;

			// restoring soldiers
			*(my_soldiers + i) = old_position;
			if (temp == -1*player_id)
				townhalls[-1*player_id+1]+=1;
			if (killed)
			{
				scount[-1*player_id + 1] += 1;
				*(opponent_soldiers + opp_soldier_killed) = opp_old_position;
			}

			// alpha beta pruning
			if (player_id == -1)
				alpha = max(alpha, temp_val);
			else 
				beta = min(beta, temp_val);
			if (alpha > beta)
				return temp_val;
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
		{
			cerr<<"SOME ERROR at depth "<<depth<<" for player id "<< player_id<<endl;
			cerr<<"soldier to be moved: "<<soldier_moved<<" to cell "<<new_pos%N<<" "<<new_pos/N<<endl;
			cerr<<"currently at cell "<<old_position%N<<" "<<old_position/N<<endl;
			cerr<<"number on this cell "<<board[old_position/N][old_position%N];
		}

		// updating board
		board[new_pos/N][new_pos%N] = player_id * (soldier_moved + 2);
		board[old_position/N][old_position%N] = 0;

		// updating soldiers
		*(my_soldiers + soldier_moved) = new_pos;

		// minimax
		if (min(townhalls[0],townhalls[2]) <= 2)
			temp_val = evaluation_function(player_id);

		else if (depth == 0)
			temp_val = evaluation_function(player_id);
		else
			temp_val = minimax(-1*player_id, depth-1, alpha, beta);
		if (player_id==-1 && temp_val>=best_val)
			best_val = temp_val;
		else if (player_id==1 && temp_val<=best_val)
			best_val = temp_val;

		// restoring board
		board[new_pos/N][new_pos%N] = 0;
		board[old_position/N][old_position%N] = player_id * (soldier_moved + 2);

		// restoring soldiers
		*(my_soldiers + soldier_moved) = old_position;

		if (player_id == -1)
			alpha = max(alpha, temp_val);
		else
			beta = min(beta, temp_val);
		if (alpha > beta)
			return temp_val;
	}
	int added=0;
	for (int i=0; i<fire_at.size(); i++)
	{
		int opp_soldier;
		int bombed = fire_at[i].first;
		// board and soldiers update
		int i_ = bombed % N;
		int j_ = bombed / N;
		int temp = board[j_][i_];

		if (added ==1 && temp==0)
			continue;
		if (temp==0)
			added=1;

		board[j_][i_] = 0;
		if (temp == -1*player_id)		// opponent's townhall
			townhalls[-1*player_id + 1]-=1;
		else if (temp!=0)		// opponent's soldier
		{
			opp_soldier = abs(temp)-2;
			if (*(opponent_soldiers + opp_soldier) != bombed)
				cerr<<"inconsistency in board and soldiers"<<endl;
			*(opponent_soldiers + opp_soldier) = -1;
			scount[-1*player_id + 1]-=1;
		}
		//minimax
		if (min(townhalls[0],townhalls[2]) <= 2)
			temp_val = evaluation_function(player_id);

		else if (depth == 0)
			temp_val = evaluation_function(player_id);
		else
			temp_val = minimax(-1*player_id, depth-1, alpha, beta);
		if (player_id==-1 && temp_val>=best_val)
			best_val = temp_val;
		else if (player_id==1 && temp_val<=best_val)
			best_val = temp_val;		

		// restoration
		board[j_][i_] = temp;
		if (temp == -1*player_id)		// opponent's townhall
			townhalls[-1*player_id + 1]+=1;
		else if (temp!=0)
		{
			*(opponent_soldiers + opp_soldier) = bombed;
			scount[-1*player_id + 1]+=1;
		}

		// alpha beta pruning
		if (player_id == -1)
			alpha = max(alpha, temp_val);
		else
			beta = min(beta, temp_val);
		if (alpha > beta)
			return temp_val;

		// if (bombed <= 0)
		// 	board[(-1*bombed)/N][(-1*bombed)%N] = -1 * player_id;
		// else
		// {
		// 	*(opponent_soldiers + bombed -2) = opp_position;
		// 	board[opp_position/N][opp_position%N] = -1 * player_id * bombed;
		// }
	}
	// return best_val;
	if (best_val == -1*best_val_static)
		return evaluation_function(player_id);
	return best_val;

}
string root_minimax(int player_id, int depth)
{
	clock_t t_ = clock();
	int counter = 0;		// number of times decreased

	int soldier_to_move, position_to_fire_or_move;
	char fire_or_move;
	float best_val = (player_id == -1)? MINI : MAXI;
	float best_val_static = best_val; 	float temp_val;
	int* my_soldiers = (player_id == -1)? soldiers1 : soldiers2;
	int* opponent_soldiers = (player_id == -1)? soldiers2 : soldiers1;
	vector<int> pos;

	float alpha = MINI;
	float beta = MAXI;
 	// int position, xposition, yposition,flag=0,number;
	// if (scount[1+player_id] == 1)
	// {
	// 	flag=1;
	// 	for (int i=0; i<soldier_number; i++)
	// 		if (opponent_soldiers[i] != -1)
	// 			break;
	// 	number = i;
	// 	position = opponent_soldiers[i];
	// 	xposition = opponent_soldiers[i]%N;
	// 	yposition = opponent_soldiers[i]/N;

	// }
	int flag = 0;
	if (scount[1+player_id] == 1)
		flag = 1;

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
			if (temp == -1*player_id)
			{
				townhalls[-1*player_id+1]-=1;
			}
			else if (temp*player_id<0)
			{
				killed = 1;
				opp_soldier_killed = abs(temp)-2;
				*(opponent_soldiers+opp_soldier_killed) = -1;
				scount[-1*player_id+1] -= 1;
				if (flag == 1)
				return ("S "+to_string(old_position%N) + " " + to_string(old_position/N) +" M "+to_string(i_)+" "+to_string(j_));
			}

			// minimax
			if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

			else if (depth == 0)
				temp_val = evaluation_function(player_id);
			else
			{
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 5 && counter==0)
				{
					cerr<<"5 sec passed, depth changed to "<<depth-1<<endl;
					counter = 1;
					depth--;
				}
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 10 && counter<=1)
				{
					cerr<<"10 sec passed, depth changed to "<<depth-1<<endl;
					counter++;
					depth--;
				}
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 15)
				{
					cerr<<"15 sec passed"<<endl;
					depth = 3;
				}
				temp_val = minimax(-1*player_id, depth-1, alpha, beta);
			}
			// if (player_id == 1)
			// 	cerr<<i<<" moved to "<<i_<<" "<<j_<<" gives score "<<temp_val<<endl;

			// cerr<<"soldier "<<i<<" moved to "<<pos[k]%N<<" "<<pos[k]/N<<" gives score "<<temp_val<<endl;
			if (player_id==-1 && (temp_val>best_val || isEqual(best_val,best_val_static) || (temp == -1*player_id)))
				{
					best_val = temp_val;
					soldier_to_move = i;
					if (*(my_soldiers + soldier_to_move) < 0) cerr<<"how is this possible"<<endl;
					position_to_fire_or_move = pos[k];
					fire_or_move = 'M';
				}
			else if (player_id==1 && (temp_val<best_val || isEqual(best_val, best_val_static) || (temp == -1*player_id)))
				{
					best_val = temp_val;
					soldier_to_move = i;
					if (*(my_soldiers + soldier_to_move) < 0) cerr<<"how is this possible"<<endl;
					position_to_fire_or_move = pos[k];
					fire_or_move = 'M';
				}

			// restoring board
			board[old_position/N][old_position%N] = player_id * (i+2);
			board[j_][i_] = temp;

			// restoring soldiers
			*(my_soldiers + i) = old_position;
			if (temp == -1*player_id)
				townhalls[-1*player_id + 1]+=1;
			if (killed)
			{
				scount[-1*player_id + 1]+=1;
				*(opponent_soldiers + opp_soldier_killed) = change_to_int(i_,j_);
			}

			// alpha beta pruning
			if (player_id == -1)		// maximizer
				alpha = max(alpha, temp_val);
			else 	// minimizer
				beta = min(beta, temp_val);
			// if (alpha >= beta)
			// 	return("S " + to_string(*(my_soldiers+i)%N) + " " + to_string(*(my_soldiers+i)/N) + " M " + to_string(pos[k]%N) + " " + to_string(pos[k]/N));
				// choose temp_val
		}
	}
	// cerr<<"Normal movements done "<<((float)(clock()-t))/(CLOCKS_PER_SEC)<<endl;
	vector<pair<int,int>> move_pos;
	vector<pair<int,int>> fire_at; 
	
	cannon_related(player_id, move_pos, fire_at);
	
	for (int i=0; i<move_pos.size(); i++)
	{
		int soldier_moved = move_pos[i].first;
		int new_pos = move_pos[i].second;
		int old_position = *(my_soldiers + soldier_moved);
		if (board[old_position/N][old_position%N] != (player_id*(soldier_moved+2)))
			cerr<<"SOME ERROR :("<<endl;

		// updating board
		board[new_pos/N][new_pos%N] = player_id * (soldier_moved + 2);
		board[old_position/N][old_position%N] = 0;

		// updating soldiers
		*(my_soldiers + soldier_moved) = new_pos;

		// cerr<<"soldier "<<soldier_moved<<" moved to "<<new_pos%N<<" "<<new_pos/N<<" gives score "<<temp_val<<endl;

		// minimax
		if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

		else if (depth == 0)
			temp_val = evaluation_function(player_id);
		else
		{
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 5 && counter==0)
				{
					cerr<<"5 sec passed, depth changed to "<<depth-1<<endl;
					counter = 1;
					depth--;
				}
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 10 && counter<=1)
				{
					cerr<<"10 sec passed, depth changed to "<<depth-1<<endl;
					counter++;
					depth--;
				}
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 15)
				{
					cerr<<"15 sec passed"<<endl;
					depth = 3;
				}
			temp_val = minimax(-1*player_id, depth-1, alpha, beta);
		}

		if (player_id==-1 && (temp_val>best_val || isEqual(best_val, best_val_static)))
		{
			best_val = temp_val;
			soldier_to_move = soldier_moved;
			position_to_fire_or_move = new_pos;
			fire_or_move = 'M';
		}
		else if (player_id==1 && (temp_val<best_val || isEqual(best_val, best_val_static)))
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
		// cerr<<" movement to "<<new_pos%N<<" " <<new_pos/N<<" done "<<((float)(clock()-t))/(CLOCKS_PER_SEC)<<endl;

		// alpha beta puning
		if (player_id == -1)	// maximizer
			alpha = max(alpha, temp_val);
		else
			beta = min(beta, temp_val);
	}
	// cerr<<"Cannon movements done "<<((float)(clock()-t))/(CLOCKS_PER_SEC)<<endl;
	int added = 0;
	for (int i=0; i<fire_at.size(); i++)
	{
		int opp_soldier;
		int bombed = fire_at[i].first;
		// board and soldiers update
		int i_ = bombed % N;
		int j_ = bombed / N;
		int temp = board[j_][i_];

		if (added ==1 && temp==0)
			continue;
		if (temp==0)
			added=1;

		board[j_][i_] = 0;
		if (temp == -1*player_id)		// opponent townhall
		{
			townhalls[-1*player_id+1] -= 1;
		}
		else if (temp!=0)		// opponent's soldier
		{
			opp_soldier = abs(temp)-2;
			if (*(opponent_soldiers + opp_soldier) != bombed)
				cerr<<"inconsistency in board and soldiers :("<<endl;
			*(opponent_soldiers + opp_soldier) = -1;
			scount[-1*player_id+1]-=1;

			if (flag == 1)
			return("S "+to_string((fire_at[i].second)%N)+" "+to_string((fire_at[i].second)/N)+ " B "+to_string(i_)+" "+to_string(j_));
		}

		//minimax
		if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

		else if (depth == 0)
			temp_val = evaluation_function(player_id);
		else
		{
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 5 && counter==0)
				{
					cerr<<"5 sec passed, depth changed to "<<depth-1<<endl;
					counter = 1;
					depth--;
				}
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 10 && counter<=1)
				{
					cerr<<"10 sec passed, depth changed to "<<depth-1<<endl;
					counter++;
					// cerr<<counter<<endl;
					depth--;
				}
				if ((clock()-t_)/(float)CLOCKS_PER_SEC > 15)
				{
					cerr<<"15 sec passed"<<endl;
					depth = 3;
				}
			temp_val = minimax(-1*player_id, depth-1, alpha, beta);
		}

		// cerr<<"soldier "<<fire_at[i].second<<" shoots at "<<i_<<" "<<j_<<" gives score "<<temp_val<<endl;
		if (player_id==-1 && (temp_val>best_val || (temp_val==best_val && temp!=0)))
		{
			best_val = temp_val;
			soldier_to_move = fire_at[i].second;
			position_to_fire_or_move = bombed;
			fire_or_move = 'B';
		}
		else if (player_id==1 && (temp_val<best_val || (temp_val==best_val && temp!=0)))
		{
			best_val = temp_val;
			soldier_to_move = fire_at[i].second;
			position_to_fire_or_move = bombed;
			fire_or_move = 'B';
		}

		// restoration
		board[j_][i_] = temp;
		if (temp == -1*player_id)
			townhalls[-1*player_id+1]+=1;
		else if (temp!=0)
		{
			scount[-1*player_id+1] += 1;
			*(opponent_soldiers + opp_soldier) = bombed;
		}
		// cerr<<" firing at "<<i_<<" " <<j_<<" done "<<((float)(clock()-t))/(CLOCKS_PER_SEC)<<endl;

		// alpha beta pruning
		if (player_id == -1)	// maximizer
			alpha = max(alpha, temp_val);
		else
			beta = min(beta, temp_val);
		// if (alpha >= beta)
		// 	return ("S " + to_string(*(my_soldiers+fire_at[i].second)%N) + " " + to_string(*(my_soldiers+fire_at[i].second)/N) + " B " + to_string(bombed%N) + " " + to_string(bombed/N));

	}
	// cerr<<"Cannon firing done "<<((float)(clock()-t))/(CLOCKS_PER_SEC)<<endl;

	// we have the best move
	if (*(my_soldiers + soldier_to_move) < 0)
	{
		cerr<<player_id<<" "<<soldier_to_move<<" "<<fire_or_move<<endl;
		cerr<<"best value: "<<best_val<<endl;
		cerr<<"whyR?"<<endl;
	}
	if (position_to_fire_or_move < 0)
		cerr<<"whyR??"<<endl;

	// IMPLEMENT THE BEST MOVE

	// RETURN THE BEST MOVE
	string best_move = "S " + to_string(*(my_soldiers+soldier_to_move)%N) + " " + to_string(*(my_soldiers+soldier_to_move)/N) + " " + (fire_or_move) + " " + to_string(position_to_fire_or_move%N) + " " + to_string(position_to_fire_or_move/N);

	// cerr<<"TIMING ANALYSIS at depth "<<depth<<endl;
	// cerr<<"possible_moves2_time: "<<possible_moves2_time<<" ,cannon_related_time: "<<cannon_related_time<<" ,evaluation_function_time: "<<evaluation_function_time<<endl;
	cerr<<best_move<<endl;
	play_move2(best_move, -1*player_id);
	return best_move;
}

int main(int argc, char const *argv[])
{
	int player_id_recv=0, timelimit=10, player_id;

	// DOUBT - take in N then M ya ulta?
	cin>>player_id_recv>>M>>N>>timelimit;
	soldiers_number = 3 * (N/2);
	// cerr<<"Number of soldiers: "<<soldiers_number<<endl;
	// cerr<<"Computer is player: "<<player_id_recv<<endl;
	if (player_id_recv == 1)
		player_id = -1;
	if (player_id_recv == 2)
		player_id = 1;

	// cerr<<"I am player_id "<<player_id<<endl;
	clock_t t, t1, t2;
	float time_taken,total_time=0,opponent_time;

	initialise();
	// print_board();

	string move;
	getline(cin, move);
	if (player_id == 1)
	{
		cerr<<"waiting for move from opponent"<<endl;
		t1 = clock();
		getline(cin, move);
		while (move == "")
			getline(cin, move);
		t2 = clock();
		opponent_time += ((float)(t2-t1))/CLOCKS_PER_SEC;
		play_move2(move, player_id);
		print_board();
	}

	string s;
	int i=0;
	int minimax_depth = 4,fast_count=0,fast_cutoff=3;
	float low_cutoff=1,high_cutoff=8;
	while(true)
	{
		i++;
		t = clock();
		s = root_minimax(player_id,minimax_depth);
		cout<<s<<endl;
		time_taken = (clock()-t)/(float)CLOCKS_PER_SEC;
		cerr<<"time taken: "<<time_taken<<endl;
		total_time+=time_taken;
		time_left = timelimit - total_time;

		if ((time_left < 10) && (opponent_time > 10))
			minimax_depth = 3;
		else
		{
			if (fast_count==2 && minimax_depth==3)
			{
				minimax_depth++;
				cerr<<"Depth Increased to "<<minimax_depth<<endl;
				fast_count=0;
			}
			if( time_left > 70 )
			{
				low_cutoff=2;
				high_cutoff=8;
			}
			else if( time_left > 30 )
			{
				low_cutoff=3;
				high_cutoff=6;
			}
			else if( total_time > 10)
			{
				low_cutoff = 1.5;
				high_cutoff = 6;
				fast_cutoff =5;
			}
			else
			{
				low_cutoff=0;
				high_cutoff =2;
			}
			if(time_taken<low_cutoff)
				fast_count++;
			if(fast_count >= fast_cutoff)
			{
				fast_count =0;
				minimax_depth = min(minimax_depth+1, 7);
				cerr<<"Depth Increased to "<<minimax_depth<<endl;
			}
			if(time_taken> high_cutoff)
			{
				minimax_depth--;
				cerr<<"Depth decreased to "<<minimax_depth<<endl;
				fast_count = 0;
			}				
		}

		// cerr<<"waiting for move from opponent"<<endl;
		t1 = clock();
		getline(cin, move);
		while (move == "")
			getline(cin, move);
		t2 = clock();
		opponent_time += (t2-t1)/(float)CLOCKS_PER_SEC;
		// cerr<<"move received: "<<move<<endl;
		play_move2(move, player_id);
		if (townhalls[2] <= 2)
		{
			cerr<<"Player 1 wins!";
			break;
		}
		else if (townhalls[0] <= 2)
		{
			cerr<<"Player 2 wins!";
			break;
		}
	}
	return 0;
}