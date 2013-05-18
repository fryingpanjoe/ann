#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stategrid.h"

state_grid_t* state_grid_new (unsigned w, unsigned h, double alpha,
                              double gamma, double epsilon)
{
  state_grid_t* sg = (state_grid_t*) malloc (sizeof (state_grid_t));
  if (sg == NULL)
    return NULL;

  sg->alpha = alpha;
  sg->gamma = gamma;
  sg->epsilon = epsilon;

  sg->w = w;
  sg->h = h;
  sg->grid = (state_t*) malloc (w * h * sizeof (state_t));
  if (sg->grid == NULL)
    return NULL;

  return sg;
}

void state_grid_free (state_grid_t* sg)
{
  if (sg == NULL)
    return;

  free ((void*) sg->grid);
  free ((void*) sg);
}

void state_grid_display (state_grid_t* sg, int s)
{
  int i, j, sprime;

  for (j=0; j<sg->h; j++) {
    for (i=0; i<sg->w; i++) {
      sprime = (i + j*sg->w);

      if (sprime == s)
        fprintf (stdout, "x");
      else {
        if (state_grid_get_type (sg, sprime) == TYPE_DEF)
          fprintf (stdout, "+");
        else if (state_grid_get_type (sg, sprime) == TYPE_CLIFF)
          fprintf (stdout, ".");
      }
    }
    fprintf (stdout, "\n");
  }
}

void state_grid_init (state_grid_t* sg)
{
  int i, j;

  fprintf (stdout, "initializing\n");

  for (i=0; i<sg->w; i++) {
    for (j=0; j<sg->h; j++) {
      sg->grid[i+j*sg->w].type = TYPE_DEF;
      memset ((void*) sg->grid[i+j*sg->w].actions, 1, sizeof (sg->grid[i+j*sg->w].actions));
      memset ((void*) sg->grid[i+j*sg->w].cost, 0, sizeof (sg->grid[i+j*sg->w].cost));
    }
  }

  fprintf (stdout, "setting possible actions (1)\n");

  for (i=0; i<sg->h; i++) {
    sg->grid[i*sg->w].actions[ACTION_W] = 0;
    sg->grid[i*sg->w+sg->w-1].actions[ACTION_E] = 0;
  }

  fprintf (stdout, "setting possible actions (2)\n");

  for (i=0; i<sg->w; i++) {
    sg->grid[i].actions[ACTION_N] = 0;
    sg->grid[i+(sg->h-1)*sg->w].actions[ACTION_S] = 0;
  }

  /* SPECIIAAAAAALL!!!! */

  fprintf (stdout, "setting grid types\n");

  for (i=1; i<sg->w-1; i++) {
    sg->grid[i+(sg->h-1)*sg->w].type = TYPE_CLIFF;
  }

  sg->start = (sg->h-1)*sg->w;
  sg->end = (sg->w-1) + (sg->h-1)*sg->w;
}

int state_grid_get_state (state_grid_t* sg, point_t* p)
{
  return p->x + p->y*sg->w;
}

point_t* state_grid_get_state_point (state_grid_t* sg, int s)
{
  static point_t p;

  p.x = s % sg->w;
  p.y = s / sg->w;

  return &p;
}

double state_grid_get_reward (state_grid_t* sg, int s)
{
  int t = state_grid_get_type (sg, s);

  switch (t) {
    case TYPE_DEF:
      return REWARD_DEF;

    case TYPE_CLIFF:
      return REWARD_CLIFF;
  }

  /* should never happen */

  return 0.0;
}

int state_grid_get_type (state_grid_t* sg, int s)
{
  return sg->grid[s].type;
}

int state_grid_is_valid_action (state_grid_t* sg, int s, int a)
{
  return sg->grid[s].actions[a];
}

int state_grid_get_best_action (state_grid_t* sg, int s)
{
  int besta = 0;
  double bestq = -100000000.0;
  int a;
  double q;

  for (a=0; a<ACTION_COUNT; a++) {
    if (!state_grid_is_valid_action (sg, s, a))
      continue;

    q = state_grid_get_cost (sg, s, a);
    if (q > bestq) {
      bestq = q;
      besta = a;
    }
  }

  return besta;
}

int state_grid_get_egreedy_action (state_grid_t* sg, int s)
{
  int besta = 0;
  double bestq = -100000000.0;
  int a;
  double q;

  if ((rand () % 100) < (int) (sg->epsilon * 100.0)) {
    while (1) {
      besta = (rand () % ACTION_COUNT);
      if (state_grid_is_valid_action (sg, s, besta))
        break;
    }
  }
  else {
    for (a=0; a<ACTION_COUNT; a++) {
      if (!state_grid_is_valid_action (sg, s, a))
        continue;

      q = state_grid_get_cost (sg, s, a);
      if (q > bestq) {
        bestq = q;
        besta = a;
      }
    }
  }

  return besta;
}

void state_grid_take_action (state_grid_t* sg, agent_t* ag, int s, int a)
{
  int snew;
  point_t* p = state_grid_get_state_point (sg, s);

  /* assumes the action is valid */
  switch (a) {
    case ACTION_N:
      p->y--;
      break;

    case ACTION_S:
      p->y++;
      break;

    case ACTION_E:
      p->x++;
      break;

    case ACTION_W:
      p->x--;
      break;

    default:
      break;
  }

  snew = state_grid_get_state (sg, p);

  ag->r = state_grid_get_reward (sg, snew);

  /* return to initial if we fall down a cliff */
  if (state_grid_get_type (sg, snew) == TYPE_CLIFF)
    snew = sg->start;

  ag->s = snew;
}

double state_grid_get_cost (state_grid_t* sg, int s, int a)
{
  return sg->grid[s].cost[a];
}

void state_grid_update_cost (state_grid_t* sg, int s, int a, int r, int sprime,
                             int aprime)
{
  double q = sg->grid[s].cost[a];
  double qprime = sg->grid[sprime].cost[aprime];
  double qnew = sg->alpha * (r + sg->gamma * qprime - q);

  sg->grid[s].cost[a] += qnew;
}
