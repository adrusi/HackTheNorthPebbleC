#ifndef _ACTION_H
#define _ACTION_H

#include <stdint.h>

typedef enum {
  ACTION_TWIST, ACTION_SLAP,
  ACTION_BTN_TOP, ACTION_BTN_MID, ACTION_BTN_BOT,
  ACTION_BTN_BACK,
  ACTION_REST
} action_type;

typedef struct {
  char const *msg;
  action_type type;
  uint32_t duration;
} action_cmd;

typedef struct {
  int num_actions;
} action_gen;

extern action_gen action_gen_new(void);
extern action_cmd action_gen_next(action_gen *gen);
extern void action_exec(
    action_cmd *action,
    void (*callback)(void *data),
    void *data);

#endif
