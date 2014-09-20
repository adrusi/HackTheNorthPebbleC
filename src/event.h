#ifndef _EVENT_H
#define _EVENT_H

#include <stdint.h>

#define EVENT_TYPE_CNT 7
typedef enum {
  EVENT_BTN_TOP, EVENT_BTN_MID, EVENT_BTN_BOT,
  EVENT_BTN_BACK,
  EVENT_TWIST, EVENT_SLAP
} event_type;

typedef uint8_t event_listener;

extern event_listener event_register(
    event_type type,
    void (*callback)(void *data),
    void *data);
extern void event_unregister(event_listener evt);
extern void event_init(void);

#endif
