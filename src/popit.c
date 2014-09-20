#include <stdlib.h>
#include <time.h>
#include <pebble.h>
#include "ui.h"
#include "action.h"

static action_gen gen = { 0 };

extern int main();
static void tick(void *should_be_null);
static void redraw_loop(void *data);

int main() {
  srand(time(NULL));
  ui_init();
  redraw_loop(NULL);
  gen = action_gen_new();
  tick(NULL);
  app_event_loop();
  ui_quit();

  return 0;
}

void tick(void *should_be_null) {
  action_cmd cmd = action_gen_next(&gen);
  action_exec(&cmd, tick, NULL);
}

void redraw_loop(void *data) {
  app_timer_register(100, redraw_loop, data);
  layer_mark_dirty(window_get_root_layer(ui_main_window));
}
