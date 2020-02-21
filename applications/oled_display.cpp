#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <U8g2lib.h>
#include <stdio.h>

#include <drv_soft_i2c.h>

extern "C"
{
#include <sht3x.h>
}
extern "C"
{
sht3x_device_t sht3x_init(const char *i2c_bus_name, rt_uint8_t sht3x_addr);
rt_err_t sht3x_read_singleshot(sht3x_device_t dev);
}


#define OLED_I2C_PIN_SCL                    7   // PA7
#define OLED_I2C_PIN_SDA                    20  // PB4

static U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,\
                                         /* clock=*/ OLED_I2C_PIN_SCL,\
                                         /* data=*/ OLED_I2C_PIN_SDA,\
                                         /* reset=*/ U8X8_PIN_NONE);

#define SUN 0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4

static void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic

  switch(symbol)
  {
    case SUN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 69);
      break;
    case SUN_CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 65);
      break;
    case CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 64);
      break;
    case RAIN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 67);
      break;
    case THUNDER:
      u8g2.setFont(u8g2_font_open_iconic_embedded_6x_t);
      u8g2.drawGlyph(x, y, 67);
      break;
  }
}

static void drawWeather(uint8_t symbol, int degree)
{
  drawWeatherSymbol(0, 63, symbol);
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(55, 63);
  u8g2.print(degree);
  u8g2.print("C");
}
static void drawHumidity(uint8_t symbol, int humidity)
{
  drawWeatherSymbol(0, 63, symbol);
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(55, 63);
  u8g2.print(humidity);
  u8g2.print("%");
}


void oled_display()
{
    u8g2.begin();
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.setCursor(48+3, 42);
    u8g2.print("Hi~");     // requires enableUTF8Print()

    u8g2.setFont(u8g2_font_6x13_tr);            // choose a suitable font
    u8g2.drawStr(30, 60, "By Mculover666");   // write something to the internal memory
    u8g2.sendBuffer();

    sht3x_device_t  sht3x_device;
    sht3x_device = sht3x_init("i2c1", 0x44);

    rt_thread_mdelay(2000);

    int status = 0;
    char mstr[3];
    char hstr[3];
    time_t now;
    struct tm *p;
    int min = 0, hour = 0;
    int temperature = 0,humidity = 0;

    while(1)
    {
        switch(status)
        {
            case 0:
                now = time(RT_NULL);
                p=gmtime((const time_t*) &now);
                hour = p->tm_hour;
                min = p->tm_min;
                sprintf(mstr, "%02d", min);
                sprintf(hstr, "%02d", hour);


                u8g2.firstPage();
                do {
                     u8g2.setFont(u8g2_font_logisoso42_tn);
                     u8g2.drawStr(0,63,hstr);
                     u8g2.drawStr(50,63,":");
                     u8g2.drawStr(67,63,mstr);
                   } while ( u8g2.nextPage() );


                rt_thread_mdelay(5000);
                status = 1;
                break;
           case 1:
               if(RT_EOK == sht3x_read_singleshot(sht3x_device))
               {
                   temperature = (int)sht3x_device->temperature;
               }
               else
               {
                   temperature = 0;
               }
               u8g2.clearBuffer();
               drawWeather(SUN, temperature);
               u8g2.sendBuffer();
               rt_thread_mdelay(5000);
               status = 2;
               break;
           case 2:
               if(RT_EOK == sht3x_read_singleshot(sht3x_device))
              {
                   humidity = (int)sht3x_device->humidity;
              }
              else
              {
                  humidity = 0;
              }
              u8g2.clearBuffer();
              drawHumidity(RAIN, humidity);
              u8g2.sendBuffer();
              rt_thread_mdelay(5000);
              status = 0;
              break;
        }
    }
}

MSH_CMD_EXPORT(oled_display, oled start);
