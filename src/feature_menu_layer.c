#include "pebble.h"
#include "card_players.h"
//#include "card_chat.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 2
#define NUM_FIRST_MENU_ITEMS 2
  
#define KEY_PLAYER_LIST 1
#define KEY_CHAT_LIST 2
#define KEY_PAGE_CONTROL 3

#define WINDOW_MENU 0
#define WINDOW_PLAYERS 1
#define WINDOW_CHAT 2
  
static Window *s_main_window;
static MenuLayer *s_menu_layer;
static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static AppSync s_sync;
static uint8_t s_sync_buffer[1024];
int activeWindow = 0;

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {  
  //Get the new value from the data sync 
  if ((int)key == activeWindow) {
    static char s_count_buffer[2000];
    snprintf(s_count_buffer, sizeof(s_count_buffer), "%s", new_tuple->value->cstring);
    
    APP_LOG(APP_LOG_LEVEL_INFO, "App Received: %d  %s", (int)key, s_count_buffer);
    update_players(s_count_buffer);
  }
  
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
}

static void request_content(int page) {
  activeWindow = page;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }

  dict_write_int(iter, KEY_PAGE_CONTROL, &page, sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Misfit MC");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "Online Players", "See who is online", s_menu_icons[0]);
          break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Live Chat", "Live server chat", s_menu_icons[1]);
          break;
      }
      break;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  char title[50];
  
  switch (cell_index->row) {
    case 0:
      strcpy(title, "Players");
      show_players(title, RESOURCE_ID_IMAGE_USER);
      request_content(WINDOW_PLAYERS);
      break;
    case 1:
      strcpy(title, "Chat");
      show_players(title, RESOURCE_ID_IMAGE_CHAT);
      request_content(WINDOW_CHAT);
      break;
  }
}

static void main_window_load(Window *window) {
  // Here we load the bitmap assets
  s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_USER);
  s_menu_icons[1]= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHAT);

  // Now we prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void main_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(s_menu_icons[i]);
  }
}

static void main_window_appear(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Back in main menu!");
  request_content(WINDOW_MENU);
  activeWindow = WINDOW_MENU;
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
    .appear = main_window_appear
  });
  window_stack_push(s_main_window, true);
  
  // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  //Setup initial values
  Tuplet initial_values[] = {
    TupletCString(KEY_PLAYER_LIST, "Loading..."),
    TupletCString(KEY_CHAT_LIST, "Loading..."),
  };

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
  
  players_init();
  //chat_init();
}

static void deinit() {
  app_sync_deinit(&s_sync);
  players_deinit();
  //chat_deinit();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
