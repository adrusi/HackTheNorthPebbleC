#include "event.h"
#include <pebble.h>
#include "ui.h"

typedef struct {
  char flag;
  event_type type;
  void (*callback)(void *data);
  void *data;
} listener;

#define MAX_LISTENERS 16

static listener listeners[MAX_LISTENERS] = {};

static void click_config_provider(void *data);
static void onback(ClickRecognizerRef recognizer, void *data);
static void onup(ClickRecognizerRef recognizer, void *data);
static void onselect(ClickRecognizerRef recognizer, void *data);
static void ondown(ClickRecognizerRef recognizer, void *data);

event_listener event_register(
    event_type type,
    void (*callback)(void *data),
    void *data) {
  for (event_listener i = 0; i < MAX_LISTENERS; i++) {
    if (!listeners[i].flag) {
      listeners[i] = (listener) { 1, type, callback, data };
      return i;
    }
  }
  // ERROR
  return 255;
}

void event_unregister(event_listener evt) {
  listeners[evt].flag = '\0';
}

void event_init(void) {
  window_set_click_config_provider(ui_main_window, click_config_provider);
}

void click_config_provider(void *data) {
  window_single_click_subscribe(BUTTON_ID_BACK, onback);
  window_single_click_subscribe(BUTTON_ID_UP, onup);
  window_single_click_subscribe(BUTTON_ID_SELECT, onselect);
  window_single_click_subscribe(BUTTON_ID_DOWN, ondown);
}

void onback(ClickRecognizerRef recognizer, void *data) {
  for (int i = 0; i < MAX_LISTENERS; i++) {
    if (!listeners[i].flag) continue;
    if (listeners[i].type != EVENT_BTN_BACK) continue;
    listeners[i].callback(listeners[i].data);
  }
}
void onup(ClickRecognizerRef recognizer, void *data) {
  for (int i = 0; i < MAX_LISTENERS; i++) {
    if (!listeners[i].flag) continue;
    if (listeners[i].type != EVENT_BTN_TOP) continue;
    listeners[i].callback(listeners[i].data);
  }
}
void onselect(ClickRecognizerRef recognizer, void *data) {
  for (int i = 0; i < MAX_LISTENERS; i++) {
    if (!listeners[i].flag) continue;
    if (listeners[i].type != EVENT_BTN_MID) continue;
    listeners[i].callback(listeners[i].data);
  }
}
void ondown(ClickRecognizerRef recognizer, void *data) {
  for (int i = 0; i < MAX_LISTENERS; i++) {
    if (!listeners[i].flag) continue;
    if (listeners[i].type != EVENT_BTN_BOT) continue;
    listeners[i].callback(listeners[i].data);
  }
}
