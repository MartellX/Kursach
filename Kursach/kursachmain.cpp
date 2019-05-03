
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <fstream>
#include <string.h>

using namespace std;

class Node {							//Класс для вершины


public:
	int index;
	Node* pred, * next; // Адреса предыдущей и следующей вершины в графе
	Node** toNode, **outNode, **NodeBuffer; // Будущие указатели на массив с указателями на Вершины, куда данная Вершина ведет (toNode), и откуда (outNode)
	int toNodeCount, outNodeCount;

	Node() {
		toNode = new Node*[0];
		outNode = new Node*[0];
		toNodeCount = 0;
		outNodeCount = 0;
		
	}
	Node* getPtr() {
		return this;
	}

	void refreshNodeEdges(Node* connectedNode, int vector) { // vector означает направление (0 - входит в данную вершину, 1 - выходит из данной вершины)
		cout << "Node[" << index << "]\n";
		if (vector) {
			toBuffer(toNode, toNodeCount);
			//delete[] toNode;
			toNodeCount++;
			toNode = new Node*[toNodeCount];
			outBuffer(toNode, toNodeCount-1);
			toNode[toNodeCount - 1] = connectedNode;
		}
		else {
			toBuffer(outNode, outNodeCount);
			//delete[] outNode;
			outNodeCount++;
			outNode = new Node*[toNodeCount];
			outBuffer(outNode, outNodeCount - 1);
			outNode[outNodeCount - 1] = connectedNode;
		}
	}

	void showConnectedNodes() { //Показывает все дуги связанные с данной вершиной
		int nodeFlag = 0;
		printf("%6s| %6s\n\n", "Вход", "Выход");
		int n = (toNodeCount < outNodeCount ? outNodeCount : toNodeCount); // Нужно показать все, поэтому выбираем наибольшее
		if (n == toNodeCount) nodeFlag = 1;
		for (int i = 0; i < n; i++) {
			if (nodeFlag and i>=outNodeCount) {
				printf("%6s| %6d\n","", toNode[i]->index);
			}
			else if (!nodeFlag and i >= toNodeCount) {
				printf("%6d| %6s\n", outNode[i]->index, "");
			}
			else {
				printf("%6d| %6d\n", outNode[i]->index, toNode[i]->index);
			}
		}
	}
		

private:
	void toBuffer(Node** array, int n) {
		delete[] NodeBuffer;
		NodeBuffer = new Node*[n];
		for (int i = 0; i < n; i++) {
			NodeBuffer[i] = array[i];
		}
	}

	void outBuffer(Node** array, int n) {
		for (int i = 0; i < n; i++) {
			array[i] = NodeBuffer[i] ;
		}
	}
};

struct Edge {					// Список дуг
	Edge* pred, * next;
	Node* out = nullptr; // Адрес вершины, откуда дуга выходит
	int indexIn;		// Её индекс
	int lable;
	int indexOut;		// Наоборот
	Node* in = nullptr;
};


class Graph {
	friend class Node; 

	string graphPath; //	Путь к tgf
	ifstream graphFile;	
	int nodeCount;	// Количество вершин в графе
	int flag = 1; // Флаг для вершин
	int flag2 = 1; // Флаг для дуг 
	int indexGraph; // Индекс графа
	Node* pNodeStart = nullptr, * ppredNode = nullptr, *ptecNode = nullptr, *pNodeFinish = nullptr;
	Edge* pEdgeStart = nullptr, * ppredEdge = nullptr, * ptecEdge = nullptr, * pEdgeFinish = nullptr;

public:	
	Graph() {
		nodeCount = 0;
		cout << "Укажите путь к графу (.tgf): ";
		cin >> graphPath;
		graphFile.open(graphPath); // Начало работы с файлом 
		while (!graphFile.is_open()) {
			cout << "Ошибка открытия файла!\n"; // Проверка открытия файла
			cout << "Укажите путь к графу (.tgf): ";
			cin >> graphPath;
			graphFile.open(graphPath);
		}
		char c[20];
		graphFile.getline(c,20); 
		while (c[0]!='#') 
		{ // Парсер вершин до разделителя "#"
			nodeCount++;
			getNodes(c);
			graphFile.getline(c, 20);
		}
		
		cout << "Количество вершин: " << nodeCount<<endl;
		graphFile.getline(c, 20);
		while (!(graphFile.eof()||c[0]=='\n')) 
		{ // Парсер дуг до конца файла 
			getConnections(c);
			graphFile.getline(c, 20);
		}

		graphFile.close();
	};
private: 
	void getNodes(char c[20]) {
		char* next_token1 = NULL; // Токен нужен для работы strtok_s
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

	void createEdge(int indexOut, int indexIn, int Lable) { // Создание списка дуг
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

		ptecEdge->in->refreshNodeEdges(ptecEdge->out, 0);
		ptecEdge->out->refreshNodeEdges(ptecEdge->in, 1);
		ppredEdge = ptecEdge;
		pEdgeFinish = ptecEdge;
		
	}

	void createNode(int index){ //Создание объектов вершин
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

	Node* searchNode(int index) { // Фукнция возвращающая адрес объекта с определенным индексом
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


class MainMenu {


};



int main() {
	setlocale(0, "");


	system("pause");
	
	return 1;
}