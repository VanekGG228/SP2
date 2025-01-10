#pragma once
#include <string>
#include <utility>
#include <windows.h>
#include <vector>

using namespace std;

class Cell {
public:
	Cell();
	void setSize(const pair<float, float>& size);
	pair<float, float> getSize() const;
	void setPos(const pair<float, float>& pos);
	pair<float, float> getPos() const;
	void setColor(COLORREF color);
	void drawRect(HDC hdc) const;
	void drawText(HDC hdc);
	void setData(const wstring& sentence) {
		if (sentence.length() <= maxLen) { 
			data = sentence; 
		}
		else {
			data = sentence.substr(0, maxLen);
		}
	}
private:
	float x{}, y{}, height{100}, width{300};
	COLORREF color{RGB(255, 255, 255)};
	wstring data;
	vector<wstring> lines;
	bool needReset{ false };
	int fontSize { 40 };
	HFONT hFont;
	const int maxLen{ 50 };
};

class Matrix {
public:
	Matrix();
	void setSize(const pair<float, float>& size);
	pair<float, float> getSize() const;

	void setRowsCols(const int _rows, const int _cols);
	pair<int, int> getRowsCols() const;

	void setPos(const pair<float, float>& pos);
	pair<float, float> getPos() const;

	void draw(HDC& hdc);

	void fillWithSentences(const vector<wstring>& sentences);
private:
	void fillEqualSizes();

	float x{}, y{};
	int rows, cols;
	float width{}, height{};
	vector<vector<Cell>> data;
	int amount{};
};

