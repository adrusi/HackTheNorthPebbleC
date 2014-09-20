#include <pebble.h>

static void init(Window **main_window);
static void deinit(Window *win);
static void onload(Window *win);
static void onunload(Window *win);
extern int main();

int main() {
  Window *main_window;
  init(&main_window);
  app_event_loop();
  deinit(main_window);

  return 0;
}

void init(Window **main_window) {
  *main_window = window_create();

  WindowHandlers handlers = {
    .load = onload,
    .unload = onunload
  };
  window_set_window_handlers(*main_window, handlers);

  window_stack_push(*main_window, true);
}

void onload(Window *win) {
  // pass
}

void onunload(Window *win) {
  // pass
}

void deinit(Window *win) {
  window_destroy(win);
}
