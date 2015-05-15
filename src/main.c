#include "pebble.h"
#include "libs/pebble-assist.h"
#include "elements.h"

static void update_bg(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
#ifdef PBL_COLOR
  bat = battery_state_service_peek().charge_percent / 10;
#endif

#ifdef PBL_COLOR
  if (bat >= 7) {
    graphics_context_set_fill_color(ctx, GColorIslamicGreen);
  }
  else if (bat >= 4) {
    graphics_context_set_fill_color(ctx, GColorIcterine);
  }
  else {
    graphics_context_set_fill_color(ctx, GColorRed);
  }
#else 
    graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 68);
#ifdef PBL_COLOR
  if (bat >= 7) {
    graphics_context_set_fill_color(ctx, GColorIslamicGreen);
  }
  else if (bat >= 4) {
    graphics_context_set_fill_color(ctx, GColorIcterine);
  }
  else {
    graphics_context_set_fill_color(ctx, GColorRed);
  }
#else 
    graphics_context_set_fill_color(ctx, GColorBlack);
#endif
    graphics_fill_circle(ctx, center, 63);
}

static void update_time(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  time_t epoch = time(NULL); 
  struct tm *t = localtime(&epoch);
  
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  int32_t hour_angle = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10)) / (12 * 6);
  
  int minX = 53 * sin_lookup(minute_angle) / TRIG_MAX_RATIO + center.x;
  int minY = -53 * cos_lookup(minute_angle) / TRIG_MAX_RATIO + center.y;
  
  int hourX = 23 * sin_lookup(hour_angle) / TRIG_MAX_RATIO + center.x;
  int hourY = -23 * cos_lookup(hour_angle) / TRIG_MAX_RATIO + center.y;
  
  graphics_context_set_fill_color(ctx, GColorWhite);
#ifdef PBL_COLOR
  if (bat >= 4 && bat <= 6) {
    graphics_context_set_fill_color(ctx, GColorBlack);
  }
#endif
  
  graphics_fill_circle(ctx, GPoint(minX, minY), 8);
  graphics_fill_circle(ctx, GPoint(hourX, hourY), 13);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_time_layer);
  layer_mark_dirty(s_background_layer);
}

static void main_window_load(Window *window) {
  GRect bounds = window_get_bounds(window);
  
  s_background_layer = layer_create(bounds);
  s_time_layer = layer_create(bounds);
  
  layer_set_update_proc(s_background_layer, update_bg);
  layer_set_update_proc(s_time_layer, update_time);
  
  layer_add_to_window(s_background_layer, window);
  layer_add_to_window(s_time_layer, window);
}

static void main_window_unload(Window *window) {
  layer_destroy_safe(s_time_layer);
  layer_destroy_safe(s_background_layer);
}

static void init() {
  s_main_window = window_create();
  window_handlers(s_main_window, main_window_load, main_window_unload);
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy_safe(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}