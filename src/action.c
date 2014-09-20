#include "action.h"
#include <stdlib.h>
#include <stdio.h>
#include <pebble.h>
#include "ui.h"
#include "event.h"

typedef struct {
  void (*callback)(void *data);
  void *data;
  TextLayer *fail_layer;
} fail_callback_data;

static void action_exec_fail_timeout_callback(void *data);
static void action_exec_required_timeout_callback(void *data);
static void action_exec_fail_callback(void *data);
static void correct_input(void *data);
static void incorrect_input(void *data);
static void fail_msg(void (*callback)(void *data), void *data);
static void restore_background_color(void *useless);
static event_type action2event(action_type action);

event_type action2event(action_type action) {
  switch (action) {
    case ACTION_TWIST: return EVENT_TWIST;
    case ACTION_SLAP:  return EVENT_SLAP;
    case ACTION_BTN_TOP: return EVENT_BTN_TOP;
    case ACTION_BTN_MID: return EVENT_BTN_MID;
    case ACTION_BTN_BOT: return EVENT_BTN_BOT;
    case ACTION_BTN_BACK: return EVENT_BTN_BACK;
    default: return 255;

  }
}

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
    case ACTION_TWIST: return (action_cmd) { "Twist!", type, 2000 };
    case ACTION_SLAP: return (action_cmd) { "Slap!", type, 2000 };
    case ACTION_BTN_TOP: return (action_cmd) { "Up!", type, 2000 };
    case ACTION_BTN_MID: return (action_cmd) { "Select!", type, 2000 };
    case ACTION_BTN_BOT: return (action_cmd) { "Down!", type, 2000 };
    case ACTION_BTN_BACK: return (action_cmd) { "Back!", type, 2000 };
    case ACTION_REST: return (action_cmd) { "STOP!", type, 3000 };
  }
  return (action_cmd) { "STOP!", ACTION_REST, 0 };
}
  
typedef struct {
  AppTimer *timer;
  event_listener evt[EVENT_TYPE_CNT];
  void (*callback)(void *data);
  void *data;
  TextLayer *msg_layer;
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
  callback_data->msg_layer = msg_layer;

  static event_type event_types[EVENT_TYPE_CNT] = {
    EVENT_BTN_TOP, EVENT_BTN_MID, EVENT_BTN_BOT,
    EVENT_BTN_BACK,
    EVENT_TWIST, EVENT_SLAP
  };
  switch (action->type) {
  case ACTION_REST:
    callback_data->timer = app_timer_register(
        action->duration,
        action_exec_required_timeout_callback,
        callback_data);

    for (int i = 0; i < EVENT_TYPE_CNT; i++) {
      callback_data->evt[i] = event_register(
          event_types[i],
          incorrect_input,
          callback_data);
    }
    break;

  default:
    callback_data->timer = app_timer_register(
        action->duration,
        action_exec_fail_timeout_callback,
        callback_data);

    for (int i = 0; i < EVENT_TYPE_CNT; i++) {
      if (action->type == ACTION_BTN_TOP, 
      callback_data->evt[i] = event_register(
          event_types[i],
          event_types[i] == action2event(action->type)
            ? correct_input
            : incorrect_input,
          callback_data);
    }
  }
}

void incorrect_input(void *data) {
  app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "incorrect");
  action_exec_data exec_data = *((action_exec_data *) data);
  free(data);

  for (int i = 0; i < EVENT_TYPE_CNT; i++) {
    event_unregister(exec_data.evt[i]);
  }
  app_timer_cancel(exec_data.timer);

  layer_remove_from_parent(text_layer_get_layer(exec_data.msg_layer));
  text_layer_destroy(exec_data.msg_layer);

  fail_msg(exec_data.callback, exec_data.data);
}

void correct_input(void *data) {
  app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "correct");
  action_exec_data exec_data = *((action_exec_data *) data);
  free(data);

  for (int i = 0; i < EVENT_TYPE_CNT; i++) {
    event_unregister(exec_data.evt[i]);
  }
  app_timer_cancel(exec_data.timer);

  layer_remove_from_parent(text_layer_get_layer(exec_data.msg_layer));
  text_layer_destroy(exec_data.msg_layer);
  
  window_set_background_color(ui_main_window, GColorBlack);
  app_timer_register(200, restore_background_color, NULL);
  
  exec_data.callback(exec_data.data);
}

void restore_background_color(void *useless) {
  window_set_background_color(ui_main_window, GColorWhite);
}

void action_exec_fail_timeout_callback(void *data) {
  action_exec_data exec_data = *((action_exec_data *) data);
  free(data);

  for (int i = 0; i < EVENT_TYPE_CNT; i++) {
    event_unregister(exec_data.evt[i]);
  }

  layer_remove_from_parent(text_layer_get_layer(exec_data.msg_layer));
  text_layer_destroy(exec_data.msg_layer);

  fail_msg(exec_data.callback, exec_data.data);
}

void fail_msg(void (*callback)(void *data), void *data) {
  vibes_short_pulse();

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

 fail_callback_data *callback_data = malloc(sizeof (fail_callback_data));
 callback_data->callback = callback;
 callback_data->data = data;
 callback_data->fail_layer = fail_layer;

 app_timer_register(
     500,
     action_exec_fail_callback,
     callback_data);
}

void action_exec_fail_callback(void *data) {
  fail_callback_data timer_data = *((fail_callback_data *) data);
  free(data);
  layer_remove_from_parent(text_layer_get_layer(timer_data.fail_layer));
  text_layer_destroy(timer_data.fail_layer);
  timer_data.callback(timer_data.data);
}

void action_exec_required_timeout_callback(void *data) {
  correct_input(data);
}
