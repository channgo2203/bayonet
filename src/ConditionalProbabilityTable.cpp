/* 
 * bayonet - C++ Bayesian networks library
 * Copyright (C) 2015  Massimiliano Patacchiola
 * Author: Massimiliano Patacchiola
 * email:  
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
*/

#include <iostream>
#include <iomanip>
#include <random>
#include "ConditionalProbabilityTable.h"

namespace bayonet{

/**
* The constructor of the object.
*
* @param NodeStatesNumber is the number of states of the node associated with the table.
* Using this constructor the table is initialized without any parent.
**/
ConditionalProbabilityTable::ConditionalProbabilityTable(unsigned int NodeStatesNumber){

 //Wrong input check
 if(NodeStatesNumber < 2) NodeStatesNumber=2;

 //Fill the map
 FillMap(NodeStatesNumber, {});

}

/**
* The constructor of the object.
*
* @param NodeStatesNumber is the number of states of the node associated with the table
* @param parentsStates this is a list that represents the number of states of each parent of the node
* If the node has 3 boolean states as parents the list is {2, 2, 2}
**/
ConditionalProbabilityTable::ConditionalProbabilityTable(unsigned int NodeStatesNumber, std::vector<unsigned int> parentsStates){

 //Wrong input check
 if(NodeStatesNumber < 2) NodeStatesNumber=2;

 //Fill the map
 FillMap(NodeStatesNumber, parentsStates);

 //Set the Parents state vector
 mTotalParentsStates = parentsStates;
}

/**
* Destroying the object
*
**/
ConditionalProbabilityTable::~ConditionalProbabilityTable(){}

/**
* Return a single row of the table.
* The row is a pair containing the parent state
* and the probabilities for all the variable states.
*
* @param index
**/
std::pair<std::vector<unsigned int>, std::vector<double>> ConditionalProbabilityTable::ReturnRow(unsigned int index){
 std::vector<unsigned int> parent_vector;
 std::vector<double> state_vector;
 if(index > conditionalMap.size()) return make_pair(parent_vector, state_vector); //out of range > empty vector returned
 auto it_map=conditionalMap.begin();
 std::advance(it_map, index);
 parent_vector = it_map->first;
 state_vector = it_map->second;
 return make_pair(parent_vector, state_vector);
}

/**
* Return the Parents state at a given index
*
* @param index
**/
std::vector<unsigned int> ConditionalProbabilityTable::ReturnParentsState(unsigned int index){
 std::vector<unsigned int> vector_to_return;
 if(index > conditionalMap.size()) return vector_to_return; //out of range > empty vector returned
 auto it_map=conditionalMap.begin();
 std::advance(it_map, index);
 vector_to_return = it_map->first;
 return vector_to_return;
}

/**
* It find in which rows the specified parent has the specified state.
* It returns a list of integers, representing the positions of the
* parent in the CPT.
*
* @param parentIndex
* @param parentState
**/
std::vector<unsigned int> ConditionalProbabilityTable::FindParentState(unsigned int parentIndex, unsigned int parentState){
 unsigned int row = 0;
 std::vector<unsigned int> rows_vector;

 //Iterate through map content:
 for (auto it_map=conditionalMap.begin(); it_map!=conditionalMap.end(); ++it_map){
  std::vector<unsigned int> parents_vector = it_map->first;

  if(parents_vector.size()==0){
   std::cerr << "ERROR: the Conditional Table does not have any parent" << std::endl;
   return rows_vector;
  }
  if(parentIndex > parents_vector.size()-1){
   std::cerr << "ERROR: parentIndex out of range in Conditional Table" << std::endl;
   return rows_vector;
  }
//std::cout  << "  "  << "    CHEK 1.1.1.1.CON " << parentIndex << " # " << parents_vector.size() << " " << row << std::endl;
  if(parents_vector[parentIndex] == parentState) rows_vector.push_back(row);

  row++;
 }
 return rows_vector;
}

/**
* Given the state of the variable and a vector key it returns the associated probability
*
* @param variableState
* @param parentsStates
**/
double ConditionalProbabilityTable::GetProbability(unsigned int variableState, std::vector<unsigned int> parentsStates){
 auto row_vector = conditionalMap[parentsStates];
 //std::cout << "AT: " << row_vector.at(variableState) << std::endl;
 return row_vector.at(variableState);
}



/**
* Given a vector key it returns the associated probabilities
*
* @param parentsStates
**/
std::vector<double> ConditionalProbabilityTable::GetProbabilities(std::vector<unsigned int> parentsStates){
 return conditionalMap[parentsStates];
}

/**
* Given a variable State, a specific parent index and a parent state,
* it returns all the probabilities associated with that configuration
* finding them through the rows of the CPT
*
* @param variableState
* @param parentIndex
* @param parentsState
**/
std::vector<double> ConditionalProbabilityTable::GetProbabilities(unsigned int variableState, unsigned int parentIndex, unsigned int parentsState){
 std::vector<double> vector_to_return;
 //Iterate through map content:
 for (auto it_map=conditionalMap.begin(); it_map!=conditionalMap.end(); ++it_map){
  if(it_map->first[parentIndex] == parentsState) vector_to_return.push_back( it_map->second[variableState] );
 }
 return vector_to_return;
}

/**
* Given a vector key it sets the probabilities associated
*
* @param parentsStates
* @param probabilities
**/
bool ConditionalProbabilityTable::SetProbabilities(std::vector<unsigned int> parentsStates, std::vector<double> probabilities){
 if(conditionalMap.find(parentsStates) != conditionalMap.end()){
  conditionalMap[parentsStates] = probabilities;
  return true;
 }else{
  return false;
 }
}

/**
* Given a vector key and a variable state, it adds a value
* to the associated probability.
*
* @param variableState
* @param parentsStates
* @param valueToAdd
**/
bool ConditionalProbabilityTable::AddToProbability(unsigned int variableState, std::vector<unsigned int> parentsStates, double valueToAdd){
 if(variableState > conditionalMap.at(parentsStates).size()){
  std::cerr << "ERROR: Conditional Table out of range index." << std::endl;
  return false; 
 }

 conditionalMap.at(parentsStates)[variableState] += valueToAdd;
 return true;
}

/**
* It prints the conditional table on the terminal.
* If the number of columns and rows is huge, the terminal could cut parts of the output.
*
**/
void ConditionalProbabilityTable::Print(){
 
  std::cout << std::endl;

 //std::cout << '|' << std::setw(5) << "KEY" << std::setw(5) << '|' << std::endl;

  //Iterate through map content:
  for (auto it_map=conditionalMap.begin(); it_map!=conditionalMap.end(); ++it_map){

   //std::cout << "KEY: ";
   std::cout << '|' << std::setw(5);
   for (auto it_key=it_map->first.begin(); it_key!=it_map->first.end(); ++it_key){
    std::cout << (*it_key);
     if(it_key!=it_map->first.end()-1) std::cout << "-";
   }
   std::cout << std::setw(5);

   //std::cout << "  PROB: ";
   for (auto it_data=it_map->second.begin(); it_data!=it_map->second.end(); ++it_data){
    std::cout << "| "  << std::setw(6)  << (*it_data) << std::setw(6);  //<< std::setprecision(4)
   }

   std::cout << '|' << std::endl;
  }

 std::cout << std::endl << std::endl;
 std::cout << "COLUMNS .... " << ConditionalProbabilityTable::ReturnColumnsNumber() << std::endl;
 std::cout << "ROWS    .... " << ConditionalProbabilityTable::ReturnRowsNumber() << std::endl;
}

/**
* It prints the probabilities associated with a particular set of parents.
* If the number of columns is huge, the terminal could cut parts of the output.
*
* @param parentsStates
**/
void ConditionalProbabilityTable::PrintProbabilities(std::vector<unsigned int> parentsStates){

 std::vector<double> prob_vector = conditionalMap[parentsStates];

 std::cout << '\n';

 //std::cout << "KEY: ";
 std::cout << '|' << std::setw(5);
 for (auto it=parentsStates.begin(); it!=parentsStates.end(); ++it){
  std::cout << *it;
  if(it!=parentsStates.end()-1) std::cout << "-";
 }
 std::cout << std::setw(5);

 //std::cout << "  PROB: ";
 for (auto it=prob_vector.begin(); it!=prob_vector.end(); ++it){
  std::cout << "| "  << std::setw(6) << *it << std::setw(6);
 }

 std::cout << '|' << '\n';
}

/**
* It normalize the probabilities inside each row of the table.
*
**/
void ConditionalProbabilityTable::NormalizeProbabilities(){
 
 //Iterate through map content:
 for (auto it_map=conditionalMap.begin(); it_map!=conditionalMap.end(); ++it_map){
   //define an accumulator variable
   //and assigne to it the value of zero
   double accumulator = 0;
   //Iterate through the vector of probabilities for accumulate the sum
   for (auto it_data=it_map->second.begin(); it_data!=it_map->second.end(); ++it_data){
    accumulator += *it_data;
   }

   //Iterate through the vector of probabilities for the normalization
   for (auto it_data=it_map->second.begin(); it_data!=it_map->second.end(); ++it_data){
    //If the accumulator is zero, then all the variables are zero
    //It prevent a problem in case of a zero divisor
    if(accumulator != 0) *it_data = *it_data / accumulator;
    else *it_data = 0.0;
   }
 }
}

/**
* It randomize the probabilities inside each row of the table.
* The probabilities are also normalized.
**/
void ConditionalProbabilityTable::RandomizeProbabilities(){

 std::random_device random_device;
 std::mt19937 generator(random_device());
 std::uniform_real_distribution<> real_dist(0, 1);


 //Iterate through map content:
 for (auto it_map=conditionalMap.begin(); it_map!=conditionalMap.end(); ++it_map){

  //save the size of the vector and then clear it
  unsigned int numberOfStates = it_map->second.size();
  it_map->second.clear();
  double accumulator = 0;

  //generate random values and push them inside the state vector
  for (unsigned int i = 0; i < numberOfStates; i++) {
   double temp_value = real_dist(generator);
   it_map->second.push_back(temp_value);
   accumulator += temp_value;      
  }

  //Iterate through the vector of probabilities for the normalization
  for (auto it_data=it_map->second.begin(); it_data!=it_map->second.end(); ++it_data){
   *it_data = *it_data / accumulator;
  }
 }

}

/**
* It returns a random sample, given a particular configuration of the parents
*
* @param parentsStates
**/
unsigned int ConditionalProbabilityTable::ReturnSample(std::vector<unsigned int> parentsStates){
 std::random_device random_device;
 std::mt19937 generator(random_device());

 std::discrete_distribution<unsigned int> values_distribution (conditionalMap[parentsStates].begin(), conditionalMap[parentsStates].end());

 return values_distribution(generator);
}

/**
* It add a new parent to the table.
*
* @param totStates the total number of states to assign to the new parent
**/
void ConditionalProbabilityTable::AddVariable(unsigned int totStates=2){

 //Check for wrong input
 if(totStates <2) totStates=2;

 //Push the new total in the vector
 mTotalParentsStates.push_back(totStates);

 unsigned int tot_columns = ReturnColumnsNumber();

 //clear the map
 conditionalMap.clear();

 FillMap(tot_columns, mTotalParentsStates);
}

/**
* It set to zero all the entries of the table.
*
* @param valueToSet
**/
void ConditionalProbabilityTable::ResetProbabilities(double valueToSet){
 //Iterate through map content:
 for (auto it_map=conditionalMap.begin(); it_map!=conditionalMap.end(); ++it_map){

  //Iterate through the vector of probabilities
  for (auto it_data=it_map->second.begin(); it_data!=it_map->second.end(); ++it_data){
   *it_data = valueToSet;
  }
 }
}

/**
* It clear the content of the table.
*
**/
void ConditionalProbabilityTable::Clear(){
 conditionalMap.clear();
 mTotalParentsStates.clear();
}

/**
* It returns the total number of rows.
*
**/
unsigned int ConditionalProbabilityTable::ReturnRowsNumber(){
 return conditionalMap.size();
}

/**
* It returns the total number of columns.
* The number of columns coincides with the number of states.
*
**/
unsigned int ConditionalProbabilityTable::ReturnColumnsNumber(){
 if(conditionalMap.size() == 0) return 0;
 else{
  return conditionalMap.begin()->second.size();
 }
}

/**
* Private function. It's a low level function for filling the content of the map.
* It works like an odometer and create all the possible combination of states starting from the parents states.
*
* @param NodeStatesNumber
* @param parentsStates
**/
void ConditionalProbabilityTable::FillMap(unsigned int NodeStatesNumber, std::vector<unsigned int> parentsStates){
 //0- In case of a root node without parents, it adds only one row
 if(parentsStates.size() == 0){
  std::vector<double> data_vector;
  std::vector<unsigned int> key_vector;
  for(unsigned int i=0; i<NodeStatesNumber; i++){
   data_vector.push_back((double)1.0 / (double)NodeStatesNumber);
  }
  conditionalMap.insert( std::make_pair( key_vector, data_vector ) );
  return;
 }

 //1- Creation of the vector that contains the Subsets of unsigned int
 std::vector<std::vector<unsigned int>> vector_container; 
 for (auto it = parentsStates.begin() ; it != parentsStates.end() ; ++it)
 {
  std::vector<unsigned int> temp_vector;
  unsigned int total_states = (*it);
  for(unsigned int state_counter=0; state_counter<total_states; state_counter++){
   temp_vector.push_back(state_counter);
  }
  vector_container.push_back(temp_vector);
  }

 //2- Creation of the vector that contains the Iterators
 std::vector< std::vector<unsigned int>::iterator > iterator_container;
 for (auto it = vector_container.begin() ; it != vector_container.end() ; ++it){
   std::vector<unsigned int>::iterator temp_iterator;
   temp_iterator = (*it).begin();
   iterator_container.push_back(temp_iterator);
 }

 //3- filling the data_vector
 std::vector<double> data_vector;
 data_vector.reserve(NodeStatesNumber);
 for(unsigned int p_counter=0; p_counter<NodeStatesNumber; p_counter++){
  data_vector.push_back((double) 1.0 / (double)NodeStatesNumber);
 }

 //4- Cascade iteration for storing the whole set of combination
 unsigned int K = iterator_container.size();

 while (iterator_container[0] != vector_container[0].end()) {

    //Clear the key_vector before pushing a new key
    std::vector<unsigned int> key_vector;
    key_vector.reserve(parentsStates.size());

    //Filling the key_vector 
    for (auto it_key=iterator_container.begin(); it_key!=iterator_container.end(); ++it_key){    
     key_vector.push_back( (*(*it_key)) ); //pushing a new key
    }

    //Assign a new row in the conditional map
    conditionalMap.insert( std::make_pair( key_vector, data_vector ) );

   //Increment the odometer by 1 
   //ATTENTION: It must called at the end of the while otherwise the first key get missed...
   ++iterator_container[K-1]; 
   for (int i = K-1; (i > 0) && (iterator_container[i] == vector_container[i].end()); --i) {
    //subtracting the counter
    iterator_container[i] = vector_container[i].begin();
    ++iterator_container[i-1];
   }
 }
}



} //namespace









