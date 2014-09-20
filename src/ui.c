#include "ui.h"

static void onload(Window *win);
static void onunload(Window *win);

Window *ui_main_window = NULL;

void ui_init(void) {
  ui_main_window = window_create();

  WindowHandlers handlers = { .load = onload, .unload = onunload };
  window_set_window_handlers(ui_main_window, handlers);

  window_stack_push(ui_main_window, true);
}

void onload(Window *win) {
  // pass
}

void onunload(Window *win) {
  // pass
}

void ui_quit(void) {
  window_destroy(ui_main_window);
}
