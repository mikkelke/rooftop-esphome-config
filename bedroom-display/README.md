# AirDot bedroom

This configuration targets the Sensy-One AirDot with an ESP32-S3, 16 MB of flash and a 480 × 480 NHD-2.1-480480AF-ASXP RGB panel. The panel and LTR303 drivers and SEN66 compensation come from Sensy-One AirDot commit `28df83b373a39abe05cdef28c9bf952103b5ac6d`. See [Sensy-One's hardware-bound licence](LICENSE.txt).

The display uses `sensor.bedroom_display`. In sleep mode, it switches off the backlight until the button wakes the night face for 30 seconds. A short press outside sleep mode sends `esphome.bedroom_display_play`. A press of at least one second sends `esphome.bedroom_display_alarm`. Home Assistant requires `allow_service_calls` for these events.

Display Power enables the screen to follow Home Assistant's view. Display Brightness sets the normal brightness and is capped at 19% from 23:00 to 06:00 and at 4% for the night face. Air-quality readings remain available to Home Assistant in sleep mode.

Build with `esp-acer-build bedroom-display --no-upload`. The first USB flash uses `firmware.factory.bin`. Update the existing ESPHome entry with the key stored as `bedroom_display_api_key` to reconnect Home Assistant after flashing.
