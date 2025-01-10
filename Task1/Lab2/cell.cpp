#include "cell.h"
#include <ranges>
#include <algorithm>

HFONT createFont(int size);

Cell::Cell() : width(100), height(50)
{ 
	hFont = createFont(fontSize);
}

void Cell::setSize(const pair<float, float>& size)
{
	width = size.first;
	height = size.second;
	needReset = true;
}

pair<float, float> Cell::getSize() const
{
	return {width, height};
}

void Cell::setPos(const pair<float, float>& pos){
	x = pos.first;
	y = pos.second;
}

pair<float, float> Cell::getPos() const {
	return {x, y};
}

void Cell::setColor(COLORREF _color) {
	color = _color;
}

void Cell::drawRect(HDC hdc) const {
	HBRUSH hBrush = CreateSolidBrush(color);
	SelectObject(hdc, hBrush);
	Rectangle(hdc, x, y, x + width, y + height);
	DeleteObject(hBrush);
}

void Cell::drawText(HDC hdc) {	
	if (!data.empty()) {
		int currSize = 80;//fontSize;
		HFONT hFont = nullptr;
		RECT rect{ x + 10, y + 10, x + width - 10, y + height - 10};
		while (currSize > 10) {
			hFont = createFont(currSize);

			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

			RECT calcRect = rect;
			DrawTextW(hdc, data.c_str(), -1, &calcRect, DT_WORDBREAK | DT_CALCRECT);

			if (calcRect.right - calcRect.left <= rect.right - rect.left &&
				calcRect.bottom - calcRect.top <= rect.bottom - rect.top) {
				SelectObject(hdc, hOldFont);
				break;
			}
			SelectObject(hdc, hOldFont);
			DeleteObject(hFont);
			currSize -= 2;
		}

		if (currSize > 10) {
			hFont = CreateFontW(currSize, 0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
			DrawTextW(hdc, data.c_str(), -1, &rect, DT_WORDBREAK);
			SelectObject(hdc, hOldFont);
			DeleteObject(hFont);
		}
	}
}


HFONT createFont(int size) {
	return CreateFont(
		size,                
		0,                       
		0,                
		0,               
		FW_BOLD ,             
		TRUE,              
		FALSE,                 
		FALSE,                   
		DEFAULT_CHARSET,          
		OUT_DEFAULT_PRECIS,     
		CLIP_DEFAULT_PRECIS,     
		DEFAULT_QUALITY,     
		DEFAULT_PITCH | FF_SWISS, 
		L"Arial");
}



Matrix::Matrix():rows(5),cols(5) {}

void Matrix::setSize(const pair<float, float>& size){
	float k1 = width == 0 ? 1 : float(size.first) / width;
	float k2 = height == 0 ? 1 : float(size.second) / height;
	float _width = k1 * width / cols;
	float _height = k2 * height / rows;

	int h = 0;
	std::ranges::for_each(data, [&](auto& row) {
		int w = 0;
		std::ranges::for_each(row, [&](auto& elem) {
			elem.setSize({_width, _height});
			elem.setPos({ x + w, y + h });
			w += _width;
			});
		h += _height;
		});
	width = size.first;
	height = size.second;
}

pair<float, float> Matrix::getSize() const {
	return {width, height};
}

void Matrix::setRowsCols(const int _rows, const int _cols) {
	rows = _rows;
	cols = _cols;
	amount = rows * cols;

	data.resize(rows);

	std::ranges::for_each(data, [&](auto& row) {
		row.resize(cols);
		});

	fillEqualSizes();
}

pair<int, int> Matrix::getRowsCols() const {
	return {rows, cols};
}

void Matrix::setPos(const pair<float, float>& pos) {
	int dx = pos.first - x;
	int dy = pos.second - y;
	x += dx;
	y += dy;

	std::ranges::for_each(data, [&](auto& row) {
		std::ranges::for_each(row, [&dx, &dy](Cell& elem) {
			auto [cx, cy] = elem.getPos();
			elem.setPos({ cx + dx, cy + dy });
		});
	});
}

pair<float, float> Matrix::getPos() const {
	return {x, y};
}


void Matrix::draw(HDC& hdc) {
	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(14, 14, 14));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	std::ranges::for_each(data, [&hdc](auto& row) {
		std::ranges::for_each(row, [&hdc](auto& elem) {
			elem.drawRect(hdc);
		});
	});

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	SetBkMode(hdc, TRANSPARENT);
	std::ranges::for_each(data, [&hdc](auto& row) {
		std::ranges::for_each(row, [&hdc](auto& elem) {
			elem.drawText(hdc);
			});
		});
}

void Matrix::fillWithSentences(const vector<wstring>& sentences)
{
		int sentenceIndex = 0;
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < cols; ++col) {
				if (sentenceIndex < sentences.size()) {
					data[row][col].setData(sentences[sentenceIndex]); 
					sentenceIndex++;
				}
				else {
					data[row][col].setData(L""); 
				}
			}
		}
	}


void Matrix::fillEqualSizes() {
	if (rows == 0 || cols == 0) return;

	int _width = width / cols;
	int _height = height / rows;
	
	int row_index = 0;
	std::ranges::for_each(data, [&](auto& row) {
		int col_index = 0;
		std::ranges::for_each(row, [&](auto& elem) {
			elem.setSize({ _width, _height });
			elem.setPos({ x + col_index * _width, y + row_index * _height });
			++col_index;
			});
		++row_index;
		});
}

