#include <stdio.h>
#include <string.h>
#include "stategrid.h"

char* format_action (int a)
{
  switch (a) {
    case ACTION_N:
      return "north";
    case ACTION_S:
      return "south";
    case ACTION_E:
      return "east";
    case ACTION_W:
      return "west";
  }

  return "unknown";
}

int main (int argc, char** argv)
{
  int c;
  state_grid_t* sg;
  agent_t ag;
  int s;
  int a, aprime;
  double reward;

  fprintf (stdout, "Q-LEARNING\n");

  srand (time (NULL));

  sg = state_grid_new (12, 4, 0.1, 1, 0.1);
  state_grid_init (sg);

  for (c=0; c<4000; c++) {
    ag.s = ag.r = 0;
    reward = 0;
    s = sg->start;

    while (ag.s != sg->end) {
      a = state_grid_get_best_action (sg, s);

      state_grid_take_action (sg, &ag, s, a);
      aprime = state_grid_get_best_action (sg, ag.s);

      state_grid_update_cost (sg, s, a, ag.r, ag.s, aprime);

      s = ag.s;

      reward += ag.r;
    }
  }

  fprintf (stdout, "path:\n");

  ag.s = ag.r = 0;
  reward = 0;
  s = sg->start;
  while (ag.s != sg->end) {
    state_grid_display (sg, s);
    getchar ();

    a = state_grid_get_best_action (sg, s);

    state_grid_take_action (sg, &ag, s, a);
    aprime = state_grid_get_best_action (sg, ag.s);

    state_grid_update_cost (sg, s, a, ag.r, ag.s, aprime);

    s = ag.s;

    reward += ag.r;
  }

  fprintf (stdout, "reward : %.2f\n", reward);

  state_grid_free (sg);

  return 0;
}
