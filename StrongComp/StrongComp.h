#ifndef STRONGCOMP_H
#define STRONGCOMP_H
//////////////////////////////////////////////////////////////////////
//  StrongComp.h - analyze the strong component by building graph   //
//  ver 1.0                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module defines StrongComp class. StrongComp Class is to analyze the strong component 
* based on dependency info stored in the DepAnal Class.
* StrongComp Class must be initialized by addNode and addDependency function.
* The value of node in graph is string type.
* The edges and nodes of graph to analyze will be stored in graph_
*
*
* Public Interface:
* =================
* - addDependency		//add edges between nodes to build graph
* - addNode				//add nodes into graph
* - Tarjan				// Tarjan Analysis
* - writeToXml			//write the result into Xml file. The file name is component.xml
* - show                //show the strong component set.
*
* Build Process:
* ==============
* Required files
* - StrongComp.h, StrongComp.cpp, XmlWriter.h, XmlWriter.cpp
* Build commands 
* - SET CL=/DTEST_STRONGCOMP
* - devenv CodeAnalyzerEx.sln /rebuild debug
*
* Maintenance History:
* ====================
* ver 1.0 : 07 Mar 2017
* - first release
*/



#include "../XmlWriter/XmlWriter.h"
#include <string>
#include <unordered_map>
#include<iostream>
#include <stack>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>





namespace CodeAnalysis
{
	class StrongComp 
	{
	public:
		//add edges between nodes to build graph
		void addDependency(std::string,std::string);
		//add nodes into graph
		void addNode(std::string);
		// Tarjan Analysis
		void Tarjan();
		//write the result into Xml file. The file name is component.xml
		void writeToXml(std::string);
		//show the component list
		void show();
		//clear the data in StrongComp class
		void clear();

	private:
		//for the Tarjan function to iterate the graph_
		void DFS(std::string node, std::unordered_map<std::string, int>&rDFN, std::unordered_map<std::string, int>&rLow, std::stack<std::string> &strStack, std::unordered_map<std::string, bool>& rInStock);
		//store the dependency relationship
		std::unordered_map<std::string, std::vector<std::string>> graph_; 
		//store the compenent 
		std::unordered_map<int, std::vector<std::string>> components;  
		
	};
}


#endif // !STRONGCOMP_H

