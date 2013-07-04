/* 
 * File : evilHangman.cpp
 * -----------------------------------------------------
 * Source file for the console based evil hangman game.
 */

/* Including standard libraries */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <numeric>
#include "lib/getFunctionsLib.h"
using namespace std;

/* Function prototypes */
void printGameStatus(vector<char>& guessedChars,int noOfGuesses,bool runningTotal,set<string>& curGuessSet);
void updateCurGuessSet(set<string>& curGuessSet, char currentGuess,vector<char>& guessedChars);
void runGame(int initGuess, map<int,set<string> >& dict,int noOfGuesses, bool runningTotal);
void readDictIntoMap(map<int,set<string> >& dict, ifstream& dictionary);
int readGuessSize(int minSize,int maxSize, map<int,set<string> >& dict);
char getUserGuess(vector<char>& guessedChars);
int readNoOfGuesses();

/* Some extra utility functions */
string GetLine();
int GetInteger();

/* Main program */
int main(){

  /* Reading the dictionary.txt file into a map of integer and strings.*/
  ifstream dictionary("dictionary.txt");
  map<int,set<string> > dict; 
  readDictIntoMap(dict,dictionary);
 
  /* Getting the maximum and minimum size of words in dictionary according to initialised map */
  map<int,set<string> >::iterator itr = --dict.end();
  int maxSize = itr->first;
  itr = dict.begin();
  int minSize = itr->first;
  
  /* Getting the input about the initial guess size from the user */
  int initGuess = readGuessSize(minSize,maxSize,dict);
  
  /* Prompting the user about the number of tries he wants */
  int noOfGuesses = readNoOfGuesses();

  /* Prompting the user about whether she wants a running total of the words remaining */
  char otherChar;
  cout<<"Do you want the running total of the words remaining? [y/n] "<<endl;
  cin>>otherChar;
  bool runningTotal = otherChar=='y'?true:false;
   
  /* Running the actual game */
  runGame(initGuess,dict,noOfGuesses,runningTotal);

  return 0;
}

/* Function definitions */

  
/* Function : runGame
 * -------------------------------------------------
 * Simulates the actual EVIL(snigger) hangman game.
 */
  void runGame(int initGuess,map<int,set<string> >& dict,int noOfGuesses,bool runningTotal){
    
    /* Constructing a set of words with word length matching initial length */
      set<string> curGuessSet = dict[initGuess];
    
    /* Vectors containing the characters the user has guessed, and those that are correct */ 
      vector<char> guessedChars(initGuess,'_');
     
    while(noOfGuesses>0){

      /* Printing the current game status */
      printGameStatus(guessedChars,noOfGuesses,runningTotal,curGuessSet);

      /* Prompt the user to enter a character */
      char currentGuess = getUserGuess(guessedChars); 
    
      /* Update the curGuessSet accorfing to the user guess */ 
      updateCurGuessSet(curGuessSet,currentGuess,guessedChars);
   
      /* Exits if the user has won */
      if(count(guessedChars.begin(),guessedChars.end(),'_')==0){
        cout<<"Congrats, you win!"<<endl;
        cout<<"The letter was : "<<endl;
        for(int i=0;i<guessedChars.size();++i){
          cout<<guessedChars[i]; 
        }
        cout<<endl;
        return; 
      } 

      noOfGuesses--;   

    }//End of while loop
  
    /* Display a message in case the no of guesses exhausted */  
    cout<<"Sorry, you lose"<<endl;

    return;        
  }

/* Function  :updateCurGuessSet
 * ------------------------------------------------------------------------------------------------------------------
 * Updates the current guessed set of strings by partitioning the set and choosing the one with most number of words.
 */
void updateCurGuessSet(set<string>& curGuessSet, char currentGuess,vector<char>& guessedChars){
  
  int wordLength = curGuessSet.begin()->length();
  
  /* Initialising map of set partitions containing word families */
  map<int,set<string> > partitions;

  /* Logic for repeated letters. Assigning repeat Letter position the position pos of last repeated letter.*/
  int repeatLetterPosition = -1;
  for(int i=0;i<guessedChars.size();i++){
    if(guessedChars[i]==currentGuess)
      repeatLetterPosition = i;
  }

  /* Looping through the current word set to populate set partitions */
  for(set<string>::iterator itr=curGuessSet.begin();itr!=curGuessSet.end();++itr){
    string curWord = *itr;
    int pos;

    /* Starts find operation after the last found repeated letter.If not, starts at 0, as repeatLetter position is
     *  initialised to -1. */
      
    pos = curWord.find(currentGuess,repeatLetterPosition+1);    
    if(pos==string::npos)
      partitions[0].insert(curWord);
    else{
      for(int i=0;i<wordLength;++i){
        if(pos==i)
    partitions[i+1].insert(curWord);        
      }//Inner for loop ends here
    }//If else block ends here 
  }//Outer for loop ends here
  
  
  /* Updating the current guess set according to max size */
  int maxSizeIndex=-1;
  int maxSize = -1; 
  for(map<int,set<string> >::iterator itr = partitions.begin();itr!=partitions.end();++itr){
    int size = (*itr).second.size();
    if( size > maxSize){
      maxSize = size;
      maxSizeIndex =  (*itr).first;
    }
  }
 
  curGuessSet = partitions[maxSizeIndex];
  
  /*
   * Updating the gussedCharsVector vector according to the partitions selected . maxSizeIndex -1 corresponds 
   * to the index of the letter in the word family.
   */
  if(maxSizeIndex!=0)
    guessedChars[maxSizeIndex-1] = currentGuess;

  return;
}

/* 
 * Function  :getUserGuess
 * --------------------------------------
 * Prompts the user to enter a character
 */
char getUserGuess(vector<char>& guessedChars){
  cout<<"Enter you guess : ";
  char currentChar;
  while(true){
    cin>>currentChar;
    if(currentChar>='a' && currentChar<='z')
      break; 
    else
      cout<<"Please enter an alphabetical lowercase character"<<endl;     
  }
  return currentChar;
}


/* Function : printGameStatus
 * ----------------------------
 * Prints out the current game status including  : - 
 * 1] the guessed characters, the word length [the combo]
 * 2] Running total of remaining words if she has indicated so.
 * 3] No of guesses left
 */
void printGameStatus(vector<char>& guessedChars, int noOfGuesses, bool runningTotal,set<string>& curGuessSet){
  /* Printing out the guessed chars vector */
  for(int i=0;i<guessedChars.size();i++){
    cout<<guessedChars[i]<<" ";
  }
  cout<<endl;

  /* Printing out the no of guesses left */
  cout<<"You have "<<noOfGuesses<<" attempts left"<<endl;

  /* Printing out the runningTotal of words left, if user desires */
  if(runningTotal)
    cout<<"Words left : "<<curGuessSet.size()<<endl;

}

/* Function : readDictIntoMap
 * ------------------------------------------------------------------------------------------------------------
 * Reads the dictionary contents into a map which contains sizes mapped to a sets of strings of those sizes.
 */
  void readDictIntoMap(map<int,set<string> >& dict, ifstream& dictionary){
    string line;
    while(getline(dictionary,line)){
      dict[line.length()].insert(line);
    }
    return;
  }
  
/* Function : readGuessSize
 * ------------------------------------------------------------------------------------------------------------
 * Takes in input from the user and ensures that the initial guessed size is between min and max sizes of words
 * in the english dictionary.
 */ 
  int readGuessSize(int minSize,int maxSize,map<int,set<string> >& dict){
    cout<<"Enter the number of words in the word you wish to guess..."<<endl;
    int guess;
    while(true){
      guess = GetInteger();
      if(dict.count(guess))
        break; 
      cout<<"No word in dictionary of that size!"<<endl;
    }
    return guess;
  }

/*
 * Function : readNoOfGuesses
 * ---------------------------------------------------------------------------------------
 * Prompts the user about the number of times he wants to try before his chances exhaust.
 */
  int readNoOfGuesses(){
    cout<<"Enter the number of attempts you would like to have..."<<endl;
    int noOfGuesses = GetInteger();
    return noOfGuesses;
  }  
  
  
  /*
   * Function : GetLine()
   * --------------------------------------------------------------------------------
   * This function gets a line from the user and returns it in the form of a string.
   */
  string GetLine(){
    string line;
    getline(cin,line);
    return line;
  }

  /* 
   * Function : GetInteger()
   * ---------------------------------------------------------------
   * This function accepts an integer from the user and returns it,
   */
   int GetInteger(){
     int intToReturn;
     while(true){
       if(cin>>intToReturn){
         char otherChar;
         if(cin>>otherChar)
           cout<<"Illegal character format. Try again"<<endl;
         else
           return intToReturn;
       }else
         cout<<"Please enter an integer"<<endl;
     }   
   }
   
