/*******************************************************
Description: This software is of a digital water quality sensor 
which measures water turbidity, temperature, pH and dissolved oxygen.
********************************************************/
//Json
#include <ArduinoJson.h>
//LCD Shield Library
#include <LiquidCrystal.h>
//Temperature Sensor Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
//Headers
#include "LcdKeypad.h"
#include "MenuData.h"
#include "temperature.h"
#include "turbidity.h"
#include "pH.h"
#include "dissolved_oxygen.h"

enum AppModeValues
{
  APP_NORMAL_MODE,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD
};

byte appMode = APP_NORMAL_MODE;

MenuManager Menu1(sampleMenu_Root, menuCount(sampleMenu_Root));

char strbuf[LCD_COLS + 1]; // one line of lcd display
String emptyLine = "                ";
byte btn;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);



void refreshMenuDisplay (byte refreshMode);
byte getNavAction();


void setup()
{
  backLightOn();
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  bootUpMessage();
  // fall in to menu mode by default.
  appMode = APP_MENU_MODE;
  refreshMenuDisplay(REFRESH_DESCEND);
  Serial.begin(9600);
  Serial1.begin(9600);
  //DOxygen
  pinMode(DoSensorPin,INPUT);
  readDoCharacteristicValues();      //read Characteristic Values calibrated from the EEPROM
}
static unsigned long dataUpInterval = 4000;
static unsigned long lastDataUp = millis();
char datapacket[100];
char str_temp[10];
char str_turb[10];
char str_ph[10];
char str_doxy[10];


void loop()
{
  if(millis()-lastDataUp >= dataUpInterval)
  {
    float temp = get_temp_in_celsius();
    float turb = get_turbidity_NTU();
    float ph = get_pH();
    float doxy = get_dissolved_oxygen();
    
    DynamicJsonDocument doc(1024);

    doc["Temperature"] = temp;
    doc["Turbidity"]   = turb;
    doc["pH"] = ph;
    doc["DO"] = doxy;

    char jsonBuffer[1024];
    serializeJson(doc, jsonBuffer);
    Serial1.println(jsonBuffer);

    
    lastDataUp = millis();
  }
  
btn = getButton();

  switch (appMode)
  {
    case APP_NORMAL_MODE :
      if (btn == BUTTON_UP_LONG_PRESSED)
      {
        appMode = APP_MENU_MODE;
        lcd.setCursor(0, 1);
        lcd.print(emptyLine);
        refreshMenuDisplay(REFRESH_DESCEND);
      }
      break;
    case APP_MENU_MODE :
    {
      byte menuMode = Menu1.handleNavigation(getNavAction, refreshMenuDisplay);

      if (menuMode == MENU_EXIT)
      {
        lcd.clear();
        lcd.print("Water Quality");
        lcd.setCursor(0, 1);
        lcd.print("Hold UP for Menu");
        appMode = APP_NORMAL_MODE;
      }
      else if (menuMode == MENU_INVOKE_ITEM)
      {
        appMode = APP_PROCESS_MENU_CMD;

        // Indicate selected item.
        if (Menu1.getCurrentItemCmdId())
        {
          lcd.setCursor(0, 1);
          strbuf[0] = 0b01111110; // forward arrow representing input prompt.
          strbuf[1] = 0;
          lcd.print(strbuf);
      
        }
      }
      break;
    }
    case APP_PROCESS_MENU_CMD :
    {
      
      
      byte processingComplete = processMenuCommand(Menu1.getCurrentItemCmdId());

      if (processingComplete)
      {
        appMode = APP_MENU_MODE;
        // clear forward arrow
        lcd.setCursor(0, 1);
        strbuf[0] = ' '; // clear forward arrow
        strbuf[1] = 0;
        lcd.print(strbuf);
        lcd.print(emptyLine);
      }
      break;
    }
  }

}

//----------------------------------------------------------------------
// Addition or removal of menu items in MenuData.h will require this method
// to be modified accordingly. 
byte processMenuCommand(byte cmdId)
{
  byte complete = false;  // set to true when menu command processing complete.

  if (btn == BUTTON_SELECT_PRESSED)
  {
    complete = true;
  }

  switch (cmdId)
  {
    // TODO Process menu commands here:
        case 1:
         // Serial.println("Temperature");
          lcd.setCursor(2, 1);
          lcd.print(get_temp_in_celsius());
          lcd.setCursor(8, 1);
          lcd.print(" C");
        break;
        case 2:
        //  Serial.println("pH");
          lcd.setCursor(2, 1);
          lcd.print("pH: ");
          lcd.print(get_pH());
          
          
        break;
        case 3:
          //Serial.println("Turbidity");
          lcd.setCursor(2, 1);
          lcd.print(get_turbidity_NTU());
          lcd.setCursor(9, 1);
          lcd.print(" NTU");
        break;
        case 4:
          //Serial.println("Diss. Oxygen");
          lcd.setCursor(2, 1);
          lcd.print(get_dissolved_oxygen());
          lcd.setCursor(9, 1);
          lcd.print(" mg/L");
        break;
      default:
    break;
  }

  return complete;
}


//----------------------------------------------------------------------
// Callback to convert button press to navigation action.
byte getNavAction()
{
  byte navAction = 0;
  byte currentItemHasChildren = Menu1.currentItemHasChildren();
  
  if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) navAction = MENU_ITEM_PREV;
  else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) navAction = MENU_ITEM_NEXT;
  else if (btn == BUTTON_SELECT_PRESSED || (btn == BUTTON_RIGHT_PRESSED && currentItemHasChildren)) navAction = MENU_ITEM_SELECT;
  //else if (btn == BUTTON_LEFT_PRESSED) navAction = MENU_BACK;
  return navAction;
}


//----------------------------------------------------------------------
const char EmptyStr[] = "";

// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
void refreshMenuDisplay (byte refreshMode)
{
  char nameBuf[LCD_COLS+1];
  


  lcd.setCursor(0, 0);
  if (Menu1.currentItemHasChildren())
  {
    rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
    strbuf[LCD_COLS-1] = 0b01111110;            // Display forward arrow if this menu item has children.
    lcd.print(strbuf);
    lcd.setCursor(0, 1);
    lcd.print(rpad(strbuf, EmptyStr));          // Clear config value in display
  }
  else
  {
    byte cmdId;
    rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
    
    if ((cmdId = Menu1.getCurrentItemCmdId()) == 0)
    {
      strbuf[LCD_COLS-1] = 0b01111111;          // Display back arrow if this menu item ascends to parent.
      lcd.print(strbuf);
      lcd.setCursor(0, 1);
      lcd.print(rpad(strbuf, EmptyStr));        // Clear config value in display.
    }
    else
    {
      lcd.print(strbuf);
      lcd.setCursor(0, 1);
      lcd.print(" ");
     // Serial.println(cmdId);
      switch(cmdId)
      {
        case 1:
        //  Serial.println("Temperature");
          
        break;
        case 2:
         // Serial.println("pH");
        break;
        case 3:
          //Serial.println("Turbidity");
        break;
        case 4:
         // Serial.println("Diss. Oxygen");
        break;
        
      }
      // TODO Display config value.
    }
  }
}

void bootUpMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("Water Quality "); // print a simple message
  lcd.setCursor(0, 1);
  lcd.print("Sensor v1.0 ");
  delay(2000);
  lcd.clear();
}
