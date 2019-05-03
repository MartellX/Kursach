
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <fstream>
#include <string.h>

using namespace std;

class Node {


public:
	int index;
	Graph* main;
	Node* pred, * next;
	Edge* NodeEdges;
	Node() {
		NodeEdges = new Edge;
	}
	Node* getPtr() {
		return this;
	}

private:
	void refreshNodeEdges() {
		delete[] NodeEdges;


	}

};

struct Edge {
	Edge* pred, * next;
	Node* out = nullptr;
	int indexIn;
	int lable;
	int indexOut;
	Node* in = nullptr;
};


class Graph {
	friend class Node;

	string graphPath;
	ifstream graphFile;
	int nodeCount;
	int flag = 1;
	int flag2 = 1;
	Node* pNodeStart = nullptr, * ppredNode = nullptr, *ptecNode = nullptr, *pNodeFinish = nullptr;
	Edge* pEdgeStart = nullptr, * ppredEdge = nullptr, * ptecEdge = nullptr, * pEdgeFinish = nullptr;

public:	
	Graph() {
		nodeCount = 0;
		cout << "Укажите путь к графу (.tgf): ";
		cin >> graphPath;
		graphFile.open(graphPath);
		if (!graphFile.is_open()) {
			cout << "Ошибка открытия файла!\n";
			system("pause");
			exit(0);
		}
		char c[20];
		graphFile.getline(c,20);
		while (c[0]!='#') {
			nodeCount++;
			getNodes(c);
			graphFile.getline(c, 20);
		}
		
		cout << "Количество вершин: " << nodeCount<<endl;
		graphFile.getline(c, 20);
		while (!(graphFile.eof()||c[0]=='\n')) {
			getConnections(c);
			graphFile.getline(c, 20);
		}
	}
private: 
	void getNodes(char c[20]) {
		char* next_token1 = NULL;
		char* pch = strtok_s(c, " ", &next_token1);
		int index = atoi(pch);
		createNode(index);

	}

	void getConnections(char c[20]) {
		char* next_token1 = NULL;
		char* pch = strtok_s(c, " ", &next_token1);
		int nodeOut = atoi(pch);
		pch = strtok_s(NULL, " ", &next_token1);
		int nodeIn = atoi(pch);
		pch = strtok_s(NULL, " ", &next_token1);
		int Lable = atoi(pch);
		
		createEdge(nodeOut, nodeIn, Lable);

		printf("%d %d %d\n", nodeOut, nodeIn, Lable);
	}

	void createEdge(int indexOut, int indexIn, int Lable) {
		ptecEdge = new Edge;
		if (flag2) {
			pEdgeStart = ptecEdge;
			flag2 = 0;

		}
		else {
			ppredEdge->next = ptecEdge;
			ptecEdge->pred = ptecEdge;
		}

		ptecEdge->indexOut = indexOut;
		ptecEdge->out = searchNode(indexOut);
		ptecEdge->lable = Lable;
		ptecEdge->indexIn = indexIn;
		ptecEdge->in = searchNode(indexIn);
		ppredEdge = ptecEdge;
		pEdgeFinish = ptecEdge;
		
	}

	void createNode(int index){
		ptecNode = new Node;
		if (flag) {
			pNodeStart = ptecNode;
			flag = 0;
			
		}
		else {
			ppredNode->next = ptecNode;
			ptecNode->pred = ptecNode;
		}

		ptecNode->index = index;
		ppredNode = ptecNode;
		pNodeFinish = ptecNode;
	}

	Node* searchNode(int index) {
		ptecNode = pNodeStart;
		
		while (1) {
			if (ptecNode->index == index) {
				return ptecNode;
			}
			if (ptecNode == pNodeFinish) break;
			ptecNode = ptecNode->next;
		}

		cout << "\n[searchNodes] Нет вершины с таким индексом!\n";
	}

};






int main() {
	setlocale(0, "");

	Graph graph1;
	system("pause");
	
	return 1;
}