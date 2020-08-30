#define MY_DEVICE_TOPIC       "feeds/device/"

#define ON_TIME               "mode/onoff/on_time"
#define OFF_TIME              "mode/onoff/off_time"

#define STATUS                "status"
#define POWER                 "power"

#define LIGHT_COLOR           "mode/light/color"
#define LIGHT_ZOOM            "mode/light/zoom"
#define LIGHT_DISPLAY_OFFSET  "mode/light/display_offset"

#define MUSIC_COLOR   "mode/music/color"
#define MUSIC_DISPLAY_OFFSET  "mode/music/display_offset"
#define MUSIC_STYLE           "mode/music/style"

enum _CONST_BYTE {
  END_BYTE               = 0x87,
  START_BYTE             = 0x95
};
enum _POWER {
  OFF                    = 0x00,
  ON                     = 0x01
};
enum _CONTROL_TYPE {
  MODE_SWITCH            = 0x01,
  LIGHT                  = 0x02,
  MUSIC                  = 0x03
};
enum _LED {
  COLOR                  = 0x01,
  DISPLAY_OFFSET         = 0x02,
  ZOOM                   = 0x03,
  STYLE                  = 0x04
};
