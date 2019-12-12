# CANNON

We created a bot using adversarial search techniques to play an abstract strategy board game named Cannon. The rules of the original game can be found [here](https://nestorgames.com/rulebooks/CANNON_EN.pdf). The assignment pdf can be found [here](http://www.cse.iitd.ac.in/~mausam/courses/col333/autumn2019/A5/A5.pdf).
We implemented minimax along with alpha beta pruning. For better pruning, we sorted the expansion of internal nodes according to our evaluation function. We also implemented iterative deepening for getting to more depths as the game progressed.

## Key features:

1. **Sorting for better pruning:** 
	We used evaluation function to evaluate child nodes, sort in decending order of this value and expand the children in this order. This significantly reduced the time taken for one move and we were able to run the bot at one increased depth.

2. **Transposition table**:
	We hashed the entire board using Zobrist hashing. There were a large number of hits, more as we descended down the search tree. This helped us evaluate the board faster by looking up in the table instead of evaluating the state.

3. **Adaptive depth**:
	The game starts at depth 5. Depending on the time taken to find the best move at a given depth and the time remaining, we increase/ decrease the depth at which the bot is playing. Within the first ten moves, the depth changes to 6 or 7. Close to the end of the game when the number of soldiers becomes small, we were able to acheive a depth of 10 as well. This also helped us reach different depths on different computers according to their speed without running out of time.

4. **Implementation of reverse move**:
	While descending down the minimax tree implementing depth first search, the move leading to one child is undone/reversed to move back to the parent board. This saves the time and memory otherwise required for storing the entire board configuration.

5. **Evaluation function**:
	To evaluate any board state, we have taken into account a number of features like the number of cannons, number of townhalls and soldiers under fire by opponent cannons, number of townhalls and soldiers under attack by opponent soldiers, the number of soldiers ahead of some pre-defined mark, number of soldiers remaining.