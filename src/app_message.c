#include <pebble.h>
#include "app_message.h"
#include "face_handler.h"

char icon[7];

static void inbox_received_callback(DictionaryIterator* it, void* context)
{  
  static char temp[8];
  // Read first item
  Tuple *t = dict_read_first(it);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "-- inbox_received_callback --");
  // For all items
  while(t != NULL)
  {
    // Which key was received?
    if (t->key == MESSAGE_KEY_TEMPERATURE)
    {
      snprintf(temp, sizeof(temp), "%d°", (int)t->value->int32);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE_KEY_TEMPERATURE received");
    }
    else if (t->key == MESSAGE_KEY_ICON)
    {
      snprintf(icon, sizeof(icon), "%s", t->value->cstring);
      icon_handler();
      APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE_KEY_ICON recieved.");
    }
    else
    {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
    }
    // Look for next item
    t = dict_read_next(it);
  }
  text_layer_set_text(s_weather_temp, temp);
}

static void inbox_dropped_callback(AppMessageResult reason, void* context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator* it, AppMessageResult reason, void* context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
  
  switch(reason)
  {
  case APP_MSG_SEND_TIMEOUT:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Send Timeout.");
    break;
  case APP_MSG_BUSY:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Busy");
    break;
  case APP_MSG_SEND_REJECTED:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Send Rejected");
    break;
  case APP_MSG_OUT_OF_MEMORY:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Out of Memory!");
    break;
  case APP_MSG_ALREADY_RELEASED:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Already Released");
    break;
  case APP_MSG_BUFFER_OVERFLOW:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Buffer Overflow");
    break;
  case APP_MSG_INTERNAL_ERROR:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: Internal Error");
    break;
  case APP_MSG_APP_NOT_RUNNING:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: App Not Running");
    break;
  default:
    APP_LOG(APP_LOG_LEVEL_ERROR, "reason: unhandled case...");
    break;
  }
}

static void outbox_sent_callback(DictionaryIterator* it, void* context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void message_init(void)
{
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void message_deinit(void)
{
  app_message_deregister_callbacks();
}

void pull_weather(void)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Pulling weather...");
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
