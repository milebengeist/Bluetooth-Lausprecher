
#include "BluetoothA2DPSink.h"
#include "Arduino.h"
#include <EasyButton.h>

BluetoothA2DPSink a2dp_sink;

#define BUTTON_VOL_UP_PIN 16
#define BUTTON_VOL_DOWN_PIN 17
#define BUTTON_PLAY_PAUSE_PIN 19
#define BUTTON_PRESSED_DELAY 200
#define BUTTON_RECONNECT 14

unsigned long lastMillis = 0;
unsigned long lastMillisSerial = 0;

// Instance of the button.
EasyButton button(BUTTON_PLAY_PAUSE_PIN,200,true,true);

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2);
void buttonPlayPressed();
void buttonPlayLongPressed();

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
  Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);
}

void buttonPlayPressed()
{
  Serial.println(F("Button VOL PLAY/PAUSE was Pressed"));
  switch (a2dp_sink.get_audio_state())
  {
  case ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND:
    a2dp_sink.play();
    break;
  case ESP_A2D_AUDIO_STATE_STARTED:
    a2dp_sink.pause();
    break;
  case ESP_A2D_AUDIO_STATE_STOPPED:
    a2dp_sink.play();
    break;
  default:
    a2dp_sink.stop();
    break;
  }
}

void buttonPlayLongPressed()
{
  Serial.println(F("Button RECONNECT was Pressed"));
  Serial.print(F("btnReconnect state: "));
  Serial.print(a2dp_sink.get_connection_state());

  a2dp_sink.disconnect();
  a2dp_sink.set_discoverability(ESP_BT_GENERAL_DISCOVERABLE);
}

void setup()
{
  Serial.begin(115200);
  a2dp_sink.set_bits_per_sample(32);
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_TRACK_NUM | ESP_AVRC_MD_ATTR_NUM_TRACKS | ESP_AVRC_MD_ATTR_GENRE | ESP_AVRC_MD_ATTR_PLAYING_TIME);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);

  a2dp_sink.start("Hifi32bit");
  static const i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 44100,                         // corrected by info from bluetooth
      .bits_per_sample = (i2s_bits_per_sample_t)32, /* the A2DP 16 bits will be expanded to 32 bits */
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S_MSB,
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true // avoiding noise in case of data unavailability

  };

  // Init Library and GPIO Pins
  a2dp_sink.set_i2s_config(i2s_config);
  pinMode(BUTTON_VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOL_UP_PIN, INPUT_PULLUP);
  //pinMode(BUTTON_PLAY_PAUSE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RECONNECT, INPUT_PULLUP);

  button.begin();
  

  button.onPressed(buttonPlayPressed);
  button.onPressedFor(2000, buttonPlayLongPressed);
}

void loop()
{
 button.read();
  // Serial.print("LastMillis: ");
  // Serial.println(lastMillis);

  // TODO Play/Pause implementieren
  int btnVolUpState = digitalRead(BUTTON_VOL_UP_PIN);
  int btnVolDownState = digitalRead(BUTTON_VOL_DOWN_PIN);
  //int btnPlayPause = digitalRead(BUTTON_PLAY_PAUSE_PIN);
  int btnReconnect = digitalRead(BUTTON_RECONNECT);

  if ((millis() - lastMillis) > (BUTTON_PRESSED_DELAY / 4)) // wenn aktuelle millisek. - letzter Buttondruck größer als 200/4 = 50
  {
    // Button VOL UP
    if (btnVolUpState == LOW)
    {
      lastMillis = millis();
      Serial.println(F("Button VOL UP was Pressed"));
      int currentVolumeValue = a2dp_sink.get_volume();
      a2dp_sink.set_volume(currentVolumeValue += 1);
      Serial.println(F("Volume was set UP"));
      // lastMillis = millis();
    }

    // Button VOL DOWN
    if (btnVolDownState == LOW)
    {
      lastMillis = millis();
      Serial.println(F("Button VOL DOWN was Pressed"));
      int currentVolumeValue = a2dp_sink.get_volume();
      a2dp_sink.set_volume(currentVolumeValue -= 1);
      Serial.println(F("Volume was set DOWN"));
    }
  }

  /* if ((millis() - lastMillis) > BUTTON_PRESSED_DELAY) // wenn aktuelle millisek. - letzter Buttondruck größer als 200
  {
    if (btnPlayPause == LOW)
    {
      lastMillis = millis();
      Serial.println(F("Button VOL PLAY/PAUSE was Pressed"));
      switch (a2dp_sink.get_audio_state())
      {
      case ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND:
        a2dp_sink.play();
        break;
      case ESP_A2D_AUDIO_STATE_STARTED:
        a2dp_sink.pause();
        break;
      case ESP_A2D_AUDIO_STATE_STOPPED:
        a2dp_sink.play();
        break;
      default:
        a2dp_sink.stop();
        break;
      }

      if (btnReconnect == LOW)
      {
        lastMillis = millis();
        Serial.println(F("Button RECONNECT was Pressed"));
        Serial.print(F("btnReconnect state: "));
        Serial.print(a2dp_sink.get_connection_state());

        a2dp_sink.disconnect();
        a2dp_sink.set_discoverability(ESP_BT_GENERAL_DISCOVERABLE);
      }
    }
  }
  */
}