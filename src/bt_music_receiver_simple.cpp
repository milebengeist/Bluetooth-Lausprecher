/*
  Streaming Music from Bluetooth

  Copyright (C) 2020 Phil Schatzmann
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ==> Example A2DP Receiver which uses I2S to an external DAC

#include "BluetoothA2DPSink.h"
#include "Arduino.h"

BluetoothA2DPSink a2dp_sink;

#define BUTTON_VOL_UP_PIN 16
#define BUTTON_VOL_DOWN_PIN 17
#define VOL_DELAYTIME 100
#define BUTTON_PLAY_PAUSE_PIN 13

unsigned long lastMillis = 0;

void setup()
{

  a2dp_sink.set_bits_per_sample(32);
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

  //Init Library and GPIO Pins
  a2dp_sink.set_i2s_config(i2s_config);
  pinMode(BUTTON_VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOL_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PLAY_PAUSE_PIN, INPUT_PULLUP);
}

void loop()
{

  Serial.begin(115200);
  Serial.println(lastMillis);

  // TODO Play/Pause implementieren
  int btnVolUpState = digitalRead(BUTTON_VOL_UP_PIN);
  int btnVolDownState = digitalRead(BUTTON_VOL_DOWN_PIN);
  int btnPlayPause = digitalRead(BUTTON_PLAY_PAUSE_PIN);

  if (btnVolUpState == LOW)
  {
    if ((millis() - lastMillis) > VOL_DELAYTIME)
    { // Nur wenn Zeitspanne zwischen letzem mal schaun > x ms ist
      int currentVolumeValue = a2dp_sink.get_volume();
      a2dp_sink.set_volume(currentVolumeValue += 1);
      Serial.println(F("Volume was set UP"));
      lastMillis = millis();
    }
  }
  if (btnVolDownState == LOW)
  {
    if ((millis() - lastMillis) > VOL_DELAYTIME)
    {
      int currentVolumeValue = a2dp_sink.get_volume();
      a2dp_sink.set_volume(currentVolumeValue -= 1);
      Serial.println(F("Volume was set DOWN"));
      lastMillis = millis();
    }
  }
  if (millis() - lastMillis > 200)
  {
    Serial.print(F("CURRENT VOLUME: "));
    Serial.println(a2dp_sink.get_volume());
  }

  Serial.print(F("Status Button VolUP: "));

  Serial.println(btnVolUpState);
  delay(100);

  if (btnPlayPause == LOW)
  {
    a2dp_sink.pause();
    Serial.print(btnPlayPause);

    if (btnPlayPause == LOW)

      a2dp_sink.play();
    Serial.print(btnPlayPause);
  }
}