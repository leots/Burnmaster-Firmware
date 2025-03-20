/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/
#include <gd32f10x.h>
#include "fatfs/ff.h"
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "Common.h"
#include "Display.h"
#include "Operate.h"
#include "GB.h"
#include "GBA.h"
#include "fatfs/sdcard.h"
#include "gd32f10x_sdio.h"
#include "flashparam.h"






//PC5是卡带3V3电压档位，低电平有效，PB0是卡带5V电压档位，低电平有效

#define TYPE_GBC (0)
#define TYPE_GBA (1)
#define TYPE_ALL (2)
#define TYPE_NONE (3)

uint8_t GetGBType()
{
  uint8_t s3v3 = gpio_input_bit_get(GPIOC,GPIO_PIN_5);
  uint8_t s5v = gpio_input_bit_get(GPIOB,GPIO_PIN_0);
  uint8_t ret = TYPE_NONE;

  if(s3v3 == RESET)ret = TYPE_GBA;
  if(s5v == RESET)ret = TYPE_GBC;
  if((s3v3 | s5v) == RESET)ret = TYPE_ALL;

  return  ret;
}





//图标数据
uint8_t Icon_data_GBC[] = 
{
0x00, 0xFE, 0x06, 0xF2, 0x12, 0x12, 0x12, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0x12, 0x12, 0x32, 0xE6, 0x2E, 0x0E, 0xFC, 
0x00, 0xFF, 0x00, 0xFF, 0x18, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 
0x00, 0xFF, 0x00, 0x7F, 0x40, 0x40, 0x40, 0x5F, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x5F, 0x40, 0x60, 0x60, 0x7F, 0x00, 0xC0, 0xFF, 
0x00, 0xFF, 0x00, 0x80, 0x80, 0xE0, 0x20, 0xE0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0xE0, 0x60, 0xC0, 0x00, 0xFF, 0xFF, 
0x00, 0xFF, 0x00, 0x07, 0x02, 0x0E, 0x08, 0x0E, 0x06, 0xC3, 0xC0, 0x40, 0x00, 0x00, 0x80, 0xC0, 0x60, 0x00, 0x07, 0x05, 0x07, 0x83, 0x40, 0x61, 0xA1, 0x01, 0x00, 0x07, 0xFF, 
0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x40, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x46, 0x4B, 0x4D, 0x57, 0x59, 0x4F, 0x63, 0x31, 0x18, 0x0F
};

uint8_t Icon_data_GBA[] = 
{
0x00, 0x00, 0xC0, 0x70, 0x78, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0E, 0x0E, 0x07, 0x03, 0xF3, 0xF3, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xF3, 0x47, 0x0E, 0x6E, 0x6C, 0x0C, 0x0C, 0x0C, 0x1C, 0x1C, 0x38, 0xF8, 0xF0, 0x00, 
0xF0, 0xFF, 0x1F, 0x00, 0x78, 0x78, 0x68, 0xCF, 0xE7, 0xEF, 0x78, 0x78, 0xF8, 0xF8, 0x3F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x70, 0x78, 0x78, 0x78, 0x00, 0x3C, 0x3C, 0x3C, 0x38, 0x07, 0xFF, 0xFE, 
0x07, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x40, 0xD7, 0xDF, 0xDB, 0xF8, 0xB8, 0xB7, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0x3F, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xC0, 0x00, 0x00, 0x80, 0xFF, 0xFF, 
0x00, 0x01, 0x07, 0x07, 0x0E, 0x1C, 0x18, 0x18, 0x38, 0x30, 0x31, 0x31, 0x71, 0x61, 0x67, 0x67, 0xEE, 0xCC, 0xCE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xD8, 0xDC, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDC, 0xDE, 0xCE, 0xE6, 0xE6, 0x67, 0x77, 0x70, 0x33, 0x33, 0x3B, 0x19, 0x1D, 0x1C, 0x0C, 0x0E, 0x07, 0x03, 0x01
};



//Game Boy Menu
/******************************************
   Menu
 *****************************************/
// GBx start menu
static const char gbxMenuItem1[] = "Game Boy (Color)";
static const char gbxMenuItem2[] = "Game Boy Advance";
static const char gbxMenuTestAll[] = "CartTest";
static const char gbxMenuTestFast[] = "CartTestFast";
static const char gbxAbout[] = "About...";

static const char* const menuOptionsGBC[] = {gbxMenuItem1,gbxMenuTestFast,gbxMenuTestAll,gbxAbout};
static const char* const menuOptionsGBA[] = {gbxMenuItem2,gbxMenuTestFast,gbxMenuTestAll,gbxAbout};
static const char* const menuOptionsGBx[] = {gbxMenuItem1, gbxMenuItem2};


void aboutScreen()
{
  //
  OledClear();
  OledShowString(0,0,(char *)("Portable"),16);
  OledShowString(8,2,(char *)("Cart Flasher"),16);
  OledShowString(20,4,(char *)("Ver:1.10-leo?"),8);
  OledShowString(20,5,(char *)("2025.3"),8);
  OledShowString(20,6,(char *)("Orz Studio"),8);

  OledShowString(0,7,(char *)("Press OK Button..."),8);

  WaitOKBtn();
}


// Start menu for both GB and GBA
uint8_t gbxMenu() 
{

  //
  uint8_t bret = 0;
  uint8_t gbxtype = GetGBType();
  unsigned char gbType;

  // create menu with title and 4 options to choose from  
  // wait for user choice to come back from the question box menu

  if(gbxtype == TYPE_GBA)
  {
    //
    LED_BLUE_ON;
    OledClear();
    OledShowPicData(64,4,56,4,Icon_data_GBA);
    gbType = questionBox_OLED("Portable Cart Flasher", menuOptionsGBA, 4, 1, 1, 0);    
    switch (gbType)
    {
      case 0:
        //cancel btn clicked
        bret = 1;
        break;
      case 1:
        gbaScreen();
        break;
      case 2:
        //
        TestMemGBA(true);
        break;
      case 3:
        //
        TestMemGBA(false);
        break;
      case 4:
        aboutScreen();
        break;
    }
  }
  else if(gbxtype == TYPE_GBC)
  {
    //
    LED_GREEN_ON;
    OledClear();
    OledShowPicData(86,2,29,6,Icon_data_GBC);
    gbType = questionBox_OLED("Portable Cart Flasher", menuOptionsGBC, 4, 1, 1, 0);    
    switch (gbType)
    {
      case 0:
        //cancel btn clicked
        bret = 1;
        break;
      case 1:
        gbScreen();
        break;
      case 2:
        //
        TestMemGB(true);
        break;
      case 3:
        //
        TestMemGB(false);
        break;
      case 4:
        aboutScreen();
        break;
    }
  }
  else 
  {
    gbType = questionBox_OLED("Portable Cart Flasher", menuOptionsGBx, 2, 1, 1, 1);

    switch (gbType)
    {
      case 0:
        //cancel btn clicked
        bret = 1;
        break;
      case 1:
        gbScreen();
        break;

      case 2:
        gbaScreen();
        break;
    }
  }

  return  bret;
}


/*********************************************************************
*
Menu to display
*/
void gbxScreen()
{
  while(1)
  {
    //
    uint8_t b = gbxMenu();
    
    if(b>0)break;
  }
}


// Main menu
static const char modeItem1[] = "Game Boy";
static const char modeItem2[] = "About";
static const char modeItem3[] = "Reset";
static const char* const modeOptions[] = {modeItem1, modeItem2/*, modeItem3*/};


// All included slots
void mainMenu() {
  // create menu with title and 6 options to choose from
  unsigned char modeMenu;
  // Copy menuOptions out of progmem
  //convertPgm(modeOptions, 7);
  LED_CLEAR();
  modeMenu = questionBox_OLED("Portable Cart Flasher", modeOptions, 2, 1, 1, 1);

  // wait for user choice to come back from the question box menu
  switch (modeMenu)
  {
#ifdef enable_GBX
    case 1:
      gbxScreen();
      break;
#endif

    case 2:
      aboutScreen();
      break;

    case 3:
      //
      ResetSystem();
      break;
  }
}


/*********************************************************************
*
SD Card Init Functions
*/

/*!
    \brief      initialize the card, get the card information, set the bus mode and transfer mode
    \param[in]  none
    \param[out] none
    \retval     sd_error_enum
*/


sd_error_enum sd_io_init(void)
{
    sd_error_enum status = SD_OK;
    uint32_t cardstate = 0;
    status = sd_init();
    if(SD_OK == status){
        status = sd_card_information_get(&sd_cardinfo);
    }
    if(SD_OK == status){
        status = sd_card_select_deselect(sd_cardinfo.card_rca);
    }
    status = sd_cardstatus_get(&cardstate);
    if(cardstate & 0x02000000){
        printf("\r\n the card is locked!");
        while (1){
        }
    }
    if ((SD_OK == status) && (!(cardstate & 0x02000000)))
    {
        /* set bus mode */
        //status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
//        status = sd_bus_mode_config( SDIO_BUSMODE_1BIT );
        //printf("Set SD to 4 Bits Mode.\r\n");
    }
    if (SD_OK == status)
    {
        /* set data transfer mode */
        //status = sd_transfer_mode_config( SD_DMA_MODE );
        //printf("Set SD to DMA Mode.\r\n");

        status = sd_transfer_mode_config( SD_POLLING_MODE );
        printf("Set SD to Polling Mode.\r\n");
    }
    return status;
}

/*!
    \brief      get the card information and print it out by USRAT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void card_info_get(void)
{
    uint8_t sd_spec, sd_spec3, sd_spec4, sd_security;
    uint32_t block_count, block_size;
    uint16_t temp_ccc;
    printf("\r\n Card information:");
    sd_spec = (sd_scr[1] & 0x0F000000) >> 24;
    sd_spec3 = (sd_scr[1] & 0x00008000) >> 15;
    sd_spec4 = (sd_scr[1] & 0x00000400) >> 10;
    if(2 == sd_spec)
    {
        if(1 == sd_spec3)
        {
            if(1 == sd_spec4) 
            {
                printf("\r\n## Card version 4.xx ##");
            }
            else 
            {
                printf("\r\n## Card version 3.0x ##");
            }
        }
        else 
        {
            printf("\r\n## Card version 2.00 ##");
        }
    }
    else if(1 == sd_spec) 
    {
        printf("\r\n## Card version 1.10 ##");
    }
    else if(0 == sd_spec) 
    {
        printf("\r\n## Card version 1.0x ##");
    }
    
    sd_security = (sd_scr[1] & 0x00700000) >> 20;
    if(2 == sd_security) 
    {
        printf("\r\n## SDSC card ##");
    }
    else if(3 == sd_security) 
    {   
        printf("\r\n## SDHC card ##");
    }
    else if(4 == sd_security) 
    {
        printf("\r\n## SDXC card ##");
    }
    
    block_count = (sd_cardinfo.card_csd.c_size + 1)*1024;
    block_size = 512;
    printf("\r\n## Device size is %dKB ##", sd_card_capacity_get());
    printf("\r\n## Block size is %dB ##", block_size);
    printf("\r\n## Block count is %d ##", block_count);
    
    if(sd_cardinfo.card_csd.read_bl_partial){
        printf("\r\n## Partial blocks for read allowed ##" );
    }
    if(sd_cardinfo.card_csd.write_bl_partial){
        printf("\r\n## Partial blocks for write allowed ##" );
    }
    temp_ccc = sd_cardinfo.card_csd.ccc;
    printf("\r\n## CardCommandClasses is: %x ##", temp_ccc);
    if((SD_CCC_BLOCK_READ & temp_ccc) && (SD_CCC_BLOCK_WRITE & temp_ccc)){
        printf("\r\n## Block operation supported ##");
    }
    if(SD_CCC_ERASE & temp_ccc){
        printf("\r\n## Erase supported ##");
    }
    if(SD_CCC_WRITE_PROTECTION & temp_ccc){
        printf("\r\n## Write protection supported ##");
    }
    if(SD_CCC_LOCK_CARD & temp_ccc){
        printf("\r\n## Lock unlock supported ##");
    }
    if(SD_CCC_APPLICATION_SPECIFIC & temp_ccc){
        printf("\r\n## Application specific supported ##");
    }
    if(SD_CCC_IO_MODE & temp_ccc){
        printf("\r\n## I/O mode supported ##");
    }
    if(SD_CCC_SWITCH & temp_ccc){
        printf("\r\n## Switch function supported ##");
    }
}



void SDCardInit()
{
  //
  FRESULT res_sd;  
  //
  sd_error_enum sd_error;
  uint16_t i = 5;
  /* initialize the card */
  do{
      sd_error = sd_io_init();
  }while((SD_OK != sd_error) && (--i));
  
  if(i){
      printf("\r\nSD Card init success!\r\n");
      SD_Status = sd_error;
  }else{
      ignoreError = 0;
      //draw_progressbar(0, 100,6);
      //draw_progressbar(50, 100,6);
      print_Error("SD Card init failed!",true);
  }
  
  /* get the information of the card and print it out by USART */
  card_info_get();

  //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
  res_sd = f_mount(&fs,"",1);


  /*----------------------- 格式化测试 ---------------------------*/  
  /* 如果没有文件系统就格式化创建创建文件系统 */
  if(res_sd == FR_NO_FILESYSTEM)
  {
          printf("\r\n!No File System...");
  }
  else if(res_sd!=FR_OK)
  {
          printf("\r\n!Mount Failed(%d)",res_sd);
          while(1){};
  }
  else
  {
          printf("\r\nMount OK!%d\r\n",res_sd);
  }

}

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
void test()
{

  byte b1 = 0xff;
  printf("%08x",(b1 << 8));

  // Config led gpio
  rcu_periph_clock_enable(RCU_GPIOC);
  //gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_2MHZ,GPIO_PIN_14);
  gpio_init(GPIOC,GPIO_MODE_OUT_PP,GPIO_OSPEED_2MHZ,GPIO_PIN_13);

  while(1)
  {
    //
    //delay(250);
    //LED_BLINK(LED3);
    //delay(250);
    //LED_BLINK(LED3);
    //delay(250);
    //LED_BLINK(LED2);
    //delay(250);
    //LED_BLINK(LED2);
  }
}

void PriInit()
{
  //
  SysClockInit();
  //
  __enable_irq();
  
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_GPIOC);
  rcu_periph_clock_enable(RCU_GPIOD);
  rcu_periph_clock_enable(RCU_GPIOE);

  gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
  gpio_init(GPIOB,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
  gpio_init(GPIOC,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
  gpio_init(GPIOD,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
  gpio_init(GPIOE,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);



  rcu_periph_clock_enable(RCU_AF);
  //gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);
  foldern = load_dword();
}

int main(void) 
{
  //  
  PriInit();
  //
  LEDSInit();
  //test();
  OledInit();
  OledClear();
  KeyBrdInit();
  SDCardInit();


  //fileBrowser("/","test");
  //printf("\n\nselect:%s",filePath);
  

  //
  //while(1)
  //{
  //   //
  //   mainMenu();
  //}

  delay(200);
  gbxScreen();

  //
  f_mount(NULL,"",1);
  SysClockFree();
  exit(EXIT_SUCCESS);
}

/*************************** End of file ****************************/
