#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <utility>
#include <list>
#include <functional>
#include <cmath>
#include <algorithm>
#include <utility>
#include <iomanip>
#include <limits>
#include <chrono>
#include "state.cpp"

using namespace std;

//typedef vector<int> State;

//  .---.
//  |2|0|
//  .---. -> State b = {2,0,1,3}
//  |1|3|
//  .---.

typedef pair<int,int> Move;



void
print( const State& state )
{
  state.display();
}


void
addNeighbor( State &currentState, Move &move, 
             vector< pair< Move,int > > &neighbors,
             list<State>& path)
{
  currentState.doMove( move );
  int h = currentState.heuristic();
  if( find( path.begin(), path.end(), currentState ) == path.end() )
  {
    neighbors.push_back( make_pair( move,  h) );
  }
  currentState.undoMove( move ); // undo move ici
}

void
search( State& currentState,
        int          ub, // upper bound over which exploration must stop
        int&         nub,
        list<State>& path,
        list<State>& bestPath,
        int&         nbVisitedState )

{

  nbVisitedState++;

  int f; // under-estimation of optimal length
  int g = path.size() - 1; // size of the current path to currentState

  if( currentState.isFinal() )
  {
    bestPath = path;
    return;
  }

  // generate the neighbors
  vector< pair< Move,int > > neighbors;
  neighbors.clear();
  
  //ajoouter les voisins
  for(int i = 0; i < currentState.getNbStacks(); i++)
  {
     if(!currentState.emptyStack(i))
     {
        for(int j = 0; j < currentState.getNbStacks(); j++)
        {
          if( i != j)
          {
            Move move = make_pair(i,j);
            addNeighbor(currentState, move, neighbors, path);
          }
        }
     }
  }

  for( const pair<Move,int> &p : neighbors )
  {
    f = g + 1 + p.second;
    if( f > ub )
    {
      if( f < nub )
      {
        nub = f; // update the next upper bound
      }
    }
    else
    {
      currentState.doMove( p.first );
      path.push_back( currentState );
      search( currentState, ub, nub, path, bestPath, nbVisitedState );
      path.pop_back();
      currentState.undoMove( p.first ); // undo move
      if( !bestPath.empty() ) return;
    }
  }
}

void
ida( State&        initialState, 
     list<State>&  bestPath, // path from source to destination
     int&          nbVisitedState )
{
  int ub;                      // current upper bound
  int nub = initialState.heuristic(); // next upper bound
  list<State> path;
  path.push_back( initialState ); // the path to the target starts with the source

  while( bestPath.empty() && nub != numeric_limits<int>::max() )
  {
    ub = nub;
    nub = numeric_limits<int>::max();

    cout << "upper bound: " << ub;
    search( initialState, ub, nub, path, bestPath, nbVisitedState );
    cout << " ; nbVisitedState: " << nbVisitedState << endl;
  }
}

int
main()
{
  
  State* b = new State(20,4);
  b->setInitial();
  list<State> bestPath;
  int nbVisitedState = 0;
  
  auto start = std::chrono::high_resolution_clock::now();
  ida( *b, bestPath, nbVisitedState );
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  cout << "Elapsed time: " << elapsed.count() << " s\n";
  cout << "nb moves: " << bestPath.size()-1 << endl;
  cout << "nb visited states: " << nbVisitedState << endl;

  return 0;
}

