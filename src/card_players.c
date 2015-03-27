#include <pebble.h>
#include "card_players.h"

static char s_scroll_text[2000] = "Loading...";
static char s_title_text[50] = "Content";
//static uint32_t resource_id = RESOURCE_ID_IMAGE_WAIT;

static Window *player_window;
static ScrollLayer *s_scroll_layer;
static TextLayer *s_text_layer;

static Layer *s_image_layer;
static GBitmap *s_image_load;
static GBitmap *s_image_player;
static Layer *title_layer;
static InverterLayer *s_inverter_layer;

int showing = 0;
int haveData = 0;

void show_players(char new_title[], uint32_t img) {
  strcpy(s_title_text, new_title);
  s_image_player = gbitmap_create_with_resource(img);
  
  window_stack_push(player_window, true);
}

void update_players(char t[]) {
  
  //If we've changed from Loading..., update image
  if (haveData == 0 && strcmp(t, "Loading...") != 0) {
    haveData = 1;//global to say we're done loading
    
    if (showing == 1) { //if we're already showing menu, change image live
      layer_mark_dirty(s_image_layer);
    }
  }
  
  if (showing == 1 && strcmp(t, s_scroll_text) != 0) {
    vibes_short_pulse();
  }
  
  strcpy(s_scroll_text, t); //copy the new data into scroll text variable
  
  if (showing == 1) { //If we're already showing,
    text_layer_set_text(s_text_layer, t); 
    
    // Trim text layer and scroll content to fit text box
    Layer *window_layer = window_get_root_layer(player_window);
    GSize max_size = text_layer_get_content_size(s_text_layer);
    GRect bounds = layer_get_frame(window_layer);
    text_layer_set_size(s_text_layer, max_size);
    scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h+4));
  }
}

static void layer_update_callback(Layer *layer, GContext* ctx) {
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
  graphics_draw_text(ctx, s_title_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(0, 0, bounds.size.w, bounds.size.h), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);
  
  // Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(GRect(0, 33, bounds.size.w, bounds.size.h-33));
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  
    //Add a title to the WINDOW
    title_layer = layer_create(GRect(30, 0, bounds.size.w-30, 33));
    layer_set_update_proc(title_layer, update_title_layer_callback);
    layer_add_child(window_layer, title_layer);

    //Add and image to the WINDOW
    s_image_layer = layer_create(GRect(1, 1, 28, 28));
    layer_set_update_proc(s_image_layer, layer_update_callback);
    layer_add_child(window_layer, s_image_layer);
  
    //Load in our two images
    s_image_load = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WAIT);
    //s_image_player = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WAIT);

  // Initialize the text layer
  s_text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(s_text_layer, s_scroll_text);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(s_text_layer);
  text_layer_set_size(s_text_layer, max_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h+4));

  // Add the layers for display
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  //Add inverter layer over title
  s_inverter_layer = inverter_layer_create(GRect(0, 0, bounds.size.w, 33));
  layer_add_child(window_layer, inverter_layer_get_layer(s_inverter_layer));
  
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  layer_destroy(title_layer);
  scroll_layer_destroy(s_scroll_layer);
  inverter_layer_destroy(s_inverter_layer);
  
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
  haveData = 0;
  strcpy(s_scroll_text, "Loading...");
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