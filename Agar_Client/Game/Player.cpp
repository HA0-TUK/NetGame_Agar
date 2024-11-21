#include "Player.h"


void Player::addCell(Cell* cell) {
    cells.push_back(cell);
}

void Player::clearCells() {
    // ������Ʈ ����Ʈ�� ���ҿ� ���� ������ �̹Ƿ� delete���ʿ�
    cells.clear();
}


void Player::draw(const HDC& hdc, const Map& map, const RECT& valid_area) const {
    for(auto e : cells) {
        e->draw(hdc, map, valid_area);
    }
}


Point Player::getCenterPoint() const {
    Point center { 0, 0 };
    for(auto e : cells) {
        center.x += e->position.x;
        center.y += e->position.y;
    }
    center.x /= cells.size();
    center.y /= cells.size();
    return center;
}

double Player::getRadius() const {
    Point p = getCenterPoint();
    double max = 0;
    for(auto e : cells) {
        if(e->getRadius() > max) {
            max = e->getRadius();
        }
    }
    for(auto e : cells) {
        if((e->position - p).scalar() + e->getRadius() > max) {
            max = (e->position - p).scalar() + e->getRadius();
        }
    }
    return max;
}

double Player::getSize() const {
    double result = 0;
    for(auto e : cells) {
        result += pow(e->getRadius(), 2);
    }
    return result;
}
