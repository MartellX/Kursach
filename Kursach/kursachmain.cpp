
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <fstream>
#include <string.h>

using namespace std;

class Node {							//����� ��� �������


public:
	int index;
	Node* pred, * next; // ������ ���������� � ��������� ������� � �����
	Node** toNode, **outNode, **NodeBuffer; // ������� ��������� �� ������ � ����������� �� �������, ���� ������ ������� ����� (toNode), � ������ (outNode)
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

	void refreshNodeEdges(Node* connectedNode, int vector) { // vector �������� ����������� (0 - ������ � ������ �������, 1 - ������� �� ������ �������)
		//cout << "Node[" << index << "]\n";
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

struct Edge {					// ������ ���
	Edge* pred, * next;
	Node* out = nullptr; // ����� �������, ������ ���� �������
	int indexIn;		// Ÿ ������
	int lable;
	int indexOut;		// ��������
	Node* in = nullptr;
};


class Graph {

	string graphPath; //	���� � tgf
	ifstream graphFile;	
	
	int flag = 1; // ���� ��� ������
	int flag2 = 1; // ���� ��� ��� 
	

public:	

	int nodeCount;	// ���������� ������ � �����
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

	
private: 
	void getNodes(char c[20]) {
		char* next_token1 = NULL; // ����� ����� ��� ������ strtok_s
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

		//printf("%d %d %d\n", nodeOut, nodeIn, Lable);
	}

	void createEdge(int indexOut, int indexIn, int Lable) { // �������� ������ ���
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

	void createNode(int index){ //�������� �������� ������
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

};


class Menu {
	Graph* ptecGraph = nullptr, * ppredGraph = nullptr, * pnextGraph = nullptr, * pStartGraph = nullptr, * pFinishGraph = nullptr;

public:
	
	Menu(){
		ptecGraph = nullptr;
		MainMenu();
	}
private:
	
	int flagGraph = 1, GraphCount = 0;
	void MainMenu() {
		system("cls");
		cout << "�������� ��������: \n";
		cout << "1. ��������� ���� �� �����\n";
		cout << "2. �����\n";
		
		char choice =_getch();
		switch (choice)
		{
		case('1'):
			LoadGraph();
			break;
		default:
			cout << "��� ������ ��������!\n\n";
			MainMenu();
			break;
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
		graph(ptecGraph);
	}

	void graph(Graph* pointerGraph) {
		system("cls");
		ptecGraph = pointerGraph;

		cout << "[������ � �������� ������]\n";
		cout << "�������� ��������:\n";
		cout << "1. ��������� ������� �� ��������\n";
		cout << "2. �������� �������\n";
		cout << "3. ������� �������\n";
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
		case('8'):
			ShowMatrix();
			graph(ptecGraph);
			break;
		case('0'):
			break;
		default:
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
		while (1) {
			if (ptecGraph->indexGraph == index) {
				Graph();
				break;
			}
			else if (ptecGraph == pFinishGraph) break;
			ptecGraph = ptecGraph->next;
		}
		cout << "[OpenGraph] ��� ������ �����!\n";
	}

};



int main() {
	setlocale(0, "");
	Menu System;

	system("pause");
	
	return 1;
}