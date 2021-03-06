#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_tasks.h"
#include "shared.h"
#include "usbd_hid.h"
#include "ssd1306.h"
#include "fonts.h"
#include "neopixel.h"
#include "buttons.h"
#include "keyboard.h"
#include "parser.h"
#include "animations.h"

uint8_t init_complete;
uint32_t last_keypress;
uint32_t next_pixel_shift = 30000;

void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(16);
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
      if(is_pressed(&button_status[i]))
      {
        last_keypress = HAL_GetTick();
        ssd1306_dim(0); // OLED back to full brightness

        if(i <= KEY_14)
        {
          keydown_anime_start(i);
          handle_keypress(i, &button_status[i]); // handle the button state inside here for repeats
          keydown_anime_end(i);
        }
        else if(i == KEY_BUTTON1) // -
          change_profile(PREV_PROFILE);
        else if(i == KEY_BUTTON2) // +
          change_profile(NEXT_PROFILE);
        service_press(&button_status[i]);
      }
    osDelay(16);
  }
}

void animation_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(25);
  anime_init();
  for(;;)
  {
    led_animation_handler();
    // dim OLED screen after 5 minutes of idle to prevent burn-in
    if(HAL_GetTick() - last_keypress > 300000)
      ssd1306_dim(1);
    // shift pixels around every 2 minutes to prevent burn-in
    if(HAL_GetTick() > next_pixel_shift)
    {
      if(has_valid_profiles)
        print_legend(rand()%3-1, rand()%3-1); // -1 to 1
      next_pixel_shift = HAL_GetTick() + 120000;
    }
    osDelay(25);
  }
}
