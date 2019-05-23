
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <fstream>
#include <string.h>

using namespace std;

//------------------------------------------------------------ ������ � �������� ------------------------------------------------------------------------------

class Node {							


public:
	int index;
	string label;
	Node* pred, * next; // ������ ���������� � ��������� ������� � �����
	Node** toNode, **outNode, **NodeBuffer; // ������� ��������� �� ������ � ����������� �� �������, ���� ������ ������� ����� (toNode), � ������ (outNode)
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


	void refreshNodeEdges(Node* connectedNode, int vector) { // vector �������� ����������� (0 - ������ � ������ �������, 1 - ������� �� ������ �������)
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
			//delete outNode;
			for (int i = 0; i < toNodeCount; i++) {
				if (toNode[i] == connectedNode) index = i;
			}
			toNodeCount--;
			toNode = new Node * [toNodeCount];
			outBuffer(toNode, toNodeCount,index);
		}
		else {
			toBuffer(outNode, outNodeCount);
			//delete outNode;
			for (int i = 0; i < outNodeCount; i++) {
				if (outNode[i] == connectedNode) index = i;
			}
			outNodeCount--;
			outNode = new Node * [outNodeCount];
			outBuffer(outNode, outNodeCount, index);
		}
		//delete[] NodeBuffer;
	}

	bool isConnectedTo(int index) {
		for (int i = 0; i < toNodeCount; i++) {
			if (toNode[i]->index == index) return 1;
		}
		return 0;
	}

	void showConnectedNodes() { //���������� ��� ���� ��������� � ������ ��������
		int nodeFlag = 0;
		printf("%6s| %6s\n\n", "����", "�����");
		int n = (toNodeCount < outNodeCount ? outNodeCount : toNodeCount); // ����� �������� ���, ������� �������� ����������
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
		for (int i = 0; i < n; i++) {
			if (i != index) {
				array[i] = NodeBuffer[i];
			}
		}
	}
};

//------------------------------------------------------------ ������ ��� ------------------------------------------------------------------------------

struct Edge {					
	Edge* pred, * next;
	Node* out = nullptr; // ����� �������, ������ ���� �������
	int indexIn;		// Ÿ ������
	int lable;
	int indexOut;		// ��������
	Node* in = nullptr;
};

//------------------------------------------------------------ ������ � ������ ------------------------------------------------------------------------------

class Graph {

	string graphPath; //	���� � tgf
	ifstream graphFile;	
	
	int flag = 1; // ���� ��� ������
	int flag2 = 1; // ���� ��� ��� 
	int pathsCount;

public:	

	int nodeCount;	// ���������� ������ � �����
	int edgeCount;
	Graph() {
		nodeCount = 0;
		cout << "������� ���� � ����� (.tgf): ";
		cin >> graphPath;
		graphFile.open(graphPath); // ������ ������ � ������ 
		while (!graphFile.is_open()) {
			cout << "������ �������� �����!\n"; // �������� �������� �����
			cout << "������� ���� � ����� (.tgf): ";
			cin >> graphPath;
			graphFile.open(graphPath);
		}
		char c[20];
		graphFile.getline(c,20); 
		while (c[0]!='#') 
		{ // ������ ������ �� ����������� "#"
			nodeCount++;
			getNodes(c);
			graphFile.getline(c, 20);
		}
		
		ConnectivityMatrix = new int*[nodeCount];
		cout << "���������� ������: " << nodeCount<<endl;
		graphFile.getline(c, 20);
		while (!(graphFile.eof()||c[0]=='\n')) 
		{ // ������ ��� �� ����� ����� 
			getConnections(c);
			graphFile.getline(c, 20);
		}
		//searchNode(1)->showConnectedNodes();
		graphFile.close();

		refreshMatrix();
	};

	int indexGraph; // ������ �����
	Node* pNodeStart = nullptr, * ppredNode = nullptr, * ptecNode = nullptr, * pNodeFinish = nullptr;
	Edge* pEdgeStart = nullptr, * ppredEdge = nullptr, * ptecEdge = nullptr, * pEdgeFinish = nullptr;
	Graph* pred = nullptr, * next = nullptr;
	int** ConnectivityMatrix;

	void printAllPathsInGraph() {
		pathsCount = 0;
		for (int i = 0; i < nodeCount; i++) {
			for (int j = 0; j < nodeCount; j++) {
					printAllPaths(i+1, j+1);
			}
		}

		cout << "\n����� �����: " << pathsCount <<endl;
	}

	void printAllPaths(int s, int d) {
		// ������� ������ ������������ ��� ���� ������
		bool* visited = new bool[nodeCount+1];

		// ������� ������ ��� �������� �����
		string* path = new string[nodeCount+1];
		int path_index = 0; // Initialize path[] as empty 
		int flagNode = 0;
		// �������� ��� ������� �������������
		for (int i = 0; i <= nodeCount; i++)
			visited[i] = false;

		// �������� ����������� �������, ����� �������� ��� ���� 
		printAllPathsUtil(s, d, visited, path, path_index, s, flagNode);
		
	}

	void addNode() {

		string label;
		do {
			cout << "������� �������� �������: ";

			cin >> label;
		} while (searchNode(label) != nullptr);
		nodeCount++;
		createNode(nodeCount, label);
		refreshMatrix();
	}

	void addEdge() {
		string outLabel, toLabel;
		int out, to, Weight;
		cout << "������� �������� ��������� �������: ";
		cin >> outLabel;

		cout << "������� �������� �������� �������: ";
		cin >> toLabel;

		cout << "������� ��� ����: ";
		cin >> Weight;

		out = searchNode(outLabel)->index;
		to = searchNode(toLabel)->index;
		createEdge(out, to, Weight);
		refreshMatrix();

		edgeCount++;
	}

	void changeNode() {
		string oldLabel, newLabel;
		cout << "�������� ����� ������� �� ������ ��������? (������� ��������)\n ";
		cin >> oldLabel;
		cout << "�� ����� ��������?\n ";
		cin >> newLabel;

		searchNode(oldLabel)->label = newLabel;
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
		}
		Edge* tempEdge = searchEdge(ptecNode);
		if (tempEdge != nullptr) {
			deleteEdge(tempEdge);
		}
		delete ptecNode;
		delete ConnectivityMatrix;
	}
private: 

	void printAllPathsUtil(int u, int d, bool visited[], string path[], int& path_index, int& StartNode, int& flagNode) {
		Node* uNode = searchNode(u); //������� ����� �������, � ������� ��������

		
		path[path_index] = uNode->label; //���������� � ���� ����� �������
		path_index++; //����������� ������ ����
		

		if (u == d and flagNode) {
			for (int i = 0; i < path_index; i++) {
				cout << path[i] << " "; //����� ������ ���� ���� ����, ������� ���
				
			}
			cout << endl;
			pathsCount++;
		}
		else
		{
			if (u == StartNode and !flagNode) {
				flagNode = 1;//���� �� � ������ ��� � ��������� �������, �� ������ �����, ��� �� ����� ����, �� �� ���������� � ����������
			}
			else {
				visited[u] = true;
			}

			for (int i = 0; i < uNode->toNodeCount; i++) { //������� ���������� ������� ������ �� ������ �������
				if (!visited[uNode->toNode[i]->index]) //���� �� �������� ��������� ������ �������, ��������� � ���
					printAllPathsUtil(uNode->toNode[i]->index, d, visited, path, path_index, StartNode, flagNode); // ���� ���������� ����� ��� �� ������ ������� � ��������.
			}
		}
		path_index--;
		visited[u] = false;

	}

	void deleteEdge(Edge* deletingEdge) {
		if (edgeCount > 1) {
			if (deletingEdge == pEdgeStart) {
				pEdgeStart = deletingEdge->next;
				deletingEdge->next->pred = nullptr;
			}
			if (deletingEdge == pEdgeFinish and deletingEdge != pEdgeStart) {
				pEdgeFinish = deletingEdge->pred;
				deletingEdge->pred->next = nullptr;
			}
			else {
				deletingEdge->pred->next = deletingEdge->next;
				deletingEdge->next->pred = deletingEdge->pred;
			}
		}
		cout << "[deleteEdge] ����: " << deletingEdge->lable;
		deletingEdge->in->deleteNodeEdges(deletingEdge->out, 0);
		deletingEdge->out->deleteNodeEdges(deletingEdge->in, 1);
		delete deletingEdge;
		edgeCount--;
	}
	void getNodes(char c[20]) {
		char* next_token1 = NULL; // ����� ����� ��� ������ strtok_s
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

	void createEdge(int indexOut, int indexIn, int Lable) { // �������� ������ ���
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
		
	}

	void createNode(int index, string label){ //�������� �������� ������
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

	void refreshMatrix() {
		delete ConnectivityMatrix;
		ConnectivityMatrix = new int* [nodeCount];
		for (int i = 0; i < nodeCount; i++)
		{
			ConnectivityMatrix[i] = new int[nodeCount];
			for (int j = 0; j < nodeCount; j++) {
				ConnectivityMatrix[i][j] = (searchNode(i + 1)->isConnectedTo(j + 1));
			}
		}
	}

	Node* searchNode(int index) { // ������� ������������ ����� ������� � ������������ ��������
		ptecNode = pNodeStart;
		
		while (1) {
			if (ptecNode->index == index) {
				return ptecNode;
			}
			if (ptecNode == pNodeFinish) break;
			ptecNode = ptecNode->next;
		}

		cout << "\n[searchNodes] ��� ������� � ����� ��������!\n";
	}

	Node* searchNode(string label) { // ������� ������������ ����� ������� � ������������ ��������
		ptecNode = pNodeStart;

		while (1) {
			if (ptecNode->label == label) {
				return ptecNode;
			}
			if (ptecNode == pNodeFinish) break;
			ptecNode = ptecNode->next;
		}

		cout << "\n[searchNodes] ��� ������� � ����� ��������!\n";
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

		cout << "\n[searchEdge] ��� ���� � ����� ��������!\n";
		return nullptr;
	}
};

//------------------------------------------------------------ ������ � ���� ------------------------------------------------------------------------------


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
			cout << "�������� ��������: \n";
			cout << "1. ��������� ���� �� �����\n";
			cout << "2. �����\n";
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
				cout << "��� ������ ��������!\n\n";
				MainMenu();
				break;
			}
		}
		else {
			cout << "�������� ��������: \n";
			cout << "1. ��������� ���� �� �����\n";
			cout << "2. ���������� �������� �����\n";
			cout << "3. �����\n";
			char choice = _getch();
			int index;
			switch (choice)
			{
			case('1'):
				LoadGraph();
				break;
			case('2'):
				cout << "������� �����: ";
				ptecGraph = pStartGraph;
				for (int i = 0; i < GraphCount; i++) {
					cout << ptecGraph->indexGraph << " ";
					ptecGraph = ptecGraph->next;
				}
				cout << endl;
				cout << "����� ���� ������ �������?\n";
				cin >> index;
				OpenGraph(index);
			case('3'):
				exit(0);
				break;
			default:
				cout << "��� ������ ��������!\n\n";
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
		graph(ptecGraph);
	}

	void graph(Graph* pointerGraph) {
		system("cls");
		ptecGraph = pointerGraph;

		cout << "[������ � �������� ������]\n";
		cout << "�������� ��������:\n";
		cout << "1. ��������� ������� �� ��������\n";
		cout << "2. �������� �������\n";
		cout << "3. �������� ����\n";
		//cout << "3. ������� �������\n";
		cout << "4. �������� �������\n";
		cout << "5. �������� ����\n";
		cout << "6. ������ ������ ������� � ����� \n";
		cout << "7. ������ �������, ������� � ��������� ��������, ��������� �� ��������� ��������\n";
		//cout << "8. ������� � �������� i �� ��������� ������, ������� � v\n";
		cout << "8. ������� ��������� ����� \n";
		cout << "9. ������� ����\n";
		cout << "0. ��������� � ������� ����\n";
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
		case ('4'):
			ptecGraph->changeNode();
			graph(ptecGraph);
			break;
		case('8'):
			ShowMatrix();
			graph(ptecGraph);
			break;
		case('9'):
			closeGraph(ptecGraph);
			break;
		case('0'):
			break;
		default:
			cout << "��� ������ ��������!";
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
		cout << "[OpenGraph] ��� ������ �����!\n";
	}

	void closeGraph(Graph* closingGraph) {
		if (closingGraph->pred != nullptr)
		closingGraph->pred->next = closingGraph->next;
		if (closingGraph->next != nullptr)
		closingGraph->next->pred = closingGraph->pred;
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