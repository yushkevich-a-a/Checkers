#pragma once

struct move_pos {
    int x, y; // from
    int x2, y2; // to
    int xb = -1, yb = -1; // beaten
    
    move_pos(int x, int y, int x2, int y2): x(x), y(y), x2(x2), y2(y2) {}
    move_pos(int x, int y, int x2, int y2, int xb, int yb): x(x), y(y), x2(x2), y2(y2), xb(xb), yb(yb) {}
    bool operator == (const move_pos& other) const {
        return (x == other.x && y == other.y && x2 == other.x2 && y2 == other.y2);
    }
    bool operator != (const move_pos& other) const {
        return !(*this == other);
    }
};
