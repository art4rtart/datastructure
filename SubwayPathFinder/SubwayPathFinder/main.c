#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "main.h"

#define MAX_VTXS 1000
#define INF 9999
#define NAMESIZE 100

// 기존 코드 -----------------------------------------------------------------------

#pragma region basic code
typedef char VtxData;			// 그래프 정점에 저장할 데이터의 자료형 
int adj[MAX_VTXS][MAX_VTXS];	// 인접 행렬 
//int myarr[MAX_VTXS][MAX_VTXS];	// 인접 행렬 
int vsize;						// 전체 정점의 개수					
VtxData vdata[MAX_VTXS];		// 정점에 저장할 데이터 배열

void error(char str[]) { printf("%s\n", str);	exit(1); }
int is_empty_graph() { return (vsize == 0); }
int is_full_graph() { return (vsize >= MAX_VTXS); }

// 그래프 초기화
void init_graph()
{
	int i, j;
	vsize = 0;
	for (i = 0; i<MAX_VTXS; i++)
		for (j = 0; j<MAX_VTXS; j++)
			adj[i][j] = 0;
}

// 그래프 정점 입력
void insert_vertex(char* name)
{
	if (is_full_graph())
		error("Error: 그래프 정점의 개수 초과\n");
	else
		vdata[vsize++] = name;
}

// 그래프 간선 추가
void insert_edge(int u, int v, int val)
{
	adj[u][v] = val;
}

// 그래프 간선 추가 2
void insert_edge2(int u, int v, int val)
{
	adj[u][v] = adj[v][u] = val;
}
#pragma endregion

// 사용할 자료구조 -----------------------------------------------------------------

struct Subway
{
	char stationName[NAMESIZE];			// 현재 역 이름
	char nextStationName[NAMESIZE];		// 다음 역 이름

	int stationNum;				// 현재 역 호선

	int currentNum;					// 현재 역 번호
	int nextNum;					// 다음 역 번호

	int weight;						// 현재역과 다음역간의 거리	(가중치)

	int isChangeStation;			// 환승역인지 구분
	int changeStationStationNum;	// 환승 역 호선 번호
	int changeStationNum;			// 환승 역 번호
	int changeStationWeight;		// 현재역과 환승역간의 거리 (가중치)
};

struct Subway subway[NAMESIZE];

// 노선 등록 및 등록 노선 출력 -----------------------------------------------------

void getSubwayNode(const char* filename)

{
	FILE *fp = fopen(filename, "r");					// 노드 저장 텍스트 열기(read)
	char readData[NAMESIZE];							// 파일에서 읽은 값 저장할 장소
	int index = 0;
	int size = 0;
	const int LASTSTATION = 45;
	const int FIRSTSTATION = 21;

	if (fp != NULL) {
		init_graph();
		fscanf(fp, "%s", readData);
		size = atoi(readData);

		for (int i = 0; i < size; i++) {
			// 현재역 호선
			fscanf(fp, "%s", readData);
			subway[index].stationNum = atoi(readData);

			// 환승역 판별
			fscanf(fp, "%s", readData);
			subway[index].isChangeStation = atoi(readData);

			// 현재역 번호
			fscanf(fp, "%s", readData);
			subway[index].currentNum = atoi(readData);
			insert_vertex(readData);

			// 다음역 번호
			fscanf(fp, "%s", readData);
			subway[index].nextNum = atoi(readData);

			// 역간의 거리 (가중치)
			fscanf(fp, "%s", readData);
			subway[index].weight = atoi(readData);

			// 환승역이라면
			if (subway[index].isChangeStation) {
				// 환승역 호선
				fscanf(fp, "%s", readData);
				subway[index].changeStationStationNum = atoi(readData);

				// 환승역 번호
				fscanf(fp, "%s", readData);
				subway[index].changeStationNum = atoi(readData);

				// 환승역 가중치
				fscanf(fp, "%s", readData);
				subway[index].changeStationWeight = atoi(readData);
			}

			// 현재역 이름
			fscanf(fp, "%s", readData);
			for (int k = 0; k < strlen(readData); k++)
				subway[index].stationName[k] = readData[k];

			// 다음역 이름
			fscanf(fp, "%s", readData);
			for (int i = 0; i < strlen(readData); i++)
				subway[index].nextStationName[i] = readData[i];

			//다음 역 정보를 등록하기위해 인덱스를 더함
			index++;
		}
		fclose(fp);										// 노드 저장 텍스트 파일 닫기
	}
	
	// 판별
	for (int i = 0; i < vsize; i++) {
		for (int j = 1; j < vsize; j++) {
			if (subway[i].nextNum == 1)											// 순환 호선일 경우
				insert_edge2(i, 0, subway[i].weight);

			
			if (subway[i].stationNum == subway[j].stationNum) {					// 호선이 같고
				if (subway[i].currentNum == subway[j].currentNum - 1)			// 현재역의 다음역 번호가 다음역 번호라면 -> 현재역과 다음역이 연결 되어있다면
					insert_edge2(i, j, subway[i].weight);						// 거리(가중치)를 입력

				if (subway[i].currentNum == LASTSTATION && subway[j].currentNum == FIRSTSTATION)
					insert_edge2(i, j, subway[i].weight);
			}

			else																// 호선이 같지 안고
			{
				if(subway[i].changeStationStationNum == subway[j].stationNum)	// 현재역과 연결된 환승역의 호선이 환승역의 호선과 같다면 -> 나중에 설명 드리겠습니다. 
					if (subway[i].changeStationNum == subway[j].currentNum)		// 현재역의 환승역 번호가 환승역 번호라면	-> 제가 적었지만 저도 뭔소린지 모르겠습니다.
						insert_edge2(i, j, subway[i].changeStationWeight);		// 거리(가중치)를 입력
			}
		}
	}
}

void saveSubwayNode()
{
	FILE * fp = fopen("subwayWeightGraph.txt", "w");	// 가중치 저장 텍스트 쓰기(write)

	// 세로 역 이름 및 가중치 값
	for (int i = 0; i < vsize; i++) {
		fputs(subway[i].stationName, fp);
		fputs("\t", fp);

		// 입력된 가중치 출력
		for (int j = 0; j < vsize; j++) {			
			// 출력과 동시에 가중치 텍스트 파일에 가중치를 저장
			char str[10];
			itoa(adj[i][j], str, 10);
			fputs(str, fp);
			fputs(" ", fp);  // 보기 좋게 하기 위해 공백 입력
		}
		fputs("\n", fp);	 // 보기 좋게 하기 위해 줄 바꿈
	}

	fclose(fp);											// 가중치 저장 텍스트 파일 닫기
}

// 가중치 그래프 로드 --------------------------------------------------------------

void load_wgraph(char *filename)
{
	FILE *fp = fopen(filename, "r");					// 가중치 저장 텍스트 열기(read)
	char readData[NAMESIZE];							// 파일에서 읽은 값 저장할 장소

	if (fp != NULL) {
		for (int i = 0; i < vsize; i++) {
			fscanf(fp, "%s", readData);					// 역 이름 저장은 하지않고 출력만 함

			for (int j = 0; j < vsize; j++) {
				fscanf(fp, "%s", readData);
				if (i != j && atoi(readData) == 0)
					adj[i][j] = INF;
				else
					adj[i][j] = atoi(readData);
			}
		}
	}

	fclose(fp);											// 가중치 저장 텍스트 닫기

	// printf("weight graph sucessfully loaded.\n\n");
}

void print_wgraph()
{
	int weight;	// 현재 역과 다음 역간의 거리 (가중치)

	printf("weight graph printed!\n\n \t\t  ");

	for (int i = 1; i <= vsize; i++) 
		printf("%3d ", i);
	printf("\n");
		
	for (int i = 0; i < vsize; i++) {
		printf("%s\t", subway[i].stationName);
		// 글자와 숫자 사이의 공백
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

// 다익스트라 알고리즘 -------------------------------------------------------------

int path[MAX_VTXS];
int dist[MAX_VTXS];
int found[MAX_VTXS];

void print_step(int step)				// 진행 단계별 상황출력 
{
	int i;
	printf(" Step%2d:", step);
	for (i = 0; i<vsize; i++)
		if (dist[i] == INF) printf("  x");
		else printf("%3d", dist[i]);
		printf("\n");
}

void print_shortest_path(int start, int end)	// 최종 경로 출력 
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
		//print_step(i + 1);			// 진행상황 출력용 
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

// 출력 함수 -----------------------------------------------------------------------

int menu = 0;					// 메뉴 번호
int goBackToMenu = 0;			// bool 값, true 면 초기 화면으로 돌아감

void selectMenu()
{
	printf("지하철 길찾기 프로그램\n");
	printf("====================================================\n");
	printf("1. 등록 노선 출력\n");
	printf("2. 길 찾기\n");
	printf("3. 시간 내 도착 가능역 검색\n");
	printf("4. 편집하기\n");
	printf("5. 종료\n");
	printf("====================================================\n");
	printf("메뉴를 선택하세요 : ");
	scanf("%d", &menu);
	system("cls");
}

// 메인 함수 -----------------------------------------------------------------------

int main()
{
	system("mode con cols=170 lines=40");	// 콘솔창 크기 조절

	char currentStationName[NAMESIZE];		// 현재 역 이름
	char nextStationName[NAMESIZE];			// 다음 역 이름
	
	int currentStationNum = 0;				// 현재 역 번호
	int nextStationNum = 0;					// 다음 역 번호

	getSubwayNode("subway.txt");
	saveSubwayNode();

	load_wgraph("subwayWeightGraph.txt");
	//print_wgraph("가중치 그래프");

	while (1) {
		selectMenu();

		switch (menu) {

		case 1:
		#pragma region printData
			goBackToMenu = 0;
			// code here

			break;
		#pragma endregion

		case 2:
		#pragma region pathFinding

			goBackToMenu = 0;
			do
			{
				printf("길 찾기\n");
				printf("====================================================\n\n");
				printf("현재 역을 입력하세요 : ");
				scanf("%s", &currentStationName);
				printf("도착 역을 입력하세요 : ");
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

				printf("%s 부터 %s 까지의 최단거리\n\n", subway[currentStationNum - 1].stationName, subway[nextStationNum - 1].stationName);

				shortest_path_dijkstra(nextStationNum - 1);	// 시작역 0번은 소요산
				print_shortest_path(nextStationNum - 1, currentStationNum - 1);	// 0번(소요산)부터 23번(합정) 까지 최단 거리 구하기

				printf("소요 시간 : 40 min \n\n");
				printf("도착 에정 시간 : 2시 40분 \n\n");
				printf("====================================================\n");
				printf("계속하려면 R키를 누르세요.\n");
				printf("이전으로 돌아가려면 B키를 누르세요.\n");

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
			} while (goBackToMenu != 1);
			break;
		#pragma endregion

		case 3:
		#pragma region searchIsPossible
			goBackToMenu = 0;
			// code here

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