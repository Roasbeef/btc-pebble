#include <pebble.h>

#define EXCHANGE_NAME_X_ORIGIN 2
#define EXCHANGE_NAME_Y_ORIGIN 142
#define EXCHANGE_NAME_Y_SIZE 30


static char default_exchange_name[] = "on Mt. Gox";

static Window *window;
static TextLayer *exchange_name;
static Layer *exchange_top_border;
static Layer *bid_ask_top_border;
static Layer *buy_sell;
static Layer *btc_to_usd;


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

  graphics_draw_text(ctx, "371", usd_font, self_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void buy_sell_update_proc(Layer *self, GContext *ctx) {
  GRect self_bounds = layer_get_bounds(self);
  GRect label_bounds = GRect(self_bounds.origin.x, self_bounds.origin.y + 17,
                            self_bounds.size.h, self_bounds.size.w);
  GRect buy_bounds = GRect(self_bounds.origin.x + 90, self_bounds.origin.y,
                             self_bounds.size.h, self_bounds.size.w);
  GRect sell_bounds = GRect(self_bounds.origin.x + 90, self_bounds.origin.y + 17,
                             self_bounds.size.h, self_bounds.size.w);
  GFont price_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, self_bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, GColorWhite);

  // Labels
  graphics_draw_text(ctx, "BUY", label_font, self_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, "SELL", label_font, label_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

  // Prices
  graphics_draw_text(ctx, "370", price_font, buy_bounds, 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
  graphics_draw_text(ctx, "360", price_font, sell_bounds, 
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
}

static void window_unload(Window *window) {
  text_layer_destroy(exchange_name);
  layer_destroy(exchange_top_border);
  layer_destroy(bid_ask_top_border);
  layer_destroy(buy_sell);
}

static void init(void) {
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
  window_set_background_color(window, GColorBlack);
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
