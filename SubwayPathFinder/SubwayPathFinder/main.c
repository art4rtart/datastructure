#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "main.h"

#define MAX_VTXS 1000
#define INF 9999
#define NAMESIZE 500

// ���� �ڵ� -----------------------------------------------------------------------

#pragma region basic code
typedef char VtxData;			// �׷��� ������ ������ �������� �ڷ��� 
int adj[MAX_VTXS][MAX_VTXS];	// ���� ��� 
//int myarr[MAX_VTXS][MAX_VTXS];	// ���� ��� 
int vsize;						// ��ü ������ ����					
VtxData vdata[MAX_VTXS];		// ������ ������ ������ �迭

void error(char str[]) { printf("%s\n", str);	exit(1); }
int is_empty_graph() { return (vsize == 0); }
int is_full_graph() { return (vsize >= MAX_VTXS); }

// �׷��� �ʱ�ȭ
void init_graph()
{
	int i, j;
	vsize = 0;
	for (i = 0; i<MAX_VTXS; i++)
		for (j = 0; j<MAX_VTXS; j++)
			adj[i][j] = 0;
}

// �׷��� ���� �Է�
void insert_vertex(char* name)
{
	if (is_full_graph())
		error("Error: �׷��� ������ ���� �ʰ�\n");
	else
		vdata[vsize++] = name;
}

// �׷��� ���� �߰�
void insert_edge(int u, int v, int val)
{
	adj[u][v] = val;
}

// �׷��� ���� �߰� 2
void insert_edge2(int u, int v, int val)
{
	adj[u][v] = adj[v][u] = val;
}
#pragma endregion

// ����� �ڷᱸ�� -----------------------------------------------------------------

struct Subway
{
	char stationName[NAMESIZE];			// ���� �� �̸�
	char nextStationName[NAMESIZE];		// ���� �� �̸�

	int stationNum;					// ���� �� ȣ��

	int currentNum;					// ���� �� ��ȣ
	int nextNum;					// ���� �� ��ȣ

	int weight;						// ���翪�� ���������� �Ÿ�	(����ġ)

	int isChangeStation;			// ȯ�¿����� ����
	int changeStationStationNum;	// ȯ�� �� ȣ�� ��ȣ
	int changeStationNum;			// ȯ�� �� ��ȣ
	int changeStationWeight;		// ���翪�� ȯ�¿����� �Ÿ� (����ġ)

	int arriveTime;					// �����ϴµ� �ɸ��� �ð�
};

struct Subway subway[NAMESIZE];

// �뼱 ��� �� ��� �뼱 ��� -----------------------------------------------------

void getSubwayNode(const char* filename)

{
	FILE *fp = fopen(filename, "r");					// ��� ���� �ؽ�Ʈ ����(read)
	char readData[NAMESIZE];							// ���Ͽ��� ���� �� ������ ���
	int index = 0;
	int size = 0;
	const int LASTSTATION = 45;
	const int FIRSTSTATION = 21;

	if (fp != NULL) {
		init_graph();
		fscanf(fp, "%s", readData);
		size = atoi(readData);

		for (int i = 0; i < size; i++) {
			// ���翪 ȣ��
			fscanf(fp, "%s", readData);
			subway[index].stationNum = atoi(readData);

			// ȯ�¿� �Ǻ�
			fscanf(fp, "%s", readData);
			subway[index].isChangeStation = atoi(readData);

			// ���翪 ��ȣ
			fscanf(fp, "%s", readData);
			subway[index].currentNum = atoi(readData);
			insert_vertex(readData);

			// ������ ��ȣ
			fscanf(fp, "%s", readData);
			subway[index].nextNum = atoi(readData);

			// ������ �Ÿ� (����ġ)
			fscanf(fp, "%s", readData);
			subway[index].weight = atoi(readData);

			// ȯ�¿��̶��
			if (subway[index].isChangeStation) {
				// ȯ�¿� ȣ��
				fscanf(fp, "%s", readData);
				subway[index].changeStationStationNum = atoi(readData);

				// ȯ�¿� ��ȣ
				fscanf(fp, "%s", readData);
				subway[index].changeStationNum = atoi(readData);

				// ȯ�¿� ����ġ
				fscanf(fp, "%s", readData);
				subway[index].changeStationWeight = atoi(readData);
			}

			// ���翪 �̸�
			fscanf(fp, "%s", readData);
			for (int k = 0; k < strlen(readData); k++)
				subway[index].stationName[k] = readData[k];

			// ������ �̸�
			fscanf(fp, "%s", readData);
			for (int i = 0; i < strlen(readData); i++)
				subway[index].nextStationName[i] = readData[i];

			//���� �� ������ ����ϱ����� �ε����� ����
			index++;
		}
		fclose(fp);										// ��� ���� �ؽ�Ʈ ���� �ݱ�
	}
	
	// �Ǻ�
	for (int i = 0; i < vsize; i++) {
		for (int j = 1; j < vsize; j++) {
			if (subway[i].nextNum == 1)											// ��ȯ ȣ���� ���
				insert_edge2(i, 0, subway[i].weight);

			
			if (subway[i].stationNum == subway[j].stationNum) {					// ȣ���� ����
				if (subway[i].currentNum == subway[j].currentNum - 1)			// ���翪�� ������ ��ȣ�� ������ ��ȣ��� -> ���翪�� �������� ���� �Ǿ��ִٸ�
					insert_edge2(i, j, subway[i].weight);						// �Ÿ�(����ġ)�� �Է�

				if (subway[i].currentNum == LASTSTATION && subway[j].currentNum == FIRSTSTATION)
					insert_edge2(i, j, subway[i].weight);
			}

			else																// ȣ���� ���� �Ȱ�
			{
				if(subway[i].changeStationStationNum == subway[j].stationNum)	// ���翪�� ����� ȯ�¿��� ȣ���� ȯ�¿��� ȣ���� ���ٸ�
					if (subway[i].changeStationNum == subway[j].currentNum)		// ���翪�� ȯ�¿� ��ȣ�� ȯ�¿� ��ȣ���
						insert_edge2(i, j, subway[i].changeStationWeight);		// �Ÿ�(����ġ)�� �Է�
			}
		}
	}
}

void saveSubwayNode()
{
	FILE * fp = fopen("subwayWeightGraph.txt", "w");	// ����ġ ���� �ؽ�Ʈ ����(write)

	// ���� �� �̸� �� ����ġ ��
	for (int i = 0; i < vsize; i++) {
		fputs(subway[i].stationName, fp);
		fputs("\t", fp);

		// �Էµ� ����ġ ���
		for (int j = 0; j < vsize; j++) {			
			// ��°� ���ÿ� ����ġ �ؽ�Ʈ ���Ͽ� ����ġ�� ����
			char str[10];
			itoa(adj[i][j], str, 10);
			fputs(str, fp);
			fputs(" ", fp);  // ���� ���� �ϱ� ���� ���� �Է�
		}
		fputs("\n", fp);	 // ���� ���� �ϱ� ���� �� �ٲ�
	}

	fclose(fp);											// ����ġ ���� �ؽ�Ʈ ���� �ݱ�
}

// ����ġ �׷��� �ε� --------------------------------------------------------------

void load_wgraph(char *filename)
{
	FILE *fp = fopen(filename, "r");					// ����ġ ���� �ؽ�Ʈ ����(read)
	char readData[NAMESIZE];							// ���Ͽ��� ���� �� ������ ���

	if (fp != NULL) {
		for (int i = 0; i < vsize; i++) {
			fscanf(fp, "%s", readData);					// �� �̸� ������ �����ʰ� ��¸� ��

			for (int j = 0; j < vsize; j++) {
				fscanf(fp, "%s", readData);
				if (i != j && atoi(readData) == 0)
					adj[i][j] = INF;
				else
					adj[i][j] = atoi(readData);
			}
		}
	}

	fclose(fp);											// ����ġ ���� �ؽ�Ʈ �ݱ�

	// printf("weight graph sucessfully loaded.\n\n");
}

void print_wgraph()
{
	int weight;	// ���� ���� ���� ������ �Ÿ� (����ġ)

	printf("weight graph printed!\n\n \t\t  ");

	for (int i = 1; i <= vsize; i++) 
		printf("%3d ", i);
	printf("\n");
		
	for (int i = 0; i < vsize; i++) {
		printf("%s\t", subway[i].stationName);
		// ���ڿ� ���� ������ ����
		if (strlen(subway[i].stationName) < 8)
			printf("\t  ");

		else
			printf("  ");

		for (int j = 0; j < vsize; j++) {
			weight = adj[i][j];
			if (i == j) printf(" 0 ");
			else if (weight >= INF) printf(" - ");
			else printf("%2d ", weight);
		}
		printf("\n");
	}
}

// ���ͽ�Ʈ�� �˰��� -------------------------------------------------------------

int path[MAX_VTXS];
int dist[MAX_VTXS];
int found[MAX_VTXS];

void print_step(int step)				// ���� �ܰ躰 ��Ȳ��� 
{
	int i;
	printf(" Step%2d:", step);
	for (i = 0; i<vsize; i++)
		if (dist[i] == INF) printf("  x");
		else printf("%3d", dist[i]);
		printf("\n");
}

void print_shortest_path(int start, int end)	// ���� ��� ��� 
{
	printf("%s", subway[end].stationName);
	while (path[end] != start) {
		printf(" - %s", subway[path[end]].stationName);
		end = path[end];
	}
	printf(" - %s\n\n", subway[path[end]].stationName);
}

int choose_vertex()
{
	int i, min = INF, minpos = -1;

	for (i = 0; i<vsize; i++)
		if (dist[i]< min && !found[i]) {
			min = dist[i];
			minpos = i;
		}
	return minpos;
}

void shortest_path_dijkstra(int start)
{
	int i, u, w;
	for (i = 0; i<vsize; i++) {
		dist[i] = adj[start][i];
		path[i] = start;
		found[i] = 0;
	}

	found[start] = 1;
	dist[start] = 0;

	for (i = 0; i<vsize; i++) {
		//print_step(i + 1);			// �����Ȳ ��¿� 
		u = choose_vertex();
		found[u] = 1;

		for (w = 0; w<vsize; w++) {
			if (found[w] == 0) {
				if (dist[u] + adj[u][w] < dist[w]) {
					dist[w] = dist[u] + adj[u][w];
					path[w] = u;
				}
			}
		}
	}
}


// �� ------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#define MAX_HEAP_NODE	200


typedef int HNode;
#define Key(n)   (n) 

HNode heap[MAX_HEAP_NODE];
int heap_size;

#define Parent(i) (heap[i/2])		// i�� �θ� ��� 
#define Left(i) (heap[i*2])		// i�� ���� �ڽ� ��� 
#define Right(i) (heap[i*2+1])	// i�� ������ �ڽ� ��� 

void init_heap() { heap_size = 0; }
int is_empty_heap() { return heap_size == 0; }
int is_full_heap() { return (heap_size == MAX_HEAP_NODE - 1); }
HNode find_heap() { return heap[1]; }

void insert_heap(HNode n)
{
	int i;
	if (is_full_heap()) return;
	i = ++(heap_size);
	while (i != 1 && Key(n) > Key(Parent(i))) {
		heap[i] = Parent(i);
		i /= 2;
	}
	heap[i] = n;
}

HNode delete_heap()
{
	HNode hroot, last;
	int parent = 1, child = 2;

	if (is_empty_heap() != 0)
		error("�� Ʈ�� ���� ����");

	hroot = heap[1];
	last = heap[heap_size--];
	while (child <= heap_size) {
		if (child<heap_size && Key(Left(parent))<Key(Right(parent)))
			child++;
		if (Key(last) >= Key(heap[child]))
			break;
		heap[parent] = heap[child];
		parent = child;
		child *= 2;
	}
	heap[parent] = last;
	return hroot;
}

void print_heap()
{
	int i, level;
	for (i = 1, level = 1; i <= heap_size; i++) {
		if (i == level) {
			printf("\n");
			level *= 2;
		}
		printf("%2d ", Key(heap[i]));
	}
	printf("\n-----------");  
}

void print_array(int a[], int n, char* msg)
{
	int i;
	printf("%10s : ", msg);
	for (int i = 0; i < n; i++)
		printf("%3d ", a[i]);
	printf("\n");
}


// ��� �Լ� -----------------------------------------------------------------------

int menu = 0;					// �޴� ��ȣ
int goBackToMenu = 0;			// bool ��, true �� �ʱ� ȭ������ ���ư�

void selectMenu()
{
	printf("����ö ��ã�� ���α׷�\n");
	printf("====================================================\n");
	printf("1. ��� �뼱 ���\n");
	printf("2. �� ã��\n");
	printf("3. �ð� �� ���� ���ɿ� �˻�\n");
	printf("4. �����ϱ�\n");
	printf("5. ����\n");
	printf("====================================================\n");
	printf("�޴��� �����ϼ��� : ");
	scanf("%d", &menu);
	system("cls");
}

void checkMenu()
{
	printf("\n====================================================\n");
	printf("����Ϸ��� RŰ�� ��������.\n");
	printf("�������� ���ư����� BŰ�� ��������.\n");

	while (1) {
		if (_kbhit()) {
			if (getch() == 'r') {
				system("cls");
				break;
			}
			if (getch() == 'b') {
				goBackToMenu = 1;
				system("cls");
				break;
			}
		}
	}
}

// ���� �Լ� -----------------------------------------------------------------------

int main()
{
	system("mode con cols=170 lines=40");	// �ܼ�â ũ�� ����

	char currentStationName[NAMESIZE];		// ���� �� �̸�
	char nextStationName[NAMESIZE];			// ���� �� �̸�
	
	int currentStationNum = 0;				// ���� �� ��ȣ
	int nextStationNum = 0;					// ���� �� ��ȣ

	getSubwayNode("subway.txt");
	saveSubwayNode();

	load_wgraph("subwayWeightGraph.txt");
	//print_wgraph("����ġ �׷���");


	while (1) {
		selectMenu();

		switch (menu) {

		case 1:
		#pragma region printData
			goBackToMenu = 0;
			// code here
			do {
				int stationNumber;
				int input;

				printf("��� �뼱 ���\n");
				printf("====================================================\n");
				printf("ȣ���� �Է��ϼ��� : ");
				scanf("%d", &stationNumber);

				system("cls");

				printf("%d ȣ��\n", stationNumber);
				printf("====================================================\n");
				printf("1. ��� ����ö ���\n");
				printf("2. ȯ�¿� ���\n");
				printf("3. �������� ���ư���\n");
				printf("====================================================\n");
				printf("�Է� : ");
				scanf("%d", &input);


				int STATIONSTARTLINE[6] = { 0, 39, 81, 124, 170, 213 };
				int STATIONENDLINE[6] = { 39, 81, 124, 170, 213, 251 };

				if (input == 1)
				{
					printf("\n");
					for (int i = STATIONSTARTLINE[stationNumber-1]; i < STATIONENDLINE[stationNumber - 1]; i++)
						printf("%s ", subway[i].stationName);
					printf("\n");
					printf("����Ͻ÷��� r �� ��������");

					if (getch() == 'r') {
						system("cls");
						continue;
					}
				}

				else if (input == 2) {
					printf("\n");
					for (int i = STATIONSTARTLINE[stationNumber - 1]; i < STATIONENDLINE[stationNumber - 1]; i++)
						if (subway[i].isChangeStation)
							printf("%s ", subway[i].stationName);
					printf("\n\n");
					printf("����Ͻ÷��� r �� ��������");

					if (getch() == 'r') {
						system("cls");
						continue;
					}
				}

				else if (input == 3)
					system("cls");
					break;

				checkMenu();
			} while (goBackToMenu != 1);
			break;
		#pragma endregion

		case 2:
		#pragma region pathFinding
			goBackToMenu = 0;
			do
			{
				printf("�� ã��\n");
				printf("====================================================\n\n");
				printf("���� ���� �Է��ϼ��� : ");
				scanf("%s", &currentStationName);
				printf("���� ���� �Է��ϼ��� : ");
				scanf("%s", &nextStationName);
				printf("====================================================\n\n");

				for (int i = 0; i < vsize; i++) {
					if (!strcmp(currentStationName, subway[i].stationName))
					{
						currentStationNum = subway[i].currentNum;
					}

					if (!strcmp(nextStationName, subway[i].stationName))
					{
						nextStationNum = subway[i].currentNum;
					}
				}

				printf("%s ���� %s ������ �ִܰŸ�\n\n", subway[currentStationNum - 1].stationName, subway[nextStationNum - 1].stationName);

				shortest_path_dijkstra(nextStationNum - 1);	// ���ۿ� 0���� �ҿ��
				print_shortest_path(nextStationNum - 1, currentStationNum - 1);	// 0��(�ҿ��)���� 23��(����) ���� �ִ� �Ÿ� ���ϱ�

				printf("�ҿ� �ð� : 40 min \n\n");
				printf("���� ���� �ð� : 2�� 40�� \n\n");
				checkMenu();
			} while (goBackToMenu != 1);
			break;
		#pragma endregion

		case 3:
		#pragma region searchIsPossible
			goBackToMenu = 0;
			do {
				int haveTime;

				printf("�ð� �� �� �� �ִ� �� ã��\n");
				printf("====================================================\n\n");
				printf("���� ���� �Է��ϼ��� : ");
				scanf("%s", &currentStationName);
				printf("\n");

				printf("�ð��� �Է��ϼ��� (�д���) :");
				scanf("%d", &haveTime);
				printf("\n");

				printf("%s ���� %d�� ���� �� �� �ִ� ���� �˻��մϴ�\n\n", &currentStationName, haveTime);

				int leadTime = 0;
				int searchIndex = 0;
				int checkPoint = 1;
				init_heap();

				while (1) {
					if (!strcmp(currentStationName, subway[searchIndex].stationName))
					{
						int currentStationIndex = searchIndex;

						while (leadTime < haveTime) {
							leadTime += subway[currentStationIndex].weight;

							if (currentStationIndex - checkPoint > 0) {
								subway[currentStationIndex - checkPoint].arriveTime = leadTime;
								insert_heap(currentStationIndex - checkPoint);
							}

							if (currentStationIndex + checkPoint < vsize) {
								subway[currentStationIndex + checkPoint].arriveTime = leadTime;
								insert_heap(currentStationIndex + checkPoint);
							}

							checkPoint++;
						}
						break;

						printf("\n\n");
					}
					searchIndex++;
				}


				int dist[1000];
				int distIndex = 0;

				while (!is_empty_heap()) {
					int index = Key(delete_heap());

					dist[distIndex++] = subway[index].arriveTime;
				}

				init_heap();
				for (int i = 0; i < distIndex; i++)
					insert_heap(dist[i]);

				int maxSize = vsize;

				for (int i = 1; i < distIndex; i++)
				{
					int leadTime = Key(delete_heap());
					if (searchIndex + i < maxSize) {
						printf("[ %s ] ���� [ %s ] ������ %d �� �ҿ� �˴ϴ�. \n"
							, subway[searchIndex].stationName, subway[searchIndex + i].stationName, leadTime);
					}

					if (searchIndex - i > 0) {
						printf("[ %s ] ���� [ %s ] ������ %d �� �ҿ� �˴ϴ�. \n"
							, subway[searchIndex].stationName, subway[searchIndex - i].stationName, leadTime);
					}
				}

				checkMenu();
			} while (goBackToMenu != 1);
			break;
		#pragma endregion

		case 4:
		#pragma region edit
			goBackToMenu = 0;
			// code here

			break;
		#pragma endregion

		case 5:
		#pragma region exit
			exit(0);
			break;
		#pragma endregion
		}
	}
}
// ---------------------------------------------------------------------------------