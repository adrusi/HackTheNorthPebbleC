#include "action.h"
#include <stdlib.h>
#include <pebble.h>
#include "ui.h"

static void action_exec_timer_callback(void *data);
static void action_exec_timer_callback2(void *data);

action_gen action_gen_new(void) {
  return (action_gen) { 0 };
}

#define NUM_ACTIONS 7
action_cmd action_gen_next(action_gen *gen) {
  static action_type action_types[NUM_ACTIONS] = {
    ACTION_TWIST, ACTION_SLAP,
    ACTION_BTN_TOP, ACTION_BTN_MID, ACTION_BTN_BOT,
    ACTION_BTN_BACK,
    ACTION_REST
  };
  gen->num_actions++;
  action_type type = action_types[rand() % NUM_ACTIONS];
  switch (type) {
    case ACTION_TWIST: return (action_cmd) { "Twist It!", type, 2000 };
    case ACTION_SLAP: return (action_cmd) { "Slap It!", type, 2000 };
    case ACTION_BTN_TOP: return (action_cmd) { "Up It!", type, 2000 };
    case ACTION_BTN_MID: return (action_cmd) { "Press It!", type, 2000 };
    case ACTION_BTN_BOT: return (action_cmd) { "Down It!", type, 2000 };
    case ACTION_BTN_BACK: return (action_cmd) { "Left It!", type, 2000 };
    case ACTION_REST: return (action_cmd) { "STOP!", type, 4000 };
  }
  return (action_cmd) { "STOP!", ACTION_REST, 0 };
}
  
typedef struct {
  AppTimer *timer;
  void (*callback)(void *data);
  void *data;
  Layer *msg_layer;
} action_exec_data;
void action_exec(
    action_cmd *action,
    void (*callback)(void *data),
    void *data) {
  TextLayer *msg_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(msg_layer, GColorClear);
  text_layer_set_text_color(msg_layer, GColorBlack);
  text_layer_set_text(msg_layer, action->msg);
  text_layer_set_font(msg_layer,
      fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(msg_layer, GTextAlignmentCenter);
  Layer *raw_layer = text_layer_get_layer(msg_layer);
  layer_add_child(
      window_get_root_layer(ui_main_window),
      raw_layer);

  action_exec_data *callback_data = malloc(sizeof (action_exec_data));
  callback_data->callback = callback;
  callback_data->data = data;
  callback_data->msg_layer = raw_layer;

  AppTimer *timer = app_timer_register(
      action->duration,
      action_exec_timer_callback,
      callback_data);
  callback_data->timer = timer;
}

typedef struct {
  void (*callback)(void *data);
  void *data;
  Layer *fail_layer;
} timer_callback_data;
void action_exec_timer_callback(void *data) {
  action_exec_data exec_data = *((action_exec_data *) data);
  free(data);

  layer_remove_from_parent(exec_data.msg_layer);

  TextLayer *fail_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(fail_layer, GColorClear);
  text_layer_set_text_color(fail_layer, GColorBlack);
  text_layer_set_text(fail_layer, "FAIL!");
  text_layer_set_font(fail_layer,
      fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(fail_layer, GTextAlignmentCenter);
  Layer *raw_layer = text_layer_get_layer(fail_layer);
  layer_add_child(
      window_get_root_layer(ui_main_window),
      raw_layer);

 timer_callback_data *callback_data = malloc(sizeof (timer_callback_data));
 callback_data->callback = exec_data.callback;
 callback_data->data = exec_data.data;
 callback_data->fail_layer = raw_layer;

 app_timer_register(
     500,
     action_exec_timer_callback2,
     callback_data);
}

void action_exec_timer_callback2(void *data) {
  timer_callback_data timer_data = *((timer_callback_data *) data);
  free(data);
  layer_remove_from_parent(timer_data.fail_layer);
  timer_data.callback(timer_data.data);
}
