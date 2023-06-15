#include <Novice.h>

#include <vector>
#include <queue>
#include <iostream>
#include <cmath>

#include "imgui.h"

const char kWindowTitle[] = "LE2B_02_イソガイユウト_ASTAR";

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;

enum Map {
	EMPTY,
	WALL,
	START,
	GOAL
};

int map[MAP_HEIGHT][MAP_WIDTH] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 1, 1, 1, 0, 1, 0, 0},
		{0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
		{0, 1, 1, 1, 0, 1, 0, 1, 0, 0},
		{0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
		{0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 1, 0, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// ノード
struct Node {
	int x;
	int y;
	int g;
	int h;
	int f;

	Node* parent;

	Node(int x_, int y_, int g_, int h_, Node* parent_ = nullptr)
		: x(x_), y(y_), g(g_), h(h_), f(g_ + h_), parent(parent_) {}
};

// ノードの比較
struct CompareNode {
	bool operator()(const Node* a, const Node* b) {
		return a->f > b->f;
	}
};

// 座標が有効かのチェック
bool IsValidCoordinate(int x, int y) {
	return x >= 0 && x < MAP_WIDTH&& y >= 0 && y < MAP_HEIGHT;
}

// ノードがリストにあるかどうか
bool IsNodeInList(int x, int y, const std::vector<Node*>& nodeList) {
	for (const auto& node : nodeList) {
		if (node->x == x && node->y == y)
			return true;
	}
	return false;
}

// 経路探索関数（ASTAR）
std::vector<Node*> AstarSearch(const Node& start, const Node& goal) {
	std::priority_queue<Node*, std::vector<Node*>, CompareNode>openList;
	std::vector<Node*>closedList;

	openList.push(new Node(start.x, start.y, 0, 0));

	while (!openList.empty()) {
		Node* current = openList.top();
		openList.pop();

		if (current->x == goal.x && current->y == goal.y) {
			std::vector<Node*> path;
			while (current != nullptr) {
				path.push_back(current);
				current = current->parent;
			}
			std::reverse(path.begin(), path.end());
			return path;
		}


		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx == 0 && dy == 0)
					continue;

				int newX = current->x + dx;
				int newY = current->y + dy;

				if (!IsValidCoordinate(newX, newY))
					continue;

				if (map[newY][newX] == WALL || IsNodeInList(newX, newY, closedList))
					continue;

				int newG = current->g + 1;
				int newH = std::abs(newX - goal.x) + std::abs(newY - goal.y);

				Node* newNode = new Node(newX, newY, newG, newH, current);
				openList.push(newNode);
			}
		}

		closedList.push_back(current);
	}

	// ゴールに到達できなかった場合、空の経路を返す
	return std::vector<Node*>();
}


// 色付け用
unsigned int GetColor(int red, int green, int blue, int alpha) {
	unsigned int hex = 0x00000000;
	red = red << 24;
	green = green << 16;
	blue = blue << 8;
	alpha = alpha << 0;

	return hex = red + green + blue + alpha;
}

// マップ描画
void DrawMap() {
	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			int chip = map[y][x];
			int color = 0;

			switch (chip) {
			case EMPTY:
				color = GetColor(255, 255, 255, 255);
				break;
			case WALL:
				color = GetColor(0, 0, 0, 255);
				break;
			case START:
				color = GetColor(0, 255, 0, 255);  
				break;
			case GOAL:
				color = GetColor(255, 0, 0, 255); 
				break;
			}
			Novice::DrawBox(x * 32, y * 32, x * 32 + 32, y * 32 + 32, 0.0, color, kFillModeSolid);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, MAP_WIDTH * 64, MAP_HEIGHT * 32);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Node start{ 0,0,0,0 };
	Node goal{ 6,7,0,0 }; // 6,7 // 3,6 // 26 // 9,9

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		std::vector<Node*> path;

		///
		/// ↓更新処理ここから
		///

		path = AstarSearch(start, goal);

		ImGui::Begin("Node Edit");
		ImGui::SliderInt2("Start Node", &start.x, 0, 10);
		ImGui::SliderInt2("Goal Node", &goal.x, 0, 10);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawMap();

		for (const auto& node : path) {
			Novice::DrawEllipse(node->x * 32 + 16, node->y * 32 + 16, 16, 16, 0.0, BLUE, kFillModeSolid);
		}

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
