#define CRT_SECURE_NO_WARNINGS
#include <bangtal>
#include <time.h>
using namespace bangtal;

#include <cstdlib>
#include <ctime>
#include <Windows.h>

//지역 변수로 선언
ScenePtr scene;
ObjectPtr game_board[16], game_original[16];
ObjectPtr start;

//timer이용해서 섞기
TimerPtr timer;
float animationTime = 0.05f;
int mixCount = 5;

int blank;

clock_t start_t, end_t;



int game_index(ObjectPtr piece) {
	for (int i = 0; i < 16; i++)
		if (game_board[i] == piece) return i;
	return -1;			//invalid piece
}

int index_to_x(int index) {
	return 350 + 150 * (index % 4);
}

int index_to_y(int index) {
	return 535 - 150 * (index / 4);
}

//빈 칸과 오브젝트를 바꿔주는 함수
void game_move(int index) {
	//index <-> blank
	auto piece = game_board[index];
	game_board[index] = game_board[blank];
	game_board[index]->locate(scene, index_to_x(index), index_to_y(index));
	game_board[blank] = piece;
	game_board[blank]->locate(scene, index_to_x(blank), index_to_y(blank));
	blank = index;
}



bool check_move(int index) {
	//index가 blank 왼쪽에 == blank -1
	if (blank % 4 > 0 && index == blank - 1) return true;
	//index가 blank 오른쪽 == blank +1
	if (blank % 4 < 3 && index == blank + 1) return true;
	//index가 blank 위 == blank -4
	if (index == blank + 4) return true;
	//index가 blank 아래 == blank +4
	if (index == blank - 4) return true;

	return false;
}

int random_move() {
	int index = rand() % 16;
	while (!check_move(index)) {
		index = rand() % 16;
	}
	return index;
}

//start 버튼을 누르면 timer를 시작
void start_game() {
	mixCount = 5;

	timer->set(animationTime);
	timer->start();

	blank = 15;
	game_board[blank]->hide();

	start->hide();
}

bool check_end() {
	for (int i = 0; i < 16; i++) {
		if (game_board[i] != game_original[i])
			return false;
	}
	return true;
}

void end_game() {
	game_board[blank]->show();
	start->show();

}

void watch() {
	end_t = clock();
	double result;
	char buf[32];

	result = (double)(end_t - start_t);

	sprintf_s(buf, "Took %.2f seconds!!", (result) / CLOCKS_PER_SEC);
	showMessage(buf);
}

void init_game(){
	scene = Scene::create("퍼즐", "images/background.png");

	char path[20];
	for (int i = 0; i < 16; i++) {
		sprintf_s(path, "Images/%d.png", i + 1);
		game_board[i] = Object::create(path, scene, index_to_x(i), index_to_y(i));
		game_board[i]->setOnMouseCallback([&](auto piece, auto x, auto y, auto action)->bool {
			//piece로부터 index를 알아내는 함수
			int index = game_index(piece); // 클릭된 오브젝트가 게임 보드라는 arr에 몇번째에 있냐를 반환
			if (check_move(index)) {
				game_move(index);

				if (check_end()) {
					watch();
					end_game();
				}
			}

			return true;
			});
		game_original[i] = game_board[i]; //처음 보드를 game_original에 저장
	}

	start = Object::create("images/start.png", scene, 500, 200);
	//start버튼을 누르면 임의의 방향으로 blank가 움직임
	start->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
		start_game();
		start_t = clock();

		return true;
		});

	timer = Timer::create(animationTime);
	timer->setOnTimerCallback([&](auto)->bool {
		game_move(random_move());

		mixCount--;
		if (mixCount > 0) {
			timer->set(animationTime);
			timer->start();
		}

		return true;
	});




	startGame(scene);
}

int main()
{
	srand((unsigned int)time(NULL));

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	init_game();



	return 0;
}