#define MY_DEVICE_TOPIC       "feeds/device/"

#define ON_TIME               "mode/onoff/on_time"
#define OFF_TIME              "mode/onoff/off_time"

#define CURR_MODE             "status"
#define POWER                 "power"

#define LIGHT_COLOR           "mode/light/color"
#define LIGHT_ZOOM            "mode/light/zoom"
#define LIGHT_DISPLAY_OFFSET  "mode/light/offset"

#define MUSIC_COLOR           "mode/music/color"
#define MUSIC_DISPLAY_OFFSET  "mode/music/offset"
#define MUSIC_STYLE           "mode/music/style"

/********** JSON KEY ***********/
#define SL_ID                    "id"
#define SL_VALUE                 "value"

#define SL_RED                   "red"
#define SL_GREEN                 "green"
#define SL_BLUE                  "blue"
#define SL_LEV                   "level"
#define SL_BRIGHT                "brightness"
#define SL_ZOOM                  "zoom"

#define SL_HOUR                  "hour"
#define SL_MIN                   "min"
#define SL_SEC                   "sec"
#define SL_ENABLE                "enable"

enum _CONST_BYTE {
  END_BYTE               = 0x87,
  START_BYTE             = 0x95
};
enum _STATUS {
  OFF                    = 0x00,
  ON                     = 0x01,
  STATUS_LIGHT           = 0x02,
  STATUS_MUSIC           = 0x03
};
enum _CONTROL_TYPE {
  MODE_SWITCH            = 0x01,
  LIGHT                  = 0x02,
  MUSIC                  = 0x03,
  SETTING                = 0x04
};
enum _LED {
  COLOR                  = 0x01,
  DISPLAY_OFFSET         = 0x02,
  ZOOM                   = 0x03,
  STYLE                  = 0x04
};

enum _SETTINGS {
  SYNC_TIME              = 0xFF
};
