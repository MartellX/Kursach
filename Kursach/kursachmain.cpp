
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <fstream>
#include <string.h>

using namespace std;

//------------------------------------------------------------ Работа с вершиной ------------------------------------------------------------------------------

class Node {							


public:
	int index;
	string label;
	Node* pred, * next; // Адреса предыдущей и следующей вершины в графе
	Node** toNode, **outNode, **NodeBuffer; // Будущие указатели на массив с указателями на Вершины, куда данная Вершина ведет (toNode), и откуда (outNode)
	int toNodeCount, outNodeCount;

	Node() {
		toNode = new Node*[0];
		outNode = new Node*[0];
		NodeBuffer = new Node* [0];
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
			//delete[toNodeCount] toNode;
			toNodeCount++;
			toNode = new Node*[toNodeCount];
			outBuffer(toNode, toNodeCount-1);
			toNode[toNodeCount - 1] = connectedNode;
		}
		else {
			toBuffer(outNode, outNodeCount);
			//delete[outNodeCount] outNode;
			outNodeCount++;
			outNode = new Node*[toNodeCount];
			outBuffer(outNode, outNodeCount - 1);
			outNode[outNodeCount - 1] = connectedNode;
		}
		//delete[] NodeBuffer;
	}
	void deleteNodeEdges(Node* connectedNode, int vector) {
		
		if (vector) {
			toBuffer(toNode, toNodeCount);
			for (int i = 0; i < toNodeCount; i++) {
				if (toNode[i] == connectedNode) index = i;
			}
			toNodeCount--;
			delete toNode;
			toNode = new Node * [toNodeCount];
			outBuffer(toNode, toNodeCount+1,index);
		}
		else {
			toBuffer(outNode, outNodeCount);
			for (int i = 0; i < outNodeCount; i++) {
				if (outNode[i] == connectedNode) index = i;
			}
			outNodeCount--;
			delete outNode;
			outNode = new Node * [outNodeCount];
			outBuffer(outNode, outNodeCount+1, index);
		}
		//delete[] NodeBuffer;
	}

	bool isConnectedTo(int index) {
		for (int i = 0; i < toNodeCount; i++) {
			if (toNode[i]->index == index) return 1;
		}
		return 0;
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
		delete [] NodeBuffer;
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

	void outBuffer(Node** array, int n, int index) {
		int k = 0;
		for (int i = 0; i < n; i++) {
			if (i != index) {
				array[k] = NodeBuffer[i];
				k++;
			}
		}
	}
};

//------------------------------------------------------------ Список дуг ------------------------------------------------------------------------------

struct Edge {					
	Edge* pred, * next;
	Node* out = nullptr; // Адрес вершины, откуда дуга выходит
	int indexIn;		// Её индекс
	int lable;
	int indexOut;		// Наоборот
	Node* in = nullptr;
};

//------------------------------------------------------------ Работа с графом ------------------------------------------------------------------------------

class Graph {

	string graphPath; //	Путь к tgf
	ifstream graphFile;	
	
	int flag = 1; // Флаг для вершин
	int flag2 = 1; // Флаг для дуг 
	int pathsCount;

public:	

	int nodeCount;	// Количество вершин в графе
	int edgeCount;
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

		ConnectivityMatrix = new int* [nodeCount];
		for (int i = 0; i < nodeCount; i++)
		{
			ConnectivityMatrix[i] = new int[nodeCount];
			for (int j = 0; j < nodeCount; j++) {
				ConnectivityMatrix[i][j] = 0;
			}
		}
		
		cout << "Количество вершин: " << nodeCount<<endl;
		graphFile.getline(c, 20);
		while (!(graphFile.eof()||c[0]=='\n')) 
		{ // Парсер дуг до конца файла 
			getConnections(c);
			graphFile.getline(c, 20);
		}
		//searchNode(1)->showConnectedNodes();
		graphFile.close();

		
	};

	int indexGraph; // Индекс графа
	Node* pNodeStart = nullptr, * ppredNode = nullptr, * ptecNode = nullptr, * pNodeFinish = nullptr;
	Edge* pEdgeStart = nullptr, * ppredEdge = nullptr, * ptecEdge = nullptr, * pEdgeFinish = nullptr;
	Graph* pred = nullptr, * next = nullptr;
	int** ConnectivityMatrix;

	void printAllPathsInGraph() {
		pathsCount = 0;
		for (int i = 0; i < nodeCount; i++) {
			for (int j = 0; j < nodeCount; j++) {
					printAllPaths(i, j);
			}
		}

		cout << "\nВсего путей: " << pathsCount <<endl;
	}

	void printAllPaths(int s, int d) {
		// Создаем массив посещенности для всех вершин
		bool* visited = new bool[nodeCount];

		// Создаем массив для хранения путей
		string* path = new string[nodeCount];
		int path_index = 0; // Initialize path[] as empty 
		int flagNode;
		if (s==d){
			flagNode = 0;
		}
		else {
			flagNode = 1;
		}
		
		// Отмечаем все вершины непосещенными
		for (int i = 0; i < nodeCount; i++)
			visited[i] = false;

		// Вызываем рекурсивную функцию, чтобы выводить все пути 
		printAllPathsUtil(s, d, visited, path, path_index, s, flagNode);
		
	}

	void addNode() {

		string label;
		do {
			cout << "Введите название вершины: ";

			cin >> label;
		} while (searchNode(label) != nullptr);
		nodeCount++;
		createNode(nodeCount, label);
		expandMatrix();
	}

	void addEdge() {
		string outLabel, toLabel;
		int out, to, Weight;
		cout << "Введите название начальной вершины: ";
		cin >> outLabel;

		cout << "Введите название конечной вершины: ";
		cin >> toLabel;

		cout << "Введите вес дуги: ";
		cin >> Weight;

		out = searchNode(outLabel)->index;
		to = searchNode(toLabel)->index;
		createEdge(out, to, Weight);

		edgeCount++;
	}

	void changeNode() {
		string oldLabel, newLabel;
		cout << "Название какой вершины вы хотите поменять? (введите название)\n ";
		cin >> oldLabel;
		cout << "На какое название?\n ";
		cin >> newLabel;

		searchNode(oldLabel)->label = newLabel;
	}

	void removeNode() {
		string deletingLabel;
		do {
			cout << "Введите название удаляемой вершины: ";
			cin >> deletingLabel;
		} while (searchNode(deletingLabel) == nullptr);

		deleteNode(searchNode(deletingLabel));
	}

	~Graph() {
		ptecNode = pNodeStart;
		for (int i = 0; i < nodeCount-1; i++) {
			Edge* tempEdge = searchEdge(ptecNode->pred);
			ptecNode = ptecNode->next;
			if (tempEdge != nullptr) {
				deleteEdge(tempEdge);
			}
			
			delete ptecNode->pred;
			delete[] ConnectivityMatrix[i];
		}
		Edge* tempEdge = searchEdge(ptecNode);
		if (tempEdge != nullptr) {
			deleteEdge(tempEdge);
		}
		delete ptecNode;
		delete[] ConnectivityMatrix;
	}
private: 

	void printAllPathsUtil(int u, int d, bool visited[], string path[], int& path_index, int& StartNode, int& flagNode) {
		Node* uNode = searchNode(u+1); //Находим адрес вершины, с которой работаем

		
		path[path_index] = uNode->label; //Записываем в путь метку вершины
		path_index++; //Увеличываем индекс пути
		

		if (u == d and flagNode) {
			for (int i = 0; i < path_index; i++) {
				cout << path[i] << " "; //Когда обошли весь один путь, выводим его
				
			}
			cout << endl;
			pathsCount++;
		}
		else
		{
			if (u == StartNode and !flagNode) {
				flagNode = 1;//Если мы в первый раз в начальной вершине, то делаем метку, что мы здесь были, но не записываем в посещенные
			}
			else {
				visited[u] = true;
			}

			for (int i = 0; i < nodeCount; i++) { //Перебираем массив смежности
				if (!visited[i] and ConnectivityMatrix[u][i]==1) //Если не посещали следующую смежну вершину, переходим к ней
					printAllPathsUtil(i, d, visited, path, path_index, StartNode, flagNode); // ищем количество путей уже из данной вершины в конечную.
			}
		}
		path_index--;
		visited[u] = false;

	}

	
	void getNodes(char c[20]) {
		char* next_token1 = NULL; // Токен нужен для работы strtok_s
		char* pch = strtok_s(c, " ", &next_token1);
		int index = atoi(pch);
		pch = strtok_s(NULL, " ", &next_token1);
		string label = pch;
		createNode(index, pch);

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
			ptecEdge->pred = ppredEdge;
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
		edgeCount++;
		
		ConnectivityMatrix[indexOut - 1][indexIn - 1] = 1;
	}

	void deleteEdge(Edge* deletingEdge) {
		if (edgeCount > 1) {
			if (deletingEdge == pEdgeStart) {
				pEdgeStart = deletingEdge->next;
				deletingEdge->next->pred = nullptr;
			}
			else if (deletingEdge == pEdgeFinish and deletingEdge != pEdgeStart) {
				pEdgeFinish = deletingEdge->pred;
				deletingEdge->pred->next = nullptr;
			}
			else {
				deletingEdge->pred->next = deletingEdge->next;
				deletingEdge->next->pred = deletingEdge->pred;
			}
		}
		cout << "[deleteEdge] Дуга: " << deletingEdge->lable;
		ConnectivityMatrix[deletingEdge->indexOut - 1][deletingEdge->indexIn - 1] = 0;
		if (deletingEdge->in != nullptr and deletingEdge->out != nullptr) {
			deletingEdge->in->deleteNodeEdges(deletingEdge->out, 0);
			deletingEdge->out->deleteNodeEdges(deletingEdge->in, 1);
		}
		delete deletingEdge;
		edgeCount--;
	}


	void createNode(int index, string label){ //Создание объектов вершин
		ptecNode = new Node;
		if (flag) {
			pNodeStart = ptecNode;
			flag = 0;
			
		}
		else {
			ppredNode->next = ptecNode;
			ptecNode->pred = ppredNode;
		}

		ptecNode->label = label;
		ptecNode->index = index;
		ppredNode = ptecNode;
		pNodeFinish = ptecNode;
	}

	void deleteNode(Node* deletingNode) {
			if (nodeCount > 1) {
				if (deletingNode == pNodeStart) {
					pNodeStart = deletingNode->next;
					deletingNode->next->pred = nullptr;
				}
				else if (deletingNode == pNodeFinish and deletingNode != pNodeStart) {
					pNodeFinish = deletingNode->pred;
					deletingNode->pred->next = nullptr;
				}
				else {
					deletingNode->pred->next = deletingNode->next;
					deletingNode->next->pred = deletingNode->pred;
				}
			}
			cout << "[deleteEdge] Вершина: " << deletingNode->label;
			for (int i = 0; i<(deletingNode->outNodeCount+deletingNode->toNodeCount); i++)
				deleteEdge(searchEdge(deletingNode));
			delete deletingNode;
			nodeCount--;
	}
	void expandMatrix() {
		
		for (int i = 0; i < nodeCount-1; i++) {
			delete[] ConnectivityMatrix[i];
		}

		delete[] ConnectivityMatrix;
		ConnectivityMatrix = new int* [nodeCount];
		for (int i = 0; i < nodeCount; i++)
		{
			
			ConnectivityMatrix[i] = new int[nodeCount];
			for (int j = 0; j < nodeCount; j++) {
				ConnectivityMatrix[i][j] = (searchNode(i + 1)->isConnectedTo(j + 1));
			}
		}
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

	Node* searchNode(string label) { // Фукнция возвращающая адрес объекта с определенным индексом
		ptecNode = pNodeStart;

		while (1) {
			if (ptecNode->label == label) {
				return ptecNode;
			}
			if (ptecNode == pNodeFinish) break;
			ptecNode = ptecNode->next;
		}

		cout << "\n[searchNodes] Нет вершины с таким индексом!\n";
		return nullptr;
	}

	Edge* searchEdge(Node* connectedNode) {
		ptecEdge = pEdgeStart;

		while (1) {
			if (ptecEdge->in == connectedNode or ptecEdge->out == connectedNode) {
				return ptecEdge;
			}
			if (ptecEdge == pEdgeFinish) break;
			ptecEdge = ptecEdge->next;
		}

		cout << "\n[searchEdge] Нет дуги с такой вершиной!\n";
		return nullptr;
	}
};

//------------------------------------------------------------ Работа с меню ------------------------------------------------------------------------------


class Menu {
	Graph* ptecGraph = nullptr, * ppredGraph = nullptr, * pnextGraph = nullptr, * pStartGraph = nullptr, * pFinishGraph = nullptr;
public:
	
	Menu(){
		ptecGraph = nullptr;
		MainMenu();
	}
	~Menu() {
		ptecGraph = pStartGraph;
		for (int i = 0; i < GraphCount - 1; i++) {
			ptecGraph = ptecGraph->next;
			delete ptecGraph->pred;
		}
		delete ptecGraph;
	}
private:
	
	int flagGraph = 1, GraphCount = 0;
	void MainMenu() {
		system("cls");
		if (GraphCount == 0) {
			cout << "Выберите действие: \n";
			cout << "1. Загрузить граф из файла\n";
			cout << "2. Выйти\n";
			char choice = _getch();
			switch (choice)
			{
			case('1'):
				LoadGraph();
				break;
			case('2'):
				exit(0);
				break;
			default:
				cout << "Нет такого действия!\n\n";
				MainMenu();
				break;
			}
		}
		else {
			cout << "Выберите действие: \n";
			cout << "1. Загрузить граф из файла\n";
			cout << "2. Посмотреть открытые графы\n";
			cout << "3. Выйти\n";
			char choice = _getch();
			int index;
			switch (choice)
			{
			case('1'):
				LoadGraph();
				break;
			case('2'):
				cout << "Открыты графы: ";
				ptecGraph = pStartGraph;
				for (int i = 0; i < GraphCount; i++) {
					cout << ptecGraph->indexGraph << " ";
					ptecGraph = ptecGraph->next;
				}
				cout << endl;
				cout << "Какой граф хотите открыть?\n";
				cin >> index;
				OpenGraph(index);
			case('3'):
				exit(0);
				break;
			default:
				cout << "Нет такого действия!\n\n";
				MainMenu();
				break;
			}
		}

	}
	void LoadGraph(){
		system("cls");

		GraphCount++;

		ptecGraph = new Graph;

		if (flagGraph) {
			pStartGraph = ptecGraph;
			flagGraph = 0;
		}
		else {
			ppredGraph->next = ptecGraph;
			ptecGraph->pred = ppredGraph;
		}
		ptecGraph->indexGraph = GraphCount;
		ppredGraph = ptecGraph;
		pFinishGraph = ptecGraph;
		graph(ptecGraph);
	}

	void graph(Graph* pointerGraph) {
		system("cls");
		ptecGraph = pointerGraph;

		cout << "[Работа с открытым графом]\n";
		cout << "Выберите действие:\n";
		cout << "1. Выполнить задание по варианту\n";
		cout << "2. Добавить вершину\n";
		cout << "3. Добавить дугу\n";
		cout << "4. Удалить вершину\n";
		cout << "6. Изменить вершину\n";
		cout << "7. Изменить дугу\n";
		cout << "8. Индекс первой вершины в графе \n";
		cout << "9. Индекс вершины, смежной с выбранной вершиной, следующий за выбранным индексом\n";
		//cout << "8. Вершина с индексом i из множества вершин, смежных с v\n";
		cout << "m. Матрица смежности графа \n";
		cout << "q. Закрыть граф\n";
		cout << "0. Вернуться в главное меню\n";
		char choice = _getch();

		switch (choice)
		{
		case ('1'):
			ptecGraph->printAllPathsInGraph();
			system("pause");
			graph(ptecGraph);
			break;
		case ('2'):
			ptecGraph->addNode();
			graph(ptecGraph);
			break;
		case ('3'):
			ptecGraph->addEdge();
			graph(ptecGraph);
			break;
		case('4'):
			ptecGraph->removeNode();
			graph(ptecGraph);
			break;
		case ('6'):
			ptecGraph->changeNode();
			graph(ptecGraph);
			break;
		case('m'):
			ShowMatrix();
			graph(ptecGraph);
			break;
		case('q'):
			closeGraph(ptecGraph);
			break;
		case('0'):
			break;
		default:
			cout << "Нет такого действия!";
			graph(ptecGraph);
			break;
		}
		MainMenu();
	}
	
	void ShowMatrix() {
		system("cls");


		for (int i = 0; i < ptecGraph->nodeCount; i++) {
			for (int j = 0; j < ptecGraph->nodeCount; j++) {
				cout << ptecGraph->ConnectivityMatrix[i][j] << " ";
			}
			cout << endl;
		}
		system("pause");
	}

	void OpenGraph(int index) {
		ptecGraph = pStartGraph;
		while (1 and ptecGraph!=nullptr) {
			if (ptecGraph->indexGraph == index) {
				graph(ptecGraph);
				break;
			}
			else if (ptecGraph == pFinishGraph) break;
			ptecGraph = ptecGraph->next;
		}
		cout << "[OpenGraph] Нет такого графа!\n";
	}

	void closeGraph(Graph* closingGraph) {
		if (GraphCount > 1) {
			if (closingGraph == pStartGraph) {
				pStartGraph = closingGraph->next;
				closingGraph->next->pred = nullptr;
			}
			else if (closingGraph == pFinishGraph and closingGraph != pStartGraph) {
				pFinishGraph = closingGraph->pred;
				closingGraph->pred->next = nullptr;
			}
			else {
				closingGraph->pred->next = closingGraph->next;
				closingGraph->next->pred = closingGraph->pred;
			}
		}
		delete closingGraph;

		GraphCount--;
	}
};



int main() {
	setlocale(0, "");
	Menu System;

	system("pause");
	
	return 1;
}