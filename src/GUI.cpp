#include "AutoFuzzer.h"
#include "EEPROM.h"

// GUIElement --------------------------------------------------------------------------------------------------------------------------------------------------------

GUIElement::GUIElement(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber)
{
    this->tft = tft;
    this->touch = touch;
    this->X = x;
    this->Y = y;
    this->Width = width;
    this->Height = height;
    this->ScreenNumber = screenNumber;
}

// GUILabel --------------------------------------------------------------------------------------------------------------------------------------------------------
GUILabel::GUILabel(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t screenNumber, String text, uint8_t size, uint16_t color, bool autoCenter, void (*ClickHandler)(uint8_t imageCode), uint8_t callBackCode, String** lines, uint16_t lineCount, uint16_t currPos)
    :GUIElement(tft, touch, X, Y, Width, Height, screenNumber)
{
    this->Text = text;
    this->Lines = lines;
    this->LineCount = lineCount;
    this->Size = size;  
    this->Color = color;
    this->AutoCenter = autoCenter;
    this->ClickHandler = ClickHandler;
    this->callBackCode = callBackCode;     
    this->CurrPos = currPos;
}

void GUILabel::Run(TS_Point* clickPoint)
{ 
    if (clickPoint != NULL)
    { // Handle Click  
        if (this->clickStartPoint == NULL) // Click Starting
        {
            this->clickStartPoint = new TS_Point();
            this->clickStartPoint->x = clickPoint->x;
            this->clickStartPoint->y = clickPoint->y;
            this->clickStartPoint->z = clickPoint->z;
            this->lastMovement = millis();            
        }
        else
        { // Click continuing -- Scrolled                
            if (millis() - this->lastMovement >= this->MovementSpeed) // TO DO NEEDS TWEAKING
            {
                this->needsRedrawing = true;
                if (this->clickStartPoint->y > clickPoint->y) 
                {
                    if (this->CurrPos > 0) this->CurrPos--; 
                }
                else if(!((this->CurrPos+(this->Height/9)) >= this->LineCount))
                {
                    this->CurrPos++; // ADD VALIDATION
                }
                this->lastMovement = millis(); 
                
            }
        }
    }
    
    else
    {
        if (this->clickStartPoint) 
        { 
            delete this->clickStartPoint;
            this->clickStartPoint = NULL;
            this->needsRedrawing = false;
            if (this->ClickHandler) this->ClickHandler(this->callBackCode);
        }
    }
    if (this->needsRedrawing)
    {
        this->tft->fillRect(this->X, this->Y, this->Width, this->Height, ILI9341_BLACK);
        this->tft->setTextColor(this->Color, 0xFFFF); // 5x8
        this->tft->setTextSize(this->Size);
        uint16_t x = this->X;
        if(this->LineCount > 0)
        {
             uint16_t y = this->Y;
             for(uint16_t i = this->CurrPos; i < this->LineCount && ((i - this->CurrPos + 1) * 9 < this->Height); i++) 
             { 
                  if (this->AutoCenter) x = this->X + ((this->Width - this->X - (this->Lines[i]->length() * 6 * this->Size)) / 2);
                  for(uint8_t j = 0; j < this->Lines[i]->length(); j++) this->tft->drawChar(x + (j * 6 * this->Size), y, this->Lines[i][0][j], this->Color, 0, this->Size);  
                  y += 9;              
             }
        }
        else
        {
            if (this->AutoCenter) x = this->X + ((this->Width - this->X - (this->Text.length() * 6 * this->Size)) / 2);
            for(uint8_t i = 0; i < this->Text.length(); i++) this->tft->drawChar(x + (i * 6 * this->Size), this->Y, this->Text[i], this->Color, 0, this->Size);
        }        
        this->needsRedrawing = false;
    }
}

// GUIImage --------------------------------------------------------------------------------------------------------------------------------------------------------
GUIImage::GUIImage(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t screenNumber, const uint8_t* Image, uint16_t ImageLength, void (*ClickHandler)(uint8_t imageCode), uint8_t callBackCode)
    :GUIElement(tft, touch, X, Y, Width, Height, screenNumber)
{
    this->Image = Image;
    this->ImageLength = ImageLength;
    this->ClickHandler = ClickHandler;
    this->callBackCode = callBackCode;
}

void GUIImage::Run(TS_Point* clickPoint)
{
    if (this->ClickHandler != NULL)
        {
        if (clickPoint == NULL)
        {
            if (this->clickInProgress) 
            {
                this->clickInProgress = false;
                this->needsRedrawing = true;
            }
        }
        else
        {   
            if (!this->clickInProgress)
            {     
                this->ClickHandler(this->callBackCode);
                this->clickInProgress = true;
                this->needsRedrawing = true;
                this->imageClicked = !this->imageClicked;
            }        
        }
    }
    
    if (this->needsRedrawing)
    {
        if(!this->imageClicked) 
        {
        uint32_t pos = 0;
        for(uint16_t y = 0; y < this->Height; y++)
            for(uint16_t x = 0; x < this->Width; x++)
            {
                uint16_t color = pgm_read_byte(this->Image + pos++) << 8 | pgm_read_byte(this->Image + pos++);
                tft->drawPixel((X+x), (Y+y), color); 
            }
        this->needsRedrawing = false;
        } 
        else if (this->inClickHandler)
        {
          this->tft->fillRect(this->X,this->Y,this->Width,this->Height,ILI9341_BLACK);
          this->needsRedrawing = false;
        }
        else
        {
        uint32_t pos = 0;
        for(uint16_t y = 0; y < this->Height; y++)
            for(uint16_t x = 0; x < this->Width; x++)
            {
                uint16_t color = pgm_read_byte(this->Image + pos++) << 8 | pgm_read_byte(this->Image + pos++);
                uint16_t grayscale = turnToGrayScale(color);
                tft->drawPixel((X+x), (Y+y), grayscale); 
            }
        this->needsRedrawing = false;
        }
    }
}

// GUIGauge --------------------------------------------------------------------------------------------------------------------------------------------------------

GUIGauge::GUIGauge(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Radius, uint16_t screenNumber, uint16_t DegreesStart, uint16_t DegreesStop, bool DrawCompleteCircle, uint16_t BackgroundColor, uint16_t ForegroundColor, uint16_t MinValue, uint16_t MaxValue, uint16_t Value, bool IsTransparent)
    :GUIElement(tft, touch, X, Y, Radius * 2, Radius * 2, screenNumber)
{
    this->Radius = Radius;
    this->DegreesStart = DegreesStart;
    this->DegreesStop = DegreesStop;
    this->BackgroundColor = BackgroundColor;
    this->ForegroundColor = ForegroundColor;
    this->MinValue = MinValue;
    this->MaxValue = MaxValue;
    this->Value = Value;
    this->IsTransparent = IsTransparent;
    this->DrawCompleteCircle = DrawCompleteCircle;
}

void GUIGauge::Run(TS_Point* clickPoint)
{
    
    if (clickPoint != NULL)
    { // Handle Click
      //  Serial.println("Touch Gauge at " + String(clickPoint->x) + ", " + String(clickPoint->y) + ", Pressure " + String(clickPoint->z));
        if (this->clickStartPoint == NULL) // Click Starting
        {
            this->clickStartPoint = new TS_Point();
            this->clickStartPoint->x = clickPoint->x;
            this->clickStartPoint->y = clickPoint->y;
            this->clickStartPoint->z = clickPoint->z;
            this->lastMovement = millis();            
        }
        else
        { // Click continuing
            if (millis() - this->lastMovement >= this->MovementSpeed)
            {
                if (clickPoint->y < this->clickStartPoint->y - 10 && this->Value < this->MaxValue) { this->Value++; this->needsRedrawing = true; }
                if (clickPoint->y > this->clickStartPoint->y + 10 && this->Value > this->MinValue) { this->Value--; this->needsRedrawing = true; }                
                this->lastMovement = millis(); 
                Serial.println("Value Changed to " + String(this->Value));
            }
        }
    }
    else
    {
        if (this->clickStartPoint) 
        { 
            delete this->clickStartPoint;
            this->clickStartPoint = NULL;
        }
    }
    if (this->needsRedrawing)
    {
        uint16_t centerX = this->X + this->Radius;
        uint16_t centerY = this->Y + this->Radius;
        if (!this->IsTransparent)
        {
            if (this->DrawCompleteCircle)
                this->tft->fillCircle(centerX, centerY, this->Radius, this->BackgroundColor);       
            else
                fillArc(centerX, centerY, this->DegreesStart + 90, this->DegreesStop + 90, this->Radius, this->Radius, this->Radius, this->BackgroundColor);
        }
        float angle = this->Value / (float) (this->MaxValue - this->MinValue);
        angle = ((this->DegreesStop - this->DegreesStart) * angle) + this->DegreesStart;       
        angle = angle * PI / 180;
        uint16_t x = centerX + ((this->Radius - 3) * cos(angle));
        uint16_t y = centerY + ((this->Radius - 3) * sin(angle));
        this->tft->drawLine(centerX, centerY, x, y, this->ForegroundColor);
        this->needsRedrawing = false;
    }
}
// GUICheckBox ------------------------------------------------------------------------------------------------------------------------------------------------
GUICheckBox::GUICheckBox(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, void (*ClickHandler)(uint8_t boxCode), uint8_t boxCode)
  :GUIElement(tft, touch, x, y, width, height, screenNumber)
{
    this->ClickHandler = ClickHandler;
    this->boxCode = boxCode;
}
void GUICheckBox::Run(TS_Point* clickPoint)
{
  if (clickPoint == NULL)
    {
        if (this->clickInProgress) 
        {
            this->clickInProgress = false;
            this->needsRedrawing = true;
        }
    }
    else
    {   
        if (!this->clickInProgress)
        {    
            this->ClickHandler(this->boxCode);
            this->clickInProgress = true;
            this->needsRedrawing = true;
            this->BoxClickedInProgress = !(this->BoxClickedInProgress);
        }        
    }
  if(this->needsRedrawing)
  {
    if(!this->BoxClickedInProgress)
    {
      this->tft->fillRect(this->X,this->Y,this->Width,this->Height,ILI9341_BLACK);
      this->tft->drawRect(this->X,this->Y,this->Width,this->Height,ILI9341_WHITE);
      this->needsRedrawing = false;
    }
    else if(this->inClickHandler)
    {
      this->tft->fillRect(this->X,this->Y,this->Width,this->Height,ILI9341_BLACK);
    }
    else 
    {
      this->tft->fillRect(this->X,this->Y,this->Width,this->Height,ILI9341_WHITE);
      this->needsRedrawing = false;
    }
  }
}
// GUINumScroll -----------------------------------------------------------------------------------------------------------------------------------------------
GUINumScroll::GUINumScroll(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, uint8_t currNum)
  :GUIElement(tft, touch, x, y, width, height, screenNumber)
{
  this->CurrNum = currNum;
}

void GUINumScroll::Run(TS_Point* clickPoint)
{
  if (clickPoint != NULL)
    { // Handle Click  
        if (this->clickStartPoint == NULL) // Click Starting
        {
            this->clickStartPoint = new TS_Point();
            this->clickStartPoint->x = clickPoint->x;
            this->clickStartPoint->y = clickPoint->y;
            this->clickStartPoint->z = clickPoint->z;
            this->lastMovement = millis();            
        }
        else
        { // Click continuing -- Scrolled                
            if (millis() - this->lastMovement >= this->MovementSpeed) 
            {
                this->needsRedrawing = true;
                if (this->clickStartPoint->y < clickPoint->y) 
                {
                    if (this->CurrNum > 0) this->CurrNum--; 
                }
                else if(!(this->CurrNum == 9))
                {
                    this->CurrNum++; // ADD VALIDATION
                }
                this->lastMovement = millis(); 
                
            }
        }
    }
    else
    {
        if (this->clickStartPoint) 
        { 
            delete this->clickStartPoint;
            this->clickStartPoint = NULL;
            this->needsRedrawing = false;
        }
    }
  if(this->needsRedrawing)
  {
      this->numString = String(this->CurrNum);
      for(int i=0;i<this->numString.length();i++)this->tft->drawChar(this->X, this->Y, this->numString[i], ILI9341_WHITE, 0, 2);
      this->needsRedrawing = false;  
  }
}
// GUI --------------------------------------------------------------------------------------------------------------------------------------------------------

GUI::GUI()
{    
    this->page = 'S';
    this->tft = new Adafruit_ILI9341(D8, D0);
    this->tft->begin();
    this->tft->setRotation(1);
    this->touch = new XPT2046_Touchscreen(D2);
    this->touch->begin();
    this->touch->setRotation(3);    
    this->drawScreen(); 
    EEPROM.begin(4096);
    uint16_t pos = 0;
    if (EEPROM.read(pos++) == 0xAB && EEPROM.read(pos++) == 0xCD)
    {
        vi1 = (EEPROM.read(pos++) << 8) | EEPROM.read(pos++);
        vj1 = (EEPROM.read(pos++) << 8) | EEPROM.read(pos++);
        vi2 = (EEPROM.read(pos++) << 8) | EEPROM.read(pos++);
        vj2 = (EEPROM.read(pos++) << 8) | EEPROM.read(pos++);      
    }   
    EEPROM.end();
}

GUI::~GUI()
{  
}

void GUI::calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj) 
{  
    this->tft->drawFastHLine(x - 8, y, 16, ILI9341_WHITE);
    this->tft->drawFastVLine(x, y - 8, 16, ILI9341_WHITE);
    while (!this->touch->touched()) { delay(100); yield(); }
    TS_Point p = this->touch->getPoint();    
    vi = p.x;
    vj = p.y;    
    this->tft->drawFastHLine(x - 8, y, 16, ILI9341_BLACK);
    this->tft->drawFastVLine(x, y - 8, 16, ILI9341_BLACK);
    while (this->touch->touched()) { delay(100); yield(); } 
}

void GUI::applyCalibration(TS_Point &p)
{
    uint16_t dx = vi2 - vi1;
    uint16_t dy = vj2 - vj1;
    if (dx == 0) dx = 1;
    if (dy == 0) dy = 1;
    p.x = this->tft->width() * (p.x - vi1) / dx + CAL_MARGIN;
    p.y = this->tft->height() * (p.y - vj1) / dy + CAL_MARGIN;
    if (p.x < 0) p.x = 0;
    if (p.y < 0) p.y = 0;
    if (p.x >= this->tft->width()) p.x = this->tft->width() - 1;
    if (p.y >= this->tft->height()) p.x = this->tft->height() - 1;
}

void GUI::calibrateTouchScreen()
{
    uint16_t x1, y1, x2, y2;
    x1 = y1 = CAL_MARGIN;
    x2 = this->tft->width() - CAL_MARGIN;
    y2 = this->tft->height() - CAL_MARGIN;
    this->tft->fillScreen(ILI9341_BLACK);
    this->calibratePoint(x1, y1, vi1, vj1);
    delay(1000);
    this->calibratePoint(x2, y2, vi2, vj2);
    uint16_t pos = 0;
    EEPROM.begin(4096);
    EEPROM.write(pos++, 0xAB);
    EEPROM.write(pos++, 0xCD);
    EEPROM.write(pos++, vi1 >> 8);
    EEPROM.write(pos++, vi1 & 0xFF);
    EEPROM.write(pos++, vj1 >> 8);
    EEPROM.write(pos++, vj1 & 0xFF);
    EEPROM.write(pos++, vi2 >> 8);
    EEPROM.write(pos++, vi2 & 0xFF);
    EEPROM.write(pos++, vj2 >> 8);
    EEPROM.write(pos++, vj2 & 0xFF);
    EEPROM.commit();
    EEPROM.end();
}

void GUI::RegisterElement(GUIElement* element)
{
    this->elementCount++;
    if (this->elementCount == 1)
        this->elements = (GUIElement**) malloc(sizeof(GUIElement*) * this->elementCount);
    else
        this->elements = (GUIElement**) realloc(this->elements, sizeof(GUIElement*) * this->elementCount);
    this->elements[this->elementCount - 1] = element;
}

void GUI::Run()
{
    if (vi1 == 65535) calibrateTouchScreen();
    TS_Point* point = NULL;
    if (this->touch->touched())
    {
        TS_Point p = this->touch->getPoint();
        point = &p;
        applyCalibration(point[0]);
        //Serial.println("Touch at " + String(p.x) + ", " + String(p.y) + ", Pressure " + String(p.z));
        //this->tft->drawPixel(p.x, p.y, ILI9341_WHITE);
    }  
    for(uint16_t i = 0; i < this->elementCount; i++)
        if(this->elements[i]->ScreenNumber == 0 || this->elements[i]->ScreenNumber == this->ScreenNumber)
        {
            TS_Point* elementPoint = NULL;
            if(point != NULL && point->x >= this->elements[i]->X && point->x <= this->elements[i]->X + this->elements[i]->Width && point->y >= this->elements[i]->Y && point->y <= this->elements[i]->Y + this->elements[i]->Height)
            {
                elementPoint = new TS_Point();
                elementPoint->x = point->x - this->elements[i]->X;
                elementPoint->y = point->y - this->elements[i]->Y;
                elementPoint->z = point->z;
            }
            this->elements[i]->Run(elementPoint);        
            if (elementPoint) delete elementPoint;
        }
}

void GUI::drawScreen()
{ 
    tft->fillScreen(ILI9341_BLACK);
    // Draw your screen controls here
}

// ------------------------------------------------------------------------ GRAPHICS ROUTINES ------------------------------------------------------------------

void GUIElement::fillArc(uint16_t x, uint16_t y, uint16_t start_angle, uint16_t end_angle, uint16_t radiusX, uint16_t radiusY, uint16_t width, uint16_t colour)
{
    #define DEG2RAD 0.0174532925
    uint8_t seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    uint8_t inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring
    uint16_t seg_count = (end_angle - start_angle) / 3;

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle - 90) * DEG2RAD);
    float sy = sin((start_angle - 90) * DEG2RAD);
    uint16_t x0 = sx * (radiusX - width) + x;
    uint16_t y0 = sy * (radiusY - width) + y;
    uint16_t x1 = sx * radiusX + x;
    uint16_t y1 = sy * radiusY + y;

  // Draw colour blocks every inc degrees
  for (uint16_t i = start_angle; i < start_angle + seg * seg_count; i += inc) {

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG2RAD);
    float sy2 = sin((i + seg - 90) * DEG2RAD);
    uint16_t x2 = sx2 * (radiusX - width) + x;
    uint16_t y2 = sy2 * (radiusY - width) + y;
    uint16_t x3 = sx2 * radiusX + x;
    uint16_t y3 = sy2 * radiusY + y;

    this->tft->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    this->tft->fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}

uint16_t GUIElement::turnToGrayScale(uint16_t color)
{
   uint32_t sum = ((color & 0b1111100000000000) >> 11) * 255 / 31;
   sum += ((color & 0b0000011111100000) >> 5) * 255 / 63;
   sum += (color & 0b0000000000011111) * 255 / 31;    
   sum /= 3;  
   return ((sum & 0xF8) << 8) | ((sum & 0xFC) << 3) | (sum >> 3);
}
