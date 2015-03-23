#include <pebble.h>
#include "card_players.h"

static char s_scroll_text[2000] = "Loading...";

static Window *player_window;
static ScrollLayer *s_scroll_layer;
static TextLayer *s_text_layer;

static Layer *s_image_layer;
static GBitmap *s_image_load;
static GBitmap *s_image_player;
static Layer *title_layer;

int showing = 0;
int haveData = 0;

void show_players() {
  window_stack_push(player_window, true);
}

void update_players(char t[]) {
  if (haveData == 0 && strcmp(t, "Loading...") != 0) {
    haveData = 1;
    layer_mark_dirty(s_image_layer);
  }
  strcpy(s_scroll_text, t);
  if (showing == 1) {
    text_layer_set_text(s_text_layer, t);
    
    // Trim text layer and scroll content to fit text box
    Layer *window_layer = window_get_root_layer(player_window);
    GSize max_size = text_layer_get_content_size(s_text_layer);
    GRect bounds = layer_get_frame(window_layer);
    text_layer_set_size(s_text_layer, max_size);
    scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h + 4));
  }
}

static void layer_update_callback(Layer *layer, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.
  if (haveData) {
    GSize image_size = s_image_player->bounds.size;
    graphics_draw_bitmap_in_rect(ctx, s_image_player, GRect(0, 2, image_size.w, image_size.h));
  } else {
    GSize image_size = s_image_load->bounds.size;
    graphics_draw_bitmap_in_rect(ctx, s_image_load, GRect(0, 2, image_size.w, image_size.h));
  } 
}

static void update_title_layer_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, "Players", fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(0, 0, bounds.size.w, bounds.size.h), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(0, 30, bounds.size.w, 2000);

  title_layer = layer_create(GRect(30, 0, bounds.size.w-30, 30));
  layer_set_update_proc(title_layer, update_title_layer_callback);
  layer_add_child(window_layer, title_layer);
    
  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);

  s_image_load = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WAIT);
  s_image_player = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_USER);
  
  // Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);

  // Initialize the text layer
  s_text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(s_text_layer, s_scroll_text);

  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(s_text_layer);
  text_layer_set_size(s_text_layer, max_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h + 4));

  // Add the layers for display
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
  
  // Setup AppSync
  //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Setup initial values
//   Tuplet initial_values[] = {
//     TupletCString(PLAYER_LIST, ""),
//   };

//   // Begin using AppSync
//   app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  layer_destroy(title_layer);
  scroll_layer_destroy(s_scroll_layer);
  
  gbitmap_destroy(s_image_player);
  gbitmap_destroy(s_image_load);
  layer_destroy(s_image_layer);
  //app_sync_deinit(&s_sync);
}

static void window_appear(Window *window) {
  showing = 1;
}

static void window_disappear(Window *window) {
  showing = 0;
}
  
void players_init() {
  player_window = window_create();

  //window_set_click_config_provider_with_context(s_entry_window, click_config_provider, (void*)s_entry_window);
  window_set_window_handlers(player_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear,
  });
}

void players_deinit() {
  window_destroy(player_window);
}