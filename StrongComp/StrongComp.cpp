//////////////////////////////////////////////////////////////////////
//  StrongComp.cpp - analyze the strong component by building graph   //
//  ver 1.0                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include "StrongComp.h"
using namespace CodeAnalysis;

//add edges between nodes to build graph
void StrongComp::addNode(std::string path) 
{
	graph_[path] = std::vector<std::string>();
}

void StrongComp::clear() {
	graph_.clear();
	components.clear();
}

//add nodes into graph
void StrongComp::addDependency(std::string dependant, std::string relier)
{
	graph_[dependant].push_back(relier);
}
// The function to do DFS traversal.
void StrongComp::DFS(std::string node, std::unordered_map<std::string, int>&rDiscovery, std::unordered_map<std::string, int>&rLow, std::stack<std::string> &strStack, std::unordered_map<std::string, bool>& rInStock)
{
	// A static variable is used for simplicity, we can avoid use
	// of static variable by passing a pointer.
	static int index = 0;
	static int componentNumber = 0;

	// Initialize discovery time and low value
	rDiscovery[node] = rLow[node] = ++index;
	strStack.push(node);
	rInStock[node] = true;

	// Go through all vertices adjacent to this
	for (int i = 0; i < (int)graph_[node].size(); ++i)
	{
		std::string temp = graph_[node][i];
		// If node is not visited yet, then recur for it
		if (rDiscovery.find(temp) == rDiscovery.end())
		{
			DFS(temp, rDiscovery, rLow, strStack, rInStock);
			rLow[node] = std::min(rLow[node], rLow[temp]);
		}
		else if (rInStock[temp])
			rLow[node] = std::min(rLow[node], rDiscovery[temp]);
	}

	// head node found, pop the stack and print an SCC
	std::string w;  // To store stack extracted vertices
	if (rLow[node] == rDiscovery[node])
	{
		componentNumber++;

		while (strStack.top() != node) 
		{
			w = strStack.top();
			components[componentNumber].push_back(w);
			rInStock[w] = false;
			strStack.pop();
		}
		w = strStack.top();
		components[componentNumber].push_back(w);
		rInStock[w] = false;
		strStack.pop();
	}
}

//write the result into Xml file. The file name is component.xml
void StrongComp::writeToXml(std::string path) 
{
	XmlWriter wtr;
	wtr.indent();
	wtr.addDeclaration();
	wtr.start("root");
	for (auto component : components)
	{
		wtr.start("Component");
		std::stringstream stream;
		stream << component.first;
		std::string id = stream.str();
		wtr.addAttribute("id", id);
		for (auto node : component.second) 
		{
			wtr.start("file");
			wtr.addAttribute("name",node);
			for (auto dependency : graph_[node])
			{
				wtr.start("dependency");
				wtr.addAttribute("fileName",dependency);
				wtr.end();
			}

			wtr.end();
		}
		wtr.end();
	}
	wtr.end();

	std::ofstream out(path);
	if (out.good())
	{
		out << wtr.xml().c_str();
		out.close();
	}
}

void StrongComp::show() 
{
	for (auto component : components)
	{
		std::cout << "   ===============================Component id:" << component.first << "===================================" << std::endl;

		for (auto node : component.second) 
		{
			std::cout << "    file name:" << node  << std::endl;
		}
		std::cout << "   ================================================================================" << std::endl;
	}
}

// Tarjan Analysis
void StrongComp::Tarjan()
{
	int size = graph_.size();
	std::unordered_map<std::string, int>discovery;
	std::unordered_map<std::string, int>Low;
	std::unordered_map<std::string, bool> InStack;
	std::stack<std::string> strStack;

	// Initialize instack arrays
	for (auto item : graph_)
	{
		InStack[item.first] = false;
	}

	for (auto item : graph_) 
	{
		if (discovery.find(item.first) == discovery.end()) 
		{
			DFS(item.first, discovery, Low, strStack, InStack);
		}
	}

	
}



//test sub------------------------
#ifdef TEST_STRONGCOMP

int main()
{
	StrongComp sc;
	sc.addNode("0");
	sc.addNode("1");
	sc.addNode("2");
	sc.addNode("3");
	sc.addDependency("0","1");
	sc.addDependency("1", "2");
	sc.addDependency("2","0");
	sc.addDependency("2","3");
	sc.Tarjan();
	sc.show();
	sc.writeToXml("test.xml");
	std::cout << "        end!" << std::endl;
    
}
#endif
