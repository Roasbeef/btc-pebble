#include <pebble.h>

#define EXCHANGE_NAME_X_ORIGIN 2
#define EXCHANGE_NAME_Y_ORIGIN 142
#define EXCHANGE_NAME_Y_SIZE 30


static char default_exchange_name[] = "on Mt. Gox";
static char default_conversion_rate[7] = "---.--";
static char default_buy_rate[7] = "383.90";
static char default_sell_rate[7] = "383.40";

static Window *window;
static TextLayer *exchange_name;
static Layer *exchange_top_border;
static Layer *bid_ask_top_border;
static Layer *buy_sell;
static Layer *btc_to_usd;

static AppSync btc_sync;
static uint8_t btc_sync_buffer[64];


enum {
    BTC_VALUE = 0x00,
    BTC_BUY = 0x01,
    BTC_SELL = 0x02
};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Trying %i", btc_sync_buffer[0]);
  //memset(&btc_sync_buffer[0], 0, sizeof(btc_sync_buffer));
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Made room?: %i", btc_sync_buffer[0]);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
    switch (key) {
        case BTC_VALUE:
            strcpy(default_conversion_rate, new_tuple->value->cstring);
            layer_mark_dirty(btc_to_usd);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "VALUE UPDATE: %s", new_tuple->value->cstring);
            break;
        case BTC_SELL:
            strcpy(default_buy_rate, new_tuple->value->cstring);
            layer_mark_dirty(buy_sell);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "SELL UPDATE: %s", new_tuple->value->cstring);
            break;
        case BTC_BUY:
            strcpy(default_sell_rate, new_tuple->value->cstring);
            layer_mark_dirty(buy_sell);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "BUY UPDATE: %s", new_tuple->value->cstring);
            break;
    }
}

void border_update_callback(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void btc_to_usd_update_proc(Layer *self, GContext *ctx) {
  GRect self_bounds = layer_get_bounds(self);
  GFont usd_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, self_bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, GColorWhite);

  graphics_draw_text(ctx, default_conversion_rate, usd_font, self_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void buy_sell_update_proc(Layer *self, GContext *ctx) {
  GRect self_bounds = layer_get_bounds(self);
  GRect buy_bounds = GRect(self_bounds.origin.x + 80, self_bounds.origin.y,
                             self_bounds.size.h, self_bounds.size.w);
  GRect sell_bounds = GRect(self_bounds.origin.x + 80, self_bounds.origin.y + 17,
                             self_bounds.size.h, self_bounds.size.w);
  GFont price_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, self_bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, GColorWhite);

  // Labels
  graphics_draw_text(ctx, "BUY", label_font, 
                     GRect(self_bounds.origin.x, self_bounds.origin.y, 25, 18),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, "SELL", label_font,
                     GRect(self_bounds.origin.x, self_bounds.origin.y + 17, 27, 18),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

  // Prices
  graphics_draw_text(ctx, default_buy_rate, price_font, buy_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
  graphics_draw_text(ctx, default_sell_rate, price_font, sell_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Which exchange?
  exchange_name = text_layer_create((GRect) { .origin = { EXCHANGE_NAME_X_ORIGIN, EXCHANGE_NAME_Y_ORIGIN}, 
                                              .size = { bounds.size.w, EXCHANGE_NAME_Y_SIZE} });
  text_layer_set_text_color(exchange_name, GColorWhite);
  text_layer_set_font(exchange_name, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text(exchange_name, default_exchange_name);
  text_layer_set_background_color(exchange_name, GColorClear);
  text_layer_set_text_alignment(exchange_name, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(exchange_name));

  // Top border above exchange.
  GRect line_frame = GRect(2, 140, 139, 2);
  exchange_top_border = layer_create(line_frame);
  layer_set_update_proc(exchange_top_border, border_update_callback);
  layer_add_child(window_layer, exchange_top_border);

  // Top Border above BID/ASK.
  line_frame = GRect(2, 97, 139, 2);
  bid_ask_top_border = layer_create(line_frame);
  layer_set_update_proc(bid_ask_top_border, border_update_callback);
  layer_add_child(window_layer, bid_ask_top_border);

  // Buy/Sell Prices.
  buy_sell = layer_create(GRect(2, 99, 139, 41));
  layer_set_update_proc(buy_sell, &buy_sell_update_proc);
  layer_add_child(window_layer, buy_sell);

  // Current USD/BTC
  btc_to_usd = layer_create(GRect(2, 2, 139, 41));
  layer_set_update_proc(btc_to_usd, &btc_to_usd_update_proc);
  layer_add_child(window_layer, btc_to_usd);

  // Init dummy values for all the dynamic stuff.
  Tuplet initial_values[] = {
      TupletCString(BTC_VALUE, "000.00"),
      TupletCString(BTC_SELL, "000.00"),
      TupletCString(BTC_BUY, "000.00")
  };

  // Set up sync callback.
  app_sync_init(&btc_sync, btc_sync_buffer, sizeof(btc_sync_buffer),
                initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
}

static void window_unload(Window *window) {
  app_sync_deinit(&btc_sync);

  text_layer_destroy(exchange_name);

  layer_destroy(exchange_top_border);
  layer_destroy(bid_ask_top_border);
  layer_destroy(buy_sell);
  layer_destroy(btc_to_usd);
}

static void init(void) {
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_background_color(window, GColorBlack);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
