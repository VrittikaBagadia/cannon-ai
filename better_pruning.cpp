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

typedef unsigned long long int UINT64;

#define MAXI 100000000
#define MINI -100000000
#define tablesize 20

int board[10][10];
int soldiers1[15];
int soldiers2[15];
int townhalls[3];
int scount[3];
UINT64 table[100][4];
map<UINT64, float> mapping[tablesize];
UINT64 hashvalue;


int counti[tablesize];

int N, M;		// M is the number of rows and N is the number of columns
int soldiers_number;

// int player_number = 0; 		// -1 for black, 1 for white
bool greater_float(pair<float,vector<int> > a, pair<float,vector<int> > b){
  return a.first > b.first;
}
int change_to_int(int x, int y)
{
	return (y*N + x);
}

int what_piece(int num)
{
	if (num == -1)
		return 1;	// black townhall
	if (num == 1)
		return 3;	// white townhall
	if (num < 0)	
		return 0;	// black soldier
	return 2;		// white soldier
}

UINT64 find_hash()
{
	UINT64 hash = 0;
	// int piece;
	// can traverse soldiers instead of entire board
	for (int i=0; i<M; i++)
		for (int j=0; j<N; j++)
		{
			if (board[i][j] == 0)
				continue;
			hash ^= table[change_to_int(i,j)][what_piece(board[i][j])];
		}
	return hash;
}

void initTable()
{
	random_device rd;
  	default_random_engine generator(rd());
  	generator.seed(0);
  	uniform_int_distribution<long long unsigned> distribution(0, 0xFFFFFFFFFFFFFFFF);

	for (int i = 0; i<M*N; i++) 
    	for (int j = 0; j<4; j++) 
        	table[i][j] = (uint64_t)(distribution(generator));
}

void print_board()
{
	for (int i=0; i<M; i++)
	{
		for (int j=0; j<N; j++)
			cerr<<setw(3)<<board[i][j]<<" ";
		cerr<<endl;
	}
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
int possible_moves_number(int player_id)
{
	vector<int> temp;
	int ans=0;
	if (player_id == -1)
	{
		for (int i=0; i<soldiers_number; i++)
		{
			if (soldiers1[i] == -1)
				continue;
			possible_moves2(i,player_id,temp);
			ans += temp.size();
			if (ans > 0)
				return 1;
		}
	}
	else if (player_id == 1)
	{
		for (int i=0; i<soldiers_number; i++)
		{
			if (soldiers2[i] == -1)
				continue;
			possible_moves2(i,player_id,temp);
			ans += temp.size();
			if (ans > 0)
				return 1;
		}
	}
	return 0;
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

void townhalls_attacked_by_soldiers(int &black_townhalls, int &white_townhalls)			// CHANGE THIS
{
	black_townhalls=0;
	white_townhalls=0;
	for(int i=0;i<N;i+=2)
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

	for(int i=1;i<N;i+=2)
	{
		bool f=false;
		if(board[M-1][i]==-1)
		{
			if(i+1<N)
				{
					if(board[M-1][i+1]>0 || board[M-2][i+1]>0)
						f=true;
				}
			if(board[M-2][i]>0 || board[M-1][i-1]>0 || board[M-2][i-1]>0)
				f=true;
		}

		if(f)
			black_townhalls++;
	}

	// return (white_townhalls - black_townhalls);
}

int attacking_soldiers(int player_id)
{
	int a=0;

	if(player_id==-1)
	{
		for(int i =0;i<N;i++)
		{
			if(board[M-3][i]>0)
				{
					bool defend =false;
					for(int j=M-3;j<=M-1;j++)
					{
						if(board[j][max(i-1,0)]<-1 ||	board[j][i]<-1 || board[j][min(i+1,N-1)]<-1 )
							defend =true;
					}
					if(!defend)
						a++;
				}
		}			
	}

	else if(player_id == 1)
	{
		for(int i =0;i<N;i++)
		{
			if(board[2][i]<0)
				{
					bool defend =false;
					for(int j=0;j<=2;j++)
					{
						if(board[j][max(i-1,0)]>1 ||	board[j][i]>1 || board[j][min(i+1,N-1)]>1 )
							defend =true;
					}
					if(!defend)
						a++;
				}
		}			
	}

	return a;
}

void secure_townhalls(int &not_secure_white, int &not_secure_black)
{
	// a soldier near townhall is good
	not_secure_white = 0; not_secure_black = 0;
	for (int i=0; i<N; i+=2)
	{
		if (board[0][i] == 1)
		{
			if (board[0][i+1]>0 || board[1][i]>0 || board[1][i+1]>0 || board[2][i]>0 || board[2][i+1]>0)
				continue;
			else if (i-1>=0 && (board[0][i-1]>0 || board[1][i-1]>0 || board[2][i-1]>0))
				continue;
			else
				not_secure_white+=1;
		}
	}
	for (int i=1; i<N; i+=2)
	{
		if (board[M-1][i]==-1)
		{
			if (board[M-1][i-1]<0 || board[M-2][i-1]<0 || board[M-3][i-1]<0 || board[M-2][i]<0 || board[M-3][i]<0)
				continue;
			else if (i+1<N && (board[M-1][i+1]<0 || board[M-2][i+1]<0 || board[M-3][i+1]<0))
				continue;
		}	else
			not_secure_black+=1;
	}
}

float evaluation_function(int player_id, int num1=1, int num2=1)
{
	float score;
	int number_of_cannons0, number_of_cannons2, townhalls_under_fire0, townhalls_under_fire2, soldiers_under_fire0, soldiers_under_fire2, number_of_cannons_horiz0, number_of_cannons_horiz2=0;
	int not_secure_white=0, not_secure_black=0;
	
	// if (townhalls[0] <= 2)
	// 	return -1000;
	// if (townhalls[2] <= 2)
	// 	return 1000;

	cannon_related_2(1, number_of_cannons2, number_of_cannons_horiz2 ,townhalls_under_fire0, soldiers_under_fire0);
	cannon_related_2(-1, number_of_cannons0, number_of_cannons_horiz0, townhalls_under_fire2, soldiers_under_fire2);
	int attacked_townhalls_black, attacked_townhalls_white;
	townhalls_attacked_by_soldiers(attacked_townhalls_black, attacked_townhalls_white);
	secure_townhalls(not_secure_white, not_secure_black);

	int attacking_soldiers_black = 0, attacking_soldiers_white=0;
	if (min(scount[0],scount[1]) <=8)
	{
		attacking_soldiers_white = attacking_soldiers(-1);
		attacking_soldiers_black = attacking_soldiers(1);
	}

	float black_score = 150*townhalls[0] +  30*scount[0] - 50*townhalls_under_fire0 - 40*attacked_townhalls_black - 5*not_secure_black + 5*attacking_soldiers_black;
	float white_score = 150*townhalls[2] +  30*scount[2] - 50*townhalls_under_fire2 - 40*attacked_townhalls_white - 5*not_secure_white + 5*attacking_soldiers_white;;

	if (player_id == -1)
		score = 0.6*black_score - 0.4*white_score;
		// score = black_score - white_score;
	else
		score = 0.4*black_score - 0.6*white_score;
	
	// score += 6*(number_of_cannons0-number_of_cannons2) + 2*(number_of_cannons_horiz0-number_of_cannons_horiz2)+0.5*(soldiers_under_fire2 - soldiers_under_fire0);

	// score = black_score - white_score;
	score += 6*(number_of_cannons0-number_of_cannons2) + 2*(number_of_cannons_horiz0-number_of_cannons_horiz2)+1*(soldiers_under_fire2 - soldiers_under_fire0);

	if (min(scount[0],scount[2]) <=4)
		score += 15*(scount[0] - scount[2]);
	
	// CHECK
	// both can be <= 2
	if (townhalls[0] <= 2)
		score = (-1000+score);
	if (townhalls[2] <= 2)
		score = (1000+score);

	if (scount[2] == 0)
	{
		// cerr<<"?????????????  checked1\n";
		score += 300*(townhalls[0]-townhalls[2] + 2);
	}

	if (scount[0] == 0)
	{
		// cerr<<"?????????????  checked2\n";
		score += (-300)*(townhalls[2]-townhalls[0] + 2);
	}

	// int num1, num2;
	if (num2 == 0)		// no possible moves for player 2
	{
		// cerr<<"?????????????  checked3\n";
		score += 400*(townhalls[0]-townhalls[2] + 0.5);
	}
	if (num1 == 0)
	{
		// cerr<<"?????????????  checked4\n";
		score += (-400)*(townhalls[2]-townhalls[0] + 0.5);
	}
	return score;
}
int play_move(int x1, int y1, char ch2, int x2, int y2, int player_id)		// player_id has received this move from opponent
{
	int *opponent_soldiers = (player_id==-1)? soldiers2:soldiers1;
	int *my_soldiers = (player_id==-1)? soldiers1:soldiers2;
	int ans=0;
	if (ch2 == 'B')
	{
		int temp = board[y2][x2];
		board[y2][x2] = 0;
		if (temp == 0)
			ans = 1;
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
	return ans;
}

int play_move2(string s, int player_id)
{
	return (play_move(s[2]-48, s[4]-48, s[6], s[8]-48, s[10]-48, player_id));
}


float minimax(int player_id, int depth , float alpha, float beta)
{
	// look for value in higher depth maps ow find out and put in map
	float val; int found=0;
	for (int i=depth; i<tablesize; i+=2)
	{
		if (mapping[i].find(hashvalue) != mapping[i].end())
		{
			found = 1;
			val = mapping[i][hashvalue];
		}
	}
	if (found == 1)
	{
		counti[depth]++;
		return val;
	}

	// cerr<<"depth: "<<depth<<endl;
	float best_val = (player_id == -1)? MINI : MAXI; 		// player -1 maximises, +1 minimises
	float best_val_static = -1*best_val; 
	float temp_val, temp_variable;
	int* my_soldiers = (player_id==-1)? soldiers1 : soldiers2;
	int* opponent_soldiers = (player_id==-1)? soldiers2 : soldiers1;
	vector<int> pos;

	vector<pair<float,vector<int> > > mapped_evaluation;
	mapped_evaluation.clear();
	int num1, num2;

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
				*(opponent_soldiers + opp_soldier_killed) = -1;
				scount[-1*player_id + 1]-=1;
			}
			
			pair<float,vector<int> > intermediate;
			intermediate.first = evaluation_function(player_id);
			intermediate.second.clear();
			intermediate.second.push_back(0);
			intermediate.second.push_back(i);
			intermediate.second.push_back(pos[k]);
			

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

			if (depth == 0)
			{
				if (player_id==-1 && intermediate.first>=best_val)
				{
					best_val = intermediate.first;
					alpha = max(alpha, intermediate.first);
				}
				else if (player_id==1 && intermediate.first<=best_val)
				{
					best_val = intermediate.first;
					beta = min(beta, intermediate.first);
				}
				if (alpha > beta)
				{
					mapping[depth].insert(pair<UINT64,float>(hashvalue,temp_val));
					return temp_val;
				}				
			}
			else
				mapped_evaluation.push_back(intermediate);
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

		// updating board
		board[new_pos/N][new_pos%N] = player_id * (soldier_moved + 2);
		board[old_position/N][old_position%N] = 0;
		// updating soldiers
		*(my_soldiers + soldier_moved) = new_pos;

		pair<float,vector<int> > intermediate;
		intermediate.first = evaluation_function(player_id);
		intermediate.second.clear();
		intermediate.second.push_back(1);
		intermediate.second.push_back(soldier_moved);
		intermediate.second.push_back(new_pos);

		// restoring board
		board[new_pos/N][new_pos%N] = 0;
		board[old_position/N][old_position%N] = player_id * (soldier_moved + 2);
		// restoring soldiers
		*(my_soldiers + soldier_moved) = old_position;

		if (depth == 0)
		{
			if (player_id==-1 && intermediate.first>=best_val)
			{
				best_val = intermediate.first;
				alpha = max(alpha, intermediate.first);
			}
			else if (player_id==1 && intermediate.first<=best_val)
			{
				best_val = intermediate.first;
				beta = min(beta, intermediate.first);
			}
			if (alpha > beta)
			{
				mapping[depth].insert(pair<UINT64,float>(hashvalue,temp_val));
				return temp_val;
			}				
		}
		else
			mapped_evaluation.push_back(intermediate);
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
		if (temp == -1*player_id)				// opponent's townhall
			townhalls[-1*player_id + 1]-=1;
		else if (temp!=0)						// opponent's soldier
		{
			opp_soldier = abs(temp)-2;
			*(opponent_soldiers + opp_soldier) = -1;
			scount[-1*player_id + 1]-=1;
		}

		pair<float,vector<int> > intermediate;
		intermediate.first = evaluation_function(player_id);
		intermediate.second.clear();
		intermediate.second.push_back(2);
		intermediate.second.push_back(bombed);
		// intermediate.second.push_back();

		// restoration
		board[j_][i_] = temp;
		if (temp == -1*player_id)		// opponent's townhall
			townhalls[-1*player_id + 1]+=1;
		else if (temp!=0)
		{
			*(opponent_soldiers + opp_soldier) = bombed;
			scount[-1*player_id + 1]+=1;
		}

		if (depth == 0)
		{
			if (player_id==-1 && intermediate.first>=best_val)
			{
				best_val = intermediate.first;
				alpha = max(alpha, intermediate.first);
			}
			else if (player_id==1 && intermediate.first<=best_val)
			{
				best_val = intermediate.first;
				beta = min(beta, intermediate.first);
			}
			if (alpha > beta)
			{
				mapping[depth].insert(pair<UINT64,float>(hashvalue,temp_val));
				return temp_val;
			}				
		}
		else
			mapped_evaluation.push_back(intermediate);
	}

	if (depth == 0)
	{
		if (best_val == -1*best_val_static)
		{
			float ans = evaluation_function(player_id);
			mapping[depth].insert(pair<UINT64,float>(hashvalue,ans));
			return ans;
		}
		mapping[depth].insert(pair<UINT64,float>(hashvalue,best_val));
		return best_val;
	}

	if (player_id == -1)
		sort(mapped_evaluation.begin(), mapped_evaluation.end(), greater_float);
	else
		sort(mapped_evaluation.begin(), mapped_evaluation.end());

	// execute these moves
	for (int ind = 0; ind < mapped_evaluation.size(); ind++)
	{
		if (mapped_evaluation[ind].second[0] == 0)
		{
			// soldier movement
			int i = mapped_evaluation[ind].second[1];
			int posk = mapped_evaluation[ind].second[2];

			int i_ = posk%N;
			int j_ = posk/N;
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
			else if (temp*player_id < 0)
			{
				killed = 1;
				opp_soldier_killed = abs(temp)-2;
				opp_old_position = *(opponent_soldiers + opp_soldier_killed);
				*(opponent_soldiers + opp_soldier_killed) = -1;
				scount[-1*player_id + 1]-=1;
			}
			// update hashvalue
			hashvalue ^= table[old_position][player_id+1];			// old position se hatao
			hashvalue ^= table[posk][player_id+1];				// new position pe daalo
			if (temp == -1*player_id)								// opponent townhall at new position
				hashvalue ^= table[posk][temp+2];					
			else if (killed == 1)									// opponent soldier at new position
				hashvalue ^= table[posk][-1*player_id + 1];

			// minimax
			if (min(scount[0], scount[2]) <= 4)
			{
				num1 = possible_moves_number(-1);
				num2 = possible_moves_number(+1);
			}

			if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);
			else if (min(scount[0],scount[2]) == 0)
				temp_val = evaluation_function(player_id);

			else if ((min(scount[0], scount[2]) <= 4) && min(num1,num2) == 0)
				temp_val = evaluation_function(player_id,num1,num2);
			else if (depth == 0)
				temp_val = evaluation_function(player_id);
			else
				temp_val = minimax(-1*player_id, depth-1, alpha, beta);

			if (player_id==-1 && temp_val>=best_val)
				best_val = temp_val;
			else if (player_id==1 && temp_val<=best_val)
				best_val = temp_val;

			// restore
			hashvalue ^= table[old_position][player_id+1];			// old position se hatao
			hashvalue ^= table[posk][player_id+1];				// new position pe daalo
			if (temp == -1*player_id)								// opponent townhall at new position
				hashvalue ^= table[posk][temp+2];					
			else if (killed == 1)									// opponent soldier at new position
				hashvalue ^= table[posk][-1*player_id + 1];
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
			{
				mapping[depth].insert(pair<UINT64,float>(hashvalue,temp_val));
				return temp_val;
			}
		}
		else if (mapped_evaluation[ind].second[0] == 1)
		{
			// cannon movement
			int soldier_moved = mapped_evaluation[ind].second[1];
			int new_pos = mapped_evaluation[ind].second[2];
			int old_position = *(my_soldiers + soldier_moved);

			// updating board
			board[new_pos/N][new_pos%N] = player_id * (soldier_moved + 2);
			board[old_position/N][old_position%N] = 0;
			// updating soldiers
			*(my_soldiers + soldier_moved) = new_pos;
			// update hash value
			hashvalue ^= table[old_position][player_id+1];			// old position se hatao
			hashvalue ^= table[new_pos][player_id+1];				// new position pe daalo

			// minimax
			if (min(scount[0], scount[2]) <= 4)
			{
				num1 = possible_moves_number(-1);
				num2 = possible_moves_number(+1);
			}
			if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);
			else if (depth == 0)
				temp_val = evaluation_function(player_id);
			else if (min(scount[0],scount[2]) == 0)
				temp_val = evaluation_function(player_id);

			else if ((min(scount[0], scount[2]) <= 4) && min(num1,num2) == 0)
				temp_val = evaluation_function(player_id,num1,num2);
			else
				temp_val = minimax(-1*player_id, depth-1, alpha, beta);

			if (player_id==-1 && temp_val>=best_val)
				best_val = temp_val;
			else if (player_id==1 && temp_val<=best_val)
				best_val = temp_val;

			// restore hash value
			hashvalue ^= table[old_position][player_id+1];			// old position se hatao
			hashvalue ^= table[new_pos][player_id+1];				// new position pe daalo
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
			{
				mapping[depth].insert(pair<UINT64,float>(hashvalue,temp_val));
				return temp_val;
			}
		}
		else
		{
			// cannon firing
			int bombed = mapped_evaluation[ind].second[1];
			int opp_soldier;
			// board and soldiers update
			int i_ = bombed % N;
			int j_ = bombed / N;
			int temp = board[j_][i_];
			board[j_][i_] = 0;
			if (temp == -1*player_id)		// opponent's townhall
				townhalls[-1*player_id + 1]-=1;
			else if (temp!=0)		// opponent's soldier
			{
				opp_soldier = abs(temp)-2;
				*(opponent_soldiers + opp_soldier) = -1;
				scount[-1*player_id + 1]-=1;
			}

			// update hash value
			if (temp == -1*player_id)
				hashvalue ^= table[bombed][-1*player_id + 2];
			else if (temp != 0)
				hashvalue ^= table[bombed][-1*player_id + 1];

			//minimax
			if (min(scount[0], scount[2]) <= 4)
			{
				num1 = possible_moves_number(-1);
				num2 = possible_moves_number(+1);
			}
			if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

			else if (min(scount[0],scount[2]) == 0)
				temp_val = evaluation_function(player_id);

			else if ((min(scount[0], scount[2]) <= 4) && min(num1,num2) == 0)
				temp_val = evaluation_function(player_id,num1,num2);

			else if (depth == 0)
				temp_val = evaluation_function(player_id);
			else
				temp_val = minimax(-1*player_id, depth-1, alpha, beta);
			if (player_id==-1 && temp_val>=best_val)
				best_val = temp_val;
			else if (player_id==1 && temp_val<=best_val)
				best_val = temp_val;	

			// restore hash value
			if (temp == -1*player_id)
				hashvalue ^= table[bombed][-1*player_id + 2];
			else if (temp != 0)
				hashvalue ^= table[bombed][-1*player_id + 1];	
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
			{
				mapping[depth].insert(pair<UINT64,float>(hashvalue,temp_val));
				return temp_val;
			}

		}
	}
	// return best_val;
	if (best_val == -1*best_val_static)
	{
		float ans = evaluation_function(player_id);
		mapping[depth].insert(pair<UINT64,float>(hashvalue,ans));
		return ans;
	}
	mapping[depth].insert(pair<UINT64,float>(hashvalue,best_val));
	return best_val;
}

string root_minimax(int player_id, int depth, int &repeat, int &emptym)
{	
	emptym = 0;
	// clear mapping
	for (int i=0; i<tablesize; i++)
		mapping[i].clear();
	hashvalue = find_hash();

	for (int i=0; i<tablesize; i++)
		counti[i] = 0;

	int piece;
	int num1, num2;

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
			if ((clock()-t_)/(float)CLOCKS_PER_SEC > 10)
			{
				cerr<<"10 sec passed, restart...\n";
				repeat = 1;
				return "";
			}
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
				// if (flag == 1)
				// return ("S "+to_string(old_position%N) + " " + to_string(old_position/N) +" M "+to_string(i_)+" "+to_string(j_));
			}

			// -----------------
			// 1. update hash value - player id's soldier being moved
			hashvalue ^= table[old_position][player_id+1];			// old position se hatao
			hashvalue ^= table[pos[k]][player_id+1];				// new position pe daalo
			if (temp == -1*player_id)								// opponent townhall at new position
				hashvalue ^= table[pos[k]][temp+2];					
			else if (killed == 1)									// opponent soldier at new position
				hashvalue ^= table[pos[k]][-1*player_id + 1];
			// -----------------
			

			// minimax
			if (min(scount[0], scount[2]) <= 4)
			{
				num1 = possible_moves_number(-1);
				num2 = possible_moves_number(+1);
			}

			if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

			else if (min(scount[0],scount[2]) == 0)
				temp_val = evaluation_function(player_id);

			else if ((min(scount[0], scount[2]) <= 4) && min(num1,num2) == 0)
				temp_val = evaluation_function(player_id,num1,num2);

			else if (depth == 0)
				temp_val = evaluation_function(player_id);
			else
				temp_val = minimax(-1*player_id, depth-1, alpha, beta);
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

			cerr<<"S "<<old_position%N<<" "<<old_position/N<<" M "<<i_<<" "<<j_<<" : "<<temp_val<<endl;

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

			// -----------------
			// 1. restore hash value - player id's soldier being moved
			hashvalue ^= table[old_position][player_id+1];			// old position se hatao
			hashvalue ^= table[pos[k]][player_id+1];				// new position pe daalo
			if (temp == -1*player_id)								// opponent townhall at new position
				hashvalue ^= table[pos[k]][temp+2];					
			else if (killed == 1)									// opponent soldier at new position
				hashvalue ^= table[pos[k]][-1*player_id + 1];
			// -----------------

		}
	}

	vector<pair<int,int>> move_pos;
	vector<pair<int,int>> fire_at;
	cannon_related(player_id, move_pos, fire_at);
	for (int i=0; i<move_pos.size(); i++)
	{
		if ((clock()-t_)/(float)CLOCKS_PER_SEC > 10)
		{
			cerr<<"10 sec passed, restart...\n";
			repeat = 1;
			return "";
		}

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

		// 1. update hash value - player id's soldier being moved
		hashvalue ^= table[old_position][player_id+1];			// old position se hatao
		hashvalue ^= table[new_pos][player_id+1];				// new position pe daalo
		// -----------------

		// minimax
		if (min(scount[0], scount[2]) <= 4)
		{
			num1 = possible_moves_number(-1);
			num2 = possible_moves_number(+1);
		}

		if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

		else if (min(scount[0],scount[2]) == 0)
				temp_val = evaluation_function(player_id);

		else if ((min(scount[0], scount[2]) <= 4) && min(num1,num2) == 0)
			temp_val = evaluation_function(player_id,num1,num2);

		else if (depth == 0)
			temp_val = evaluation_function(player_id);
		else
			temp_val = minimax(-1*player_id, depth-1, alpha, beta);

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

		cerr<<"S "<<old_position%N<<" "<<old_position/N<<" "<<new_pos%N<<" "<<new_pos/N<<" : "<<temp_val<<endl;

		// restoring board
		board[new_pos/N][new_pos%N] = 0;
		board[old_position/N][old_position%N] = player_id * (soldier_moved + 2);

		// restoring soldiers
		*(my_soldiers + soldier_moved) = old_position;

		// alpha beta puning
		if (player_id == -1)	// maximizer
			alpha = max(alpha, temp_val);
		else
			beta = min(beta, temp_val);

		// 1. restore hash value - player id's soldier being moved
		hashvalue ^= table[old_position][player_id+1];			// old position se hatao
		hashvalue ^= table[new_pos][player_id+1];				// new position pe daalo
		// -----------------
	}
	
	int added = 0;
	for (int i=0; i<fire_at.size(); i++)
	{
		if ((clock()-t_)/(float)CLOCKS_PER_SEC > 10)
		{
			cerr<<"10 sec passed, restart..\n";
			repeat = 1;
			return "";
		}
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

			// if (flag == 1)
			// return("S "+to_string((fire_at[i].second)%N)+" "+to_string((fire_at[i].second)/N)+ " B "+to_string(i_)+" "+to_string(j_));
		}

		// 1. update hash value
		if (temp == -1*player_id)
			hashvalue ^= table[bombed][-1*player_id + 2];
		else if (temp != 0)
			hashvalue ^= table[bombed][-1*player_id + 1];
		// -----------------

		//minimax
		if (min(scount[0], scount[2]) <= 4)
		{
			num1 = possible_moves_number(-1);
			num2 = possible_moves_number(+1);
		}

		if (min(townhalls[0],townhalls[2]) <= 2)
				temp_val = evaluation_function(player_id);

		else if (min(scount[0],scount[2]) == 0)
			temp_val = evaluation_function(player_id);

		else if ((min(scount[0], scount[2]) <= 4) && min(num1,num2) == 0)
			temp_val = evaluation_function(player_id,num1,num2);

		else if (depth == 0)
			temp_val = evaluation_function(player_id);
		else
			temp_val = minimax(-1*player_id, depth-1, alpha, beta);
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

		cerr<<"S B "<<i_<<" "<<j_<<" : "<<temp_val<<endl;

		// restoration
		board[j_][i_] = temp;
		if (temp == -1*player_id)
			townhalls[-1*player_id+1]+=1;
		else if (temp!=0)
		{
			scount[-1*player_id+1] += 1;
			*(opponent_soldiers + opp_soldier) = bombed;
		}

		// alpha beta pruning
		if (player_id == -1)	// maximizer
			alpha = max(alpha, temp_val);
		else
			beta = min(beta, temp_val);

		// 1. restore hash value
		if (temp == -1*player_id)
			hashvalue ^= table[bombed][-1*player_id + 2];
		else if (temp != 0)
			hashvalue ^= table[bombed][-1*player_id + 1];
		// -----------------

	}

	// we have the best move
	if (*(my_soldiers + soldier_to_move) < 0)
	{
		cerr<<player_id<<" "<<soldier_to_move<<" "<<fire_or_move<<endl;
		cerr<<"best value: "<<best_val<<endl;
		cerr<<"whyR?"<<endl;
	}
	if (position_to_fire_or_move < 0)
		cerr<<"whyR??"<<endl;

	// cerr<<"hashvalue end: "<<hashvalue<<endl;
	// for (int i=0; i<8; i++)
	// 	cerr<<counti[i]<<" ; ";
	// IMPLEMENT THE BEST MOVE

	// RETURN THE BEST MOVE
	string best_move = "S " + to_string(*(my_soldiers+soldier_to_move)%N) + " " + to_string(*(my_soldiers+soldier_to_move)/N) + " " + (fire_or_move) + " " + to_string(position_to_fire_or_move%N) + " " + to_string(position_to_fire_or_move/N);

	// cerr<<best_move<<endl;
	emptym = play_move2(best_move, -1*player_id);
	// if (empty_move == 1)
	// 	empty_count[player_id+1]++;
	// else
	// 	empty_count[player_id+1]=0;
	return best_move;
}
int main(int argc, char const *argv[])
{
	cerr<<"newest\n";
	int player_id_recv=0, timelimit=90, player_id;

	// DOUBT - take in N then M ya ulta?
	cin>>player_id_recv>>M>>N>>timelimit;
	soldiers_number = 3 * (N/2);
	if (player_id_recv == 1)
		player_id = -1;
	if (player_id_recv == 2)
		player_id = 1;
	initTable();
	clock_t t, t1, t2;
	float time_taken,total_time=0,opponent_time, time_left;

	initialise();
	// print_board();
	int empty_opp=0, empty_us=0;

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
		empty_opp = play_move2(move, player_id);
		print_board();
	}

	string s;
	int i=0;
	int minimax_depth = 5,fast_count=0,fast_cutoff=3;
	float low_cutoff=1,high_cutoff=8;
	int rep;
	
	// clock_t t;
	// float time_taken,total_time=0;
	int flagi = 0;
	while(true)
	{
		// cerr<<"ROUND "<<i++<<endl;
		// cerr<<"player 0"<<endl;
		// s = root_minimax(-1,min(i/5+2,4));
		i++;
		t = clock();
		// int a = (player_id==-1)?scount[0]:scount[2];
		flagi = 0;
		rep = 0;
		if (empty_us==1 && empty_opp==1)
			cout<<s<<endl;
		else
		{
			cerr<<"SAHI HAI KYA\n";
			cerr<<empty_us<<" "<<empty_opp<<" "<<s<<endl;
			s = root_minimax(player_id,minimax_depth, rep, empty_us);
			while (rep == 1)
			{
				flagi = 1;
				minimax_depth--;
				cerr<<"depth decreased to "<<minimax_depth<<endl;
				rep = 0;
				s = root_minimax(player_id, minimax_depth, rep, empty_us);
			}
			cout<<s<<endl;
		}
		
		time_taken = (clock()-t)/(float)CLOCKS_PER_SEC;
		cerr<<"time taken: "<<time_taken<<endl;

		total_time+=time_taken;
		time_left = timelimit - total_time;

		cerr<<"time left: "<<time_left<<endl;

		if (fast_count==1 && minimax_depth==3)
		{
			minimax_depth++;
			fast_count=0;
		}
		if( (timelimit>110 && time_left>90) ||  time_left > 70 )
		{
			low_cutoff=2;
			high_cutoff=10;
		}
		else if( (timelimit>110 && time_left>45) || time_left > 40 )
		{
			low_cutoff=3;
			high_cutoff=8;
		}
		else if( (timelimit>110 && time_left>20) ||  time_left > 20)
		{
			low_cutoff = 1.5;
			high_cutoff = 5;
			fast_cutoff = 4;
		}
		else if ((timelimit>110 && time_left>15) || time_left > 10)
		{
			low_cutoff=0.3;
			high_cutoff =1;
		}
		else
			minimax_depth = 4;

		if(time_taken<low_cutoff)
			fast_count++;
		if(fast_count >= fast_cutoff)
		{
			fast_count =0;
			// if (minimax_depth >= 7)
			// 	minimax_depth=7;
			// else
				// minimax_depth++;
			minimax_depth = min(19,minimax_depth+1);
			cerr<<"Depth Increased to "<<minimax_depth<<endl;
		}
		if(flagi == 0 && time_taken> high_cutoff)
		{
			minimax_depth--;
			cerr<<"Depth decreased to "<<minimax_depth<<endl;
			fast_count = 0;
		}
		if ((timelimit>110 && time_left<10) || time_left < 8)
		{
			minimax_depth = 4;
		}
		if (time_left < 5)
			minimax_depth = 3;

		
		// play_move2(s, -1*player_id);
		// print_board();
		// evaluation_function(1);

		// cerr<<"waiting for move from opponent"<<endl;
		getline(cin, move);
		while (move == "")
			getline(cin, move);
		// cerr<<"move received: "<<move<<endl;
		empty_opp = play_move2(move, player_id);
		// print_board();
		// evaluation_function(1);

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


// int main(int argc, char const *argv[])
// {
// 	cerr<<"newest\n";
// 	int player_id_recv=0, timelimit=90, player_id;

// 	// DOUBT - take in N then M ya ulta?
// 	cin>>player_id_recv>>M>>N>>timelimit;
// 	soldiers_number = 3 * (N/2);
// 	if (player_id_recv == 1)
// 		player_id = -1;
// 	if (player_id_recv == 2)
// 		player_id = 1;
// 	initTable();
// 	clock_t t, t1, t2;
// 	float time_taken,total_time=0,opponent_time, time_left;

// 	initialise();
// 	// print_board();
// 	int empty_opp=0, empty_us=0;

// 	string move;
// 	getline(cin, move);
// 	if (player_id == 1)
// 	{
// 		cerr<<"waiting for move from opponent"<<endl;
// 		t1 = clock();
// 		getline(cin, move);
// 		while (move == "")
// 			getline(cin, move);
// 		t2 = clock();
// 		opponent_time += ((float)(t2-t1))/CLOCKS_PER_SEC;
// 		empty_opp = play_move2(move, player_id);
// 		print_board();
// 	}

// 	string s;
// 	int i=0;
// 	int minimax_depth = 5,fast_count=0,fast_cutoff=3;
// 	float low_cutoff=1,high_cutoff=8;
// 	int rep;
	
// 	// clock_t t;
// 	// float time_taken,total_time=0;
// 	int flagi = 0;
// 	while(true)
// 	{
// 		// cerr<<"ROUND "<<i++<<endl;
// 		// cerr<<"player 0"<<endl;
// 		// s = root_minimax(-1,min(i/5+2,4));
// 		i++;
// 		t = clock();
// 		// int a = (player_id==-1)?scount[0]:scount[2];
// 		flagi = 0;
// 		rep = 0;
// 		if (empty_us==1 && empty_opp==1)
// 			cout<<s<<endl;
// 		else
// 		{
// 			cerr<<"SAHI HAI KYA\n";
// 			cerr<<empty_us<<" "<<empty_opp<<" "<<s<<endl;
// 			s = root_minimax(player_id,minimax_depth, rep, empty_us);
// 			while (rep == 1)
// 			{
// 				flagi = 1;
// 				minimax_depth--;
// 				cerr<<"depth decreased to "<<minimax_depth<<endl;
// 				rep = 0;
// 				s = root_minimax(player_id, minimax_depth, rep, empty_us);
// 			}
// 			cout<<s<<endl;
// 		}
		
// 		time_taken = (clock()-t)/(float)CLOCKS_PER_SEC;
// 		cerr<<"time taken: "<<time_taken<<endl;

// 		total_time+=time_taken;
// 		time_left = timelimit - total_time;

// 		cerr<<"time left: "<<time_left<<endl;

// 		if (fast_count==1 && minimax_depth==3)
// 		{
// 			minimax_depth++;
// 			fast_count=0;
// 		}
// 		if( (timelimit>110 && time_left>90) ||  time_left > 70 )
// 		{
// 			low_cutoff=2;
// 			high_cutoff=10;
// 		}
// 		else if( (timelimit>110 && time_left>45) || time_left > 40 )
// 		{
// 			low_cutoff=3;
// 			high_cutoff=8;
// 		}
// 		else if( (timelimit>110 && time_left>20) ||  time_left > 20)
// 		{
// 			low_cutoff = 1.5;
// 			high_cutoff = 5;
// 			fast_cutoff = 4;
// 		}
// 		else if ((timelimit>110 && time_left>15) || time_left > 10)
// 		{
// 			low_cutoff=0.3;
// 			high_cutoff =1;
// 		}
// 		else
// 			minimax_depth = 4;

// 		if(time_taken<low_cutoff)
// 			fast_count++;
// 		if(fast_count >= fast_cutoff)
// 		{
// 			fast_count =0;
// 			// if (minimax_depth >= 7)
// 			// 	minimax_depth=7;
// 			// else
// 				// minimax_depth++;
// 			minimax_depth = min(19,minimax_depth+1);
// 			cerr<<"Depth Increased to "<<minimax_depth<<endl;
// 		}
// 		if(flagi == 0 && time_taken> high_cutoff)
// 		{
// 			minimax_depth--;
// 			cerr<<"Depth decreased to "<<minimax_depth<<endl;
// 			fast_count = 0;
// 		}
// 		if ((timelimit>110 && time_left<10) || time_left < 8)
// 		{
// 			minimax_depth = 4;
// 		}

		
// 		// play_move2(s, -1*player_id);
// 		// print_board();
// 		// evaluation_function(1);

// 		// cerr<<"waiting for move from opponent"<<endl;
// 		getline(cin, move);
// 		while (move == "")
// 			getline(cin, move);
// 		// cerr<<"move received: "<<move<<endl;
// 		play_move2(move, player_id);
// 		// print_board();
// 		// evaluation_function(1);

// 		if (townhalls[2] <= 2)
// 		{
// 			cerr<<"Player 1 wins!";
// 			break;
// 		}
// 		else if (townhalls[0] <= 2)
// 		{
// 			cerr<<"Player 2 wins!";
// 			break;
// 		}	
// 	}
// 	return 0;
// }