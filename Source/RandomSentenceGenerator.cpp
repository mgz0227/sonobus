#include "RandomSentenceGenerator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <locale>
#include <stdlib.h>

/**   
 *    The constructor. Reads a file and populates the map with rules and their corresponding
 *    values.
 **/

using namespace std;



RandomSentenceGenerator::RandomSentenceGenerator(const std::string & fileName)
{
    //create a inFile and open the file
    ifstream inFile(fileName);

    
    //if the file doesnt open
    if( !inFile.is_open() )
    {
        cerr << "Unable to open " << fileName << " for reading. "; 
        return;
    }
    
    readGrammar(inFile);
}

RandomSentenceGenerator::RandomSentenceGenerator(std::istream & inFile)
{
    readGrammar(inFile);    
}

bool RandomSentenceGenerator::readGrammar(std::istream & inFile)
{

    string rule, sentence;
    string delimiters = " \t\n\r;";
    string line = "";

    capEveryWord = false;
    
    
    grammar.clear();
    
    //while there is still another line to read
    while(getline(inFile, line)){
        unsigned long flag = line.find("{");
        if(flag != std::string::npos){
            //the next line should be the grammar rule
            getline(inFile, line);
            rule = toLwer(line);
            rule = rule.substr(0,rule.find_last_not_of(delimiters)+1);
            //cout << "rule: " << rule << endl;
            getline(inFile, line);
            unsigned long flag2 = line.find("}");
            while(flag2 == std::string::npos){ //if the line isn't a right bracket
                sentence = line;
                sentence = sentence.substr(0,sentence.find_last_not_of(delimiters)+1);
                //cout << "sentence: " << sentence << endl;
                grammar.addProduction(rule, sentence);

                //reset the flag every time
                getline(inFile, line);
                flag2 = line.find("}");
            } //end while
        } //end if
      }
    
    return true;
}


/**
 *    Returns a random sentence generated from the grammar.
 *    Calls a recursive helper method to fill in grammar rules.
 **/
string RandomSentenceGenerator::randomSentence(){
  return randomSentence("<start>");
}//end randomSentence

/**
 *   Recursive helper method to generate a random sentence.    
 **/
string RandomSentenceGenerator::randomSentence(string rule){
    string sentence = grammar.getRandomRHS(rule); //save the first random sentence

    rule = getRule(sentence); //get the rule in the the sentence (this matters in the recursion)
    
    //while we still have more rules to get
    while(rule != ""){
        replaceRule(&sentence, rule, randomSentence(rule));
        rule = getRule(sentence);
    }//end while
    
    return sentence;
}//end recursive helper method

/**
 * Replace non terminal (non '<rule>')
 **/
void RandomSentenceGenerator::replaceRule(string *s, string lhs, string sub){
    int start = s->find(lhs);
    if(start != string::npos){
        unsigned long end = s->find('>');
        if (capEveryWord && sub.length() > 0) {
            capFirst(sub);
        }
        s->replace(start, (end - start) + 1, sub);
    } // end if
}

/**
 * Get the rule in the sentence
 **/
string RandomSentenceGenerator::getRule(string sentence){
   unsigned long ruleIndexStart = sentence.find('<');
   //if there is a rule to be found / no '<''s if not
   if(ruleIndexStart != string::npos){
      unsigned long ruleIndexEnd = sentence.find('>'); //find the beginning and end of the rule
      //return the sentence with the rule filled
      return sentence.substr(ruleIndexStart, ++ruleIndexEnd - ruleIndexStart);
   }// end if

   return "";
}


/**   
 * Prints the grammar using the overloaded << operator
 **/
void RandomSentenceGenerator::printGrammar(){
    cout << grammar << endl;
}//end printGrammar

/**
 * returns a lowercase version of a given string
 **/
string RandomSentenceGenerator::toLwer(string s){
  locale loc;
  for (int i = 0; i < s.length(); ++i){
    tolower(s[i], loc);
  }
  return s;
}

void RandomSentenceGenerator::capFirst(string & s){
  locale loc;
  if (s.length() > 0) {
      s[0] = toupper(s[0], loc);
  }
}
