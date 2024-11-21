#pragma once

#include "Cell.h"

#include <list>


class Player {
private:
    std::list<Cell*> cells;

public:
    void addCell(Cell* cell);
    void clearCells();

    void draw(const HDC& hdc, const Map& map, const RECT& valid_area) const;

    // �÷��̾� �� ������ �߽�
    Point getCenterPoint() const;
    // �÷��̾� �� ������ ������
    double getRadius() const;

    // �÷��̾� �� ������ �� ũ��
    double getSize() const;
};