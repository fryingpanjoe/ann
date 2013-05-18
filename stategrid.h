enum {
  ACTION_N = 0,
  ACTION_S,
  ACTION_E,
  ACTION_W,
  ACTION_COUNT
};

enum {
  TYPE_DEF = 0,
  TYPE_CLIFF
};

#define REWARD_DEF            -1
#define REWARD_CLIFF          -100

typedef struct point_s {
  int x;
  int y;
} point_t;

typedef struct agent_s {
  int s;
  double r;
} agent_t;

typedef struct state_s {
  int actions[ACTION_COUNT]; /* boolean */
  double cost[ACTION_COUNT];
  int type;
} state_t;

typedef struct state_grid_s {
  unsigned w;
  unsigned h;

  int start;
  int end;

  double alpha;
  double gamma;
  double epsilon;

  state_t* grid;
} state_grid_t;

state_grid_t* state_grid_new (unsigned w, unsigned h, double alpha,
                              double gamma, double epsilon);
void state_grid_free (state_grid_t* sg);
void state_grid_display (state_grid_t* sg, int s);
void state_grid_init (state_grid_t* sg);

int state_grid_get_state (state_grid_t* sg, point_t* p);
point_t* state_grid_get_state_point (state_grid_t* sg, int s);

double state_grid_get_reward (state_grid_t* sg, int s);
int state_grid_get_type (state_grid_t* sg, int s);

int state_grid_get_inverse_action (int a);
int state_grid_is_valid_action (state_grid_t* sg, int s, int a);
int state_grid_get_best_action (state_grid_t* sg, int s);
int state_grid_get_egreedy_action (state_grid_t* sg, int s);
void state_grid_take_action (state_grid_t* sg, agent_t* ag, int s, int a);

double state_grid_get_cost (state_grid_t* sg, int s, int a);
void state_grid_update_cost (state_grid_t* sg, int s, int a, int r, int sprime,
                             int aprime);
