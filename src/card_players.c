#include <pebble.h>
#include "card_players.h"

static char s_scroll_text[] = "PLAYERS! \nLorem ipsum dolor sit amet, consectetur adipiscing elit. Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien. Vestibulum lobortis pellentesque pretium. Quisque ultricies purus e  u orci convallis lacinia. Cras a urna mi. Donec convallis ante id dui dapibus nec ullamcorper erat egestas. Aenean a m  auris a sapien commodo lacinia. Sed posuere mi vel risus congue ornare. Curabitur leo nisi, euismod ut pellentesque se  d, suscipit sit amet lorem. Aliquam eget sem vitae sem aliquam ornare. In sem sapien, imperdiet eget pharetra a, lacin  ia ac justo. Suspendisse at ante nec felis facilisis eleifend.";

static Window *player_window;
static ScrollLayer *s_scroll_layer;
static TextLayer *s_text_layer;

void show_players() {
  window_stack_push(player_window, true);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);

  // Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);

  // Initialize the text layer
  s_text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(s_text_layer, s_scroll_text);

  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(s_text_layer);
  text_layer_set_size(s_text_layer, max_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h + 4));

  // Add the layers for display
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
}
  
static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  scroll_layer_destroy(s_scroll_layer);
}
  
void players_init() {
  player_window = window_create();

  //window_set_click_config_provider_with_context(s_entry_window, click_config_provider, (void*)s_entry_window);
  window_set_window_handlers(player_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void players_deinit() {
  window_destroy(player_window);
}