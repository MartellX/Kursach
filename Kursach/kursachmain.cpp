
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <fstream>
#include <string.h>
#include <iomanip>
//#include <vld.h>
using namespace std;


class UserString
{
public:
	char* s;
	UserString() 
	{
		s = new char[255]();
	}
	~UserString() 
	{
		delete[]s;
	}
	void operator=(char* _s)
	{
		strcpy_s(this->s,255, _s);
	}
	void operator=(const char* _s)
	{
		strcpy_s(this->s,255, _s);
	}
	void print()
	{
		cout << s;
	}
	void Format()
	{
		int len = strlen(s);
		cout << len;
	}
};
//------------------------------------------------------------ ������ � �������� ------------------------------------------------------------------------------

class Node {							


public:
	int index;
	string label;
	Node* pred, * next; // ������ ���������� � ��������� ������� � �����
	Node** toNode, **outNode, **NodeBuffer; // ������� ��������� �� ������ � ����������� �� �������, ���� ������ ������� ����� (toNode), � ������ (outNode)
	int toNodeCount, outNodeCount, flag1 = 1, flag2 = 1, flag3 = 1;

	Node() {
		toNode = new Node*(0);
		outNode = new Node*(0);
		NodeBuffer = new Node* (0);
		toNodeCount = 0;
		outNodeCount = 0;
		
	}
	Node* getPtr() {
		return this;
	}


	void refreshNodeEdges(Node* connectedNode, int vector) { // vector �������� ����������� (0 - ������ � ������ �������, 1 - ������� �� ������ �������)
		cout << "Node[" << index << "]\n";
		flag3 = 0;
		if (vector) {
			toBuffer(toNode, toNodeCount);
			delete[] toNode;
			toNodeCount++;
			toNode = new Node*[toNodeCount];
			outBuffer(toNode, toNodeCount-1);
			toNode[toNodeCount - 1] = connectedNode;
		}
		else {
			toBuffer(outNode, outNodeCount);
			delete[] outNode;
			outNodeCount++;
			outNode = new Node*[outNodeCount];
			outBuffer(outNode, outNodeCount - 1);
			outNode[outNodeCount - 1] = connectedNode;
		}
	}
	void deleteNodeEdges(Node* connectedNode, int vector) {
		flag3 = 0;
		int deletedIndex;
		if (vector) {
			toBuffer(toNode, toNodeCount);
			for (int i = 0; i < toNodeCount; i++) {
				if (toNode[i] == connectedNode) {
					deletedIndex = i;
					break;
				}
			}
			toNodeCount--;
			delete[] toNode;
			if (toNodeCount > 0) {
				
				toNode = new Node * [toNodeCount];
				outBuffer(toNode, toNodeCount + 1, deletedIndex);
				flag2 = 1;
			}
			else flag2 = 0;
		}
		else {
			toBuffer(outNode, outNodeCount);
			for (int i = 0; i < outNodeCount; i++) {
				if (outNode[i] == connectedNode) {
					deletedIndex = i;
					break;
				}
			}
			outNodeCount--;
			delete[] outNode;
			if (outNodeCount > 0) {

				outNode = new Node * [outNodeCount];
				outBuffer(outNode, outNodeCount + 1, deletedIndex);
				flag1 = 1;
			}
			else flag1 = 0;
			
		}		
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
		
	~Node() {
		if (flag1)
			delete[] outNode;
		if (flag2)
			delete[] toNode;
		if (flag3)
			delete[] NodeBuffer;
		
	}
private:

	void toBuffer(Node** array, int n) {
		NodeBuffer = new Node*[n];
		for (int i = 0; i < n; i++) {
			NodeBuffer[i] = array[i];
		}
	}

	void outBuffer(Node** array, int n) {
		for (int i = 0; i < n; i++) {
			array[i] = NodeBuffer[i] ;
		}
		delete[] NodeBuffer;
	}

	void outBuffer(Node** array, int n, int index) {
		int k = 0;
		for (int i = 0; i < n; i++) {
			if (i != index) {
				array[k] = NodeBuffer[i];
				k++;
			}
		}
		delete[] NodeBuffer;
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
	friend class Menu;
	string graphPath; //	���� � tgf
	ifstream graphFile;	
	
	int flag = 1; // ���� ��� ������
	int flag2 = 1; // ���� ��� ��� 
	int pathsCount;

public:	

	int nodeCount;	// ���������� ������ � �����
	int edgeCount;
	int indexGraph; // ������ �����
	Node* pNodeStart = nullptr, * ppredNode = nullptr, * ptecNode = nullptr, * pNodeFinish = nullptr;
	Edge* pEdgeStart = nullptr, * ppredEdge = nullptr, * ptecEdge = nullptr, * pEdgeFinish = nullptr;
	Graph* pred = nullptr, * next = nullptr;
	int** ConnectivityMatrix;

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

		ConnectivityMatrix = new int* [nodeCount];
		for (int i = 0; i < nodeCount; i++)
		{
			ConnectivityMatrix[i] = new int[nodeCount];
			for (int j = 0; j < nodeCount; j++) {
				ConnectivityMatrix[i][j] = 0;
			}
		}
		
		cout << "���������� ������: " << nodeCount<<endl;
		graphFile.getline(c, 20);
		while (!(graphFile.eof()||c[0]=='\n')) 
		{ // ������ ��� �� ����� ����� 
			getConnections(c);
			graphFile.getline(c, 20);
		}
		//searchNode(1)->showConnectedNodes();
		graphFile.close();

		
	};

	

	void printAllPathsInGraph() {
		pathsCount = 0;
		for (int i = 0; i < nodeCount; i++) {
			for (int j = 0; j < nodeCount; j++) {
					printAllPaths(i, j);
			}
		}

		cout << "\n����� �����: " << pathsCount <<endl;
	}

	void printAllPaths(int s, int d) {
		// ������� ������ ������������ ��� ���� ������
		bool* visited = new bool[nodeCount];

		// ������� ������ ��� �������� �����
		string* path = new string[nodeCount];
		int path_index = 0; // Initialize path[] as empty 
		int flagNode;
		if (s==d){
			flagNode = 0;
		}
		else {
			flagNode = 1;
		}
		
		// �������� ��� ������� �������������
		for (int i = 0; i < nodeCount; i++)
			visited[i] = false;

		// �������� ����������� �������, ����� �������� ��� ���� 
		printAllPathsUtil(s, d, visited, path, path_index, s, flagNode);
		delete[] path;
		delete[] visited;
	}

	void addNode() {

		string label;
		do {
			cout << "������� �������� �������: ";

			cin >> label;
		} while (searchNode(label) != nullptr);
		nodeCount++;
		createNode(nodeCount, label);
		expandMatrix();
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

		edgeCount++;
	}

	void changeNode() {
		string oldLabel, newLabel;
		do {
			cout << "�������� ����� ������� �� ������ ��������? (������� ��������)\n ";
			cin >> oldLabel;
		} while (searchNode(oldLabel) == nullptr);
		cout << "�� ����� ��������?\n ";
		cin >> newLabel;

		searchNode(oldLabel)->label = newLabel;
	}

	void changeEdge() {
		string outNode, inNode;
		int newWeight;
		Edge* tempEdge;
		do{
		cout << "������� �������� ��������� ������� ����: ";
		cin >> outNode;
		cout << "������� �������� �������� ������� ����: ";
		cin >> inNode;
		tempEdge = searchEdge(outNode, inNode);
		} while (tempEdge == nullptr);
		cout << "�������� ��� ����? (y/n)\n";
		if (_getch() == 'y') {
			cout << "������� ����� ��� ����: ";
			cin >> newWeight;
			tempEdge->lable = newWeight;
		};
		cout << "�������� ����������� ����? (y/n)\n";
		if (_getch() == 'y') {
			int tempWeight;
			tempWeight = tempEdge->lable;
			deleteEdge(tempEdge);
			createEdge(searchNode(inNode)->index, searchNode(outNode)->index, tempWeight);
		};
	}

	void removeNode() {
		string deletingLabel;
		do {
			cout << "������� �������� ��������� �������: ";
			cin >> deletingLabel;
		} while (searchNode(deletingLabel) == nullptr);

		deleteNode(searchNode(deletingLabel));
		reduceMatrix();
	}

	void removeEdge() {
		string outNode, inNode;
		Edge* deletingEdge;
		do {
			cout << "������� �������� ��������� ������� ��������� ����: ";
			cin >> outNode;
			cout << "������� �������� �������� ������� ��������� ����: ";
			cin >> inNode;
			deletingEdge = searchEdge(outNode, inNode);
		} while (deletingEdge == nullptr);

		deleteEdge(deletingEdge);
	}

	void ShowMatrix() {
		cout << setw(3)<<" ";
		for (int i = 0; i < nodeCount; i++) {
			cout <<" "<<setw(3)<<searchNode(i + 1)->label;
		}
		cout << endl;
		for (int i = 0; i < nodeCount; i++) {
			cout << setw(3)<< searchNode(i + 1)->label <<" ";
			for (int j = 0; j < nodeCount; j++) {
				cout << setw(3) << ConnectivityMatrix[i][j] << " ";
			}
			cout << endl;
		}
		system("pause");
	}

	
	~Graph() {
		ptecNode = pNodeStart;
		int N = nodeCount;
		for (int i = 0; i < N-1; i++) {
			
			ptecNode = ptecNode->next;
			deleteNode(ptecNode->pred);
			delete[] ConnectivityMatrix[i];
		}
		delete[] ConnectivityMatrix[N - 1];
		deleteNode(ptecNode);
		delete[] ConnectivityMatrix;
	}
private: 

	void printAllPathsUtil(int u, int d, bool visited[], string path[], int& path_index, int& StartNode, int& flagNode) {
		Node* uNode = searchNode(u+1); //������� ����� �������, � ������� ��������

		
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

			for (int i = 0; i < nodeCount; i++) { //���������� ������ ���������
				if (!visited[i] and ConnectivityMatrix[u][i]==1) //���� �� �������� ��������� ������ �������, ��������� � ���
					printAllPathsUtil(i, d, visited, path, path_index, StartNode, flagNode); // ���� ���������� ����� ��� �� ������ ������� � ��������.
			}
		}
		path_index--;
		visited[u] = false;

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
			pEdgeFinish->next = ptecEdge;
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
		cout << "[deleteEdge] ����: " << deletingEdge->lable<<endl;
		ConnectivityMatrix[deletingEdge->indexOut - 1][deletingEdge->indexIn - 1] = 0;
		if (deletingEdge->in != nullptr and deletingEdge->out != nullptr) {
			deletingEdge->in->deleteNodeEdges(deletingEdge->out, 0);
			deletingEdge->out->deleteNodeEdges(deletingEdge->in, 1);
		}
		delete deletingEdge;
		edgeCount--;
	}


	void createNode(int index, string label){ //�������� �������� ������
		ptecNode = new Node;
		if (flag) {
			pNodeStart = ptecNode;
			flag = 0;
			
		}
		else {
			pNodeFinish->next = ptecNode;
			ptecNode->pred = pNodeFinish;
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
			cout << "[deleteEdge] �������: " << deletingNode->label<<endl;
			int countNodeEdge = deletingNode->outNodeCount + deletingNode->toNodeCount;
			for (int i = 0; i<(countNodeEdge); i++)
				deleteEdge(searchEdge(deletingNode));
			refreshIndex(deletingNode->index);
			delete deletingNode;
			nodeCount--;
	}

	void refreshIndex(int deletedIndex) {
		Node* tempPtecNode = pNodeStart;
		Edge* tempPtecEdge = pEdgeStart;
		while (1) {
			if (tempPtecNode->index > deletedIndex) {
				tempPtecNode->index--;
			}
			if (tempPtecNode == pNodeFinish) break;
			tempPtecNode = tempPtecNode->next;
		}

		while (1) {
			if (tempPtecEdge->indexIn > deletedIndex) {
				tempPtecEdge->indexIn--;
			}
			if (tempPtecEdge->indexOut > deletedIndex) {
				tempPtecEdge->indexOut--;
			}
			if (tempPtecEdge == pEdgeFinish) break;
			tempPtecEdge = tempPtecEdge->next;
		}

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

	void reduceMatrix() {
		for (int i = 0; i < nodeCount + 1; i++) {
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

	Node* searchNode(int index) { // ������� ������������ ����� ������� � ������������ ��������
		Node* tempPtecNode = pNodeStart;
		
		while (1) {
			if (tempPtecNode->index == index) {
				return tempPtecNode;
			}
			if (tempPtecNode == pNodeFinish) break;
			tempPtecNode = tempPtecNode->next;
		}

		cout << "\n[searchNodes] ��� ������� � ����� ��������!\n";
	}

	Node* searchNode(string label) { // ������� ������������ ����� ������� � ������������ ��������
		Node* tempPtecNode = pNodeStart;

		while (1) {
			if (tempPtecNode->label == label) {
				return tempPtecNode;
			}
			if (tempPtecNode == pNodeFinish) break;
			tempPtecNode = tempPtecNode->next;
		}

		cout << "\n[searchNodes] ��� ������� � ����� ��������!\n";
		return nullptr;
	}

	Edge* searchEdge(Node* connectedNode) {
		Edge* tempPtecEdge = pEdgeStart;

		while (1) {
			if (tempPtecEdge->in == connectedNode or tempPtecEdge->out == connectedNode) {
				return tempPtecEdge;
			}
			if (tempPtecEdge == pEdgeFinish) break;
			tempPtecEdge = tempPtecEdge->next;
		}

		cout << "\n[searchEdge] ��� ���� � ����� ��������!\n";
		return nullptr;
	}

	Edge* searchEdge(int Weight) {
		Edge* tempPtecEdge = pEdgeStart;

		while (1) {
			if (tempPtecEdge->lable == Weight) {
				return tempPtecEdge;
			}
			if (tempPtecEdge == pEdgeFinish) break;
			tempPtecEdge = tempPtecEdge->next;
		}

		cout << "\n[searchEdge] ��� ���� � ����� �����!\n";
		return nullptr;
	}

	Edge* searchEdge(string outNode, string inNode) {
		Edge* tempPtecEdge = pEdgeStart;

		while (1) {
			if (tempPtecEdge->out->label == outNode and tempPtecEdge->in->label == inNode) {
				return tempPtecEdge;
			}
			if (tempPtecEdge == pEdgeFinish) break;
			tempPtecEdge = tempPtecEdge->next;
		}

		cout << "\n[searchEdge] ��� ���� � ������ ���������!\n";
		return nullptr;
	}

	int FIRST(Node* v) {
		if (v->toNodeCount > 0) return v->toNode[0]->index;
		else return -1;
	}

	int NEXT(int v, int i) {
		for (i; i < nodeCount; i++) {
			if (ConnectivityMatrix[v][i]) return i;
		}
		return NULL;
	}

	int NEXT(Node* v, int i) {
		if (v->toNodeCount > i) return v->toNode[i]->index;
		else return -1;
	}


	Node* VERTEX(Node* v, int i) {
		for (int j = 0; j < v->toNodeCount; j++) {
			if (i == v->toNode[j]->index) return v->toNode[j];
		}
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
			cout << "0. �����\n";
			char choice = _getch();
			switch (choice)
			{
			case('1'):
				LoadGraph();
				break;
			case('0'):
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
			cout << "0. �����\n";
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
			case('0'):
				exit(0);
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
		pFinishGraph = ptecGraph;
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
		cout << "4. ������� �������\n";
		cout << "5. ������� ����\n";
		cout << "6. �������� �������\n";
		cout << "7. �������� ����\n";
		cout << "f. [FIRST] ������ ������ ������� � ����� \n";
		cout << "n. [NEXT] ������ �������, ������� � ��������� ��������, ��������� �� ��������� ��������\n";
		cout << "v. [VERTEX] ������� � �������� i �� ��������� ������, ������� � v\n";
		cout << "m. ������� ��������� ����� \n";
		cout << "q. ������� ����\n";
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
		case('4'):
			ptecGraph->removeNode();
			graph(ptecGraph);
			break;
		case('5'):
			ptecGraph->removeEdge();
			graph(ptecGraph);
			break;
		case ('6'):
			ptecGraph->changeNode();
			graph(ptecGraph);
			break;
		case('7'):
			ptecGraph->changeEdge();
			graph(ptecGraph);
			break;
		case('f'):
			/*
			FIRST(ptecGraph);
			graph(ptecGraph);
			*/
			firstTest();
			graph(ptecGraph);
			break;
		case('n'):
			nextTest();
			graph(ptecGraph);
			break;
		case('v'):
			vertexTest();
			graph(ptecGraph);
			break;
		case('m'):
			ptecGraph->ShowMatrix();
			graph(ptecGraph);
			break;
		case('q'):
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

	void FIRST(Graph* graph) {
		cout << "������ ������ ������� � ����� " << graph->indexGraph << ": "<<graph->pNodeStart->index<<endl;
		cout << "Ÿ ��������: " << graph->pNodeStart->label<<endl;
		system("pause");
	}

	void firstTest() {
		cout << "������� �������� �������: ";
		string vLabel;
		cin >> vLabel;
		Node* vNode = ptecGraph->searchNode(vLabel);
		if (vNode == nullptr) {
			cout << "��� ����� �������!";
		}
		if (vNode != nullptr) {
			int returned = ptecGraph->FIRST(vNode);
			if (returned != -1) {
				Node* iNode = ptecGraph->searchNode(returned + 1);
				cout << "���������� ������� � �������� '" << iNode->index << "' � ��������� '" << iNode->label << "'\n";
			}
			else cout << "��� ������� ������!\n";
		}
		system("pause");
	}

	void nextTest() {
		int v, i, returned;
		string vLabel;
		Node* vNode;
		cout << "������� �������� �������: ";
		cin >> vLabel;
		cout << "������� ������: ";
		cin >> i;
		returned = ptecGraph->NEXT(ptecGraph->searchNode(vLabel)->index - 1, i);
		if (returned != NULL) {
			Node* iNode = ptecGraph->searchNode(returned + 1);
			cout << "���������� ������� � �������� '" << iNode->index << "' � ��������� '" << iNode->label << "'\n";
		}
		else cout << "��� ������� ������!\n";
		system("pause");
	}

	void vertexTest() {
		int v, i;
		string vLabel;
		Node* vNode, * returned;
		cout << "������� �������� �������: ";
		cin >> vLabel;
		cout << "������� ������: ";
		cin >> i;
		returned = ptecGraph->VERTEX(ptecGraph->searchNode(vLabel), i);
		if (returned != nullptr) {
			cout << "���������� ������� � �������� '" << returned->index << "' � ��������� '" << returned->label << "'\n";
		}
		else cout << "��� ����� �������!\n";
		system("pause");
	}
};



int main() {
	setlocale(0, "");
	Menu System;

	system("pause");
	
	return 1;
}