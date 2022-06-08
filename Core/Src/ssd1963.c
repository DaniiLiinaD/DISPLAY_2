/********************************************/
/* ssd1963.c                                */
/* Driver for SSD1963 display controller    */
/* MCU: STM32                               */
/* Connection FMC (i8080)                   */
/* Based on: Radoslaw Kwiecien, radek@dxp.pl*/
/* http://en.radzio.dxp.pl/ssd1963/         */
/* Tested on: WF57QTIBCDBN0                 */
/* DaniiLiinaD 8.06.2022                    */
/********************************************/

#include "ssd1963.h"

void SSD1963_WriteCommand(uint16_t command)
{
  *(uint16_t *) (SSD1963_COMMAND_ADR) = command;
}

void SSD1963_WriteData(uint16_t data)
{
  *(uint16_t *) (SSD1963_DATA_ADR)= data;
}

uint16_t SSD1963_ReadData(void) // !! NOT TESTED !! 
{
  uint16_t data = * (uint16_t *)(SSD1963_DATA_ADR);
  return data;
}

void SSD1963_Init(void)
{
  HAL_GPIO_WritePin(SSD1963_RESET_GPIO_Port, SSD1963_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(SSD1963_RESET_GPIO_Port, SSD1963_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_EXIT_SLEEP_MODE);
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_SOFT_RESET);
  HAL_Delay(10);

  SSD1963_WriteCommand(SSD1963_SET_PLL_MN);
  SSD1963_WriteData(0x1D);  // M=29 // PLLclk = REFclk * 50 (500MHz)
  SSD1963_WriteData(0x02);  // N=2 // SYSclk = PLLclk / 5  (100MHz)
  SSD1963_WriteData(0x54);  // dummy

  SSD1963_WriteCommand(SSD1963_SET_PLL);
  SSD1963_WriteData(0x01);
  HAL_Delay(100);

  SSD1963_WriteCommand(SSD1963_SET_PLL);
  SSD1963_WriteData(0x03);
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_SOFT_RESET);
 
  SSD1963_WriteCommand(SSD1963_SET_LCD_MODE); 
  SSD1963_WriteData(0x24);   
  SSD1963_WriteData(0x20);      
  SSD1963_WriteData(mHIGH((TFT_WIDTH-1)));
  SSD1963_WriteData(mLOW((TFT_WIDTH-1)));
  SSD1963_WriteData(mHIGH((TFT_HEIGHT-1)));
  SSD1963_WriteData(mLOW((TFT_HEIGHT-1)));
  SSD1963_WriteData(0x00);

  SSD1963_WriteCommand(SSD1963_SET_PIXEL_DATA_INTERFACE);
  SSD1963_WriteData(SSD1963_PDI_16BIT565);

  SSD1963_WriteCommand(SSD1963_SET_LSHIFT_FREQ); 
  SSD1963_WriteData((LCD_FPR >> 16) & 0xFF);
  SSD1963_WriteData((LCD_FPR >> 8) & 0xFF);
  SSD1963_WriteData(LCD_FPR & 0xFF);
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_SET_HORI_PERIOD);
  SSD1963_WriteData(mHIGH(TFT_HSYNC_PERIOD));
  SSD1963_WriteData(mLOW(TFT_HSYNC_PERIOD));
  SSD1963_WriteData(mHIGH((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
  SSD1963_WriteData(mLOW((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
  SSD1963_WriteData(TFT_HSYNC_PULSE);
  SSD1963_WriteData(0x00);      
  SSD1963_WriteData(0x00);
  SSD1963_WriteData(0x00);      
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_SET_VERT_PERIOD);    
  SSD1963_WriteData(mHIGH(TFT_VSYNC_PERIOD));
  SSD1963_WriteData(mLOW(TFT_VSYNC_PERIOD));
  SSD1963_WriteData(mHIGH((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
  SSD1963_WriteData(mLOW((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
  SSD1963_WriteData(TFT_VSYNC_PULSE);
  SSD1963_WriteData(0x00);      
  SSD1963_WriteData(0x00);
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_SET_COLUMN_ADDRESS); //0x2a
  SSD1963_WriteData(0x00);
  SSD1963_WriteData(0x00);
  SSD1963_WriteData(mHIGH((TFT_WIDTH-1)));
  SSD1963_WriteData(mLOW((TFT_WIDTH-1)));

  SSD1963_WriteCommand(SSD1963_SET_PAGE_ADDRESS); //0x2b
  SSD1963_WriteData(0x00);
  SSD1963_WriteData(0x00);
  SSD1963_WriteData(mHIGH((TFT_HEIGHT-1)));
  SSD1963_WriteData(mLOW((TFT_HEIGHT-1)));

  SSD1963_WriteCommand(SSD1963_SET_GPIO_CONF); //0xb8
  SSD1963_WriteData(0x0f);
  SSD1963_WriteData(0x01);

  SSD1963_WriteCommand(SSD1963_SET_GPIO_VALUE); //0xba
  SSD1963_WriteData(0x01);

  SSD1963_WriteCommand(SSD1963_SET_DISPLAY_ON); //0x29
  SSD1963_WriteCommand(SSD1963_WRITE_MEMORY_START); // 0x2c
  HAL_Delay(50);
}

void SSD1963_SetArea(uint16_t sx, uint16_t ex, uint16_t sy, uint16_t ey)
{
  SSD1963_WriteCommand(SSD1963_SET_COLUMN_ADDRESS); 
  SSD1963_WriteData((sx >> 8) & 0xFF);
  SSD1963_WriteData((sx >> 0) & 0xFF);
  SSD1963_WriteData((ex >> 8) & 0xFF);
  SSD1963_WriteData((ex >> 0) & 0xFF);
  HAL_Delay(50);

  SSD1963_WriteCommand(SSD1963_SET_PAGE_ADDRESS); 
  SSD1963_WriteData((sy >> 8) & 0xFF);
  SSD1963_WriteData((sy >> 0) & 0xFF);
  SSD1963_WriteData((ey >> 8) & 0xFF);
  SSD1963_WriteData((ey >> 0) & 0xFF);
}

void SSD1963_ClearScreen(uint16_t color)
{
  uint16_t x,y;
  SSD1963_SetArea(0, TFT_WIDTH-1 , 0, TFT_HEIGHT-1);
  SSD1963_WriteCommand(0x2c);
    for(x=0;x<TFT_WIDTH;x++)
    {
      for(y= 0;y<TFT_HEIGHT;y++)
      {
        SSD1963_WriteData(color);
      }
  }
}

void SSD1963_SetPixel(uint16_t x, uint16_t y, uint16_t color)
{
  SSD1963_SetArea(x, x, y, y);
  SSD1963_WriteCommand(0x2c);
  SSD1963_WriteData(color);
  SSD1963_WriteCommand(0x0);
}