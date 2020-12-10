#define MY_DEVICE_TOPIC       "feeds/device/"

#define ON_TIME               "mode/onoff/on_time"
#define OFF_TIME              "mode/onoff/off_time"

#define CURR_MODE             "status"
#define POWER                 "power"
#define INFO                  "info"
#define OPTION_CONFIG         "option_config"

#define LIGHT_COLOR           "mode/light/color"
#define LIGHT_ZOOM            "mode/light/zoom"
#define LIGHT_DISPLAY_OFFSET  "mode/light/offset"
#define LIGHT_DISPLAY         "mode/light/display"

#define MUSIC_COLOR           "mode/music/color"
#define MUSIC_DISPLAY_OFFSET  "mode/music/offset"
#define MUSIC_STYLE           "mode/music/style"
#define MUSIC_DISPLAY         "mode/music/display"

#define BT_MODULE_STATUS      "bt_module/status"
#define BT_MODULE_OPERATION   "bt_module/operation"

/********** JSON KEY ***********/
#define SL_ID                    "id"
#define SL_VALUE                 "value"
#define SL_UPDATE_AT             "update_at"

#define SL_RED                   "r"
#define SL_GREEN                 "g"
#define SL_BLUE                  "b"
#define SL_COLOR                 "color"
#define SL_HIGH                  "h"
#define SL_MEDIUM                "m"
#define SL_LOW                   "l"
#define SL_LEV                   "level"
#define SL_BRIGHT                "brightness"
#define SL_ZOOM                  "zoom"
#define SL_OFFSET                "offset"

#define SL_HOUR                  "hour"
#define SL_MIN                   "min"
#define SL_SEC                   "sec"
#define SL_ENABLE                "enable"

// bluetooth module
#define SL_CONNECT               "cnt"
#define SL_IS_PLAY               "is_play"
#define SL_VOLUME                "vol"
#define SL_OPERATION             "operation"

#define SL_FFT_MAG               "fft_mag"

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
  SETTING                = 0x04,
  BT_MODULE              = 0x05
};
enum _LED {
  COLOR                  = 0x01,
  // DISPLAY_OFFSET, ZOOM合併至DISPLAY
  DISPLAY_OFFSET         = 0x02,
  ZOOM                   = 0x03,
  STYLE                  = 0x04,
  LED_DISPLAY            = 0x05
};

enum _BT_MODULE {
  BT_STATUS = 0x06,
  BT_OPERATION = 0x07
};

enum _SETTINGS {
  SYNC_TIME              = 0xFF,
  OPTION                 = 0xFE
};
