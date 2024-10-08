#ifndef SOLVER_H
#define SOLVER_H

void solver(struct grid g, struct word *w);

int search(int x, int y, int addx, int addy, struct grid g, struct word *w);

#endif
