/***********************************************************************************************************************
* File Name    : Wifi_API.c
* Version      : V1.00 [26 Aug 2014]
* Device(s)    : FRDM-KL25Z
* Tool-Chain   : uVision KL25z
* Description  : This file implements Wifi APIs for GS1011Mx in Simple SPI
								 Currently sets up Auto Connection in TCP Server mode by connecting to 
								 SSID: wifidemo ( No security); DHCP
								 Port: 8010
* Creation Date: 7/24/2014
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"
#include "sim_aj.h"
#include "spi_aj.h"

#define IP 1
#define CLIENT 2

#define DEBOUNCEDELAY 10000 
#define TIMEOUT 1000

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

uint8_t Response_handle(void);           // Handle Basic Repsonse (OK or ERROR) 
void Data_handle(uint8_t);           // Handle Data Response
void IP_extract(uint8_t *,uint8_t);
extern void SPI_CS_Start(SPI_perif_t );  // Enable Chipselect for Device 
extern void SPI_CS_End(SPI_perif_t );    // Disable Chipselect for Device
void Disp_Client_Data(void);
uint8_t Parse_Data(unsigned char);           // Parse Client Messages
void Format_Data(char,uint8_t*,uint8_t*);  // Format Data to display on //LCD
uint8_t WIFI_SendString(char buffer[]);
void WIFI_idle (void);
void WIFI_wake (void);
void sub_printf(char*,...);                // sprintf substitute 

char outbuf[32];                    // Common String For Commands
char connection_id;
extern long int tick;
extern int overflow;
extern uint8_t Switch1;
extern uint8_t WIFI_ReceiveByte(unsigned char*);

/***********************************************************************************************************************
* Function Name: sub_printf
* Description  : Substitute sprintf 
* Arguments    : Character pointer (buf)
* Return Value : none
***********************************************************************************************************************/

void sub_printf (char *buf,...)
{
    va_list marker;
    va_start(marker, buf);
    vsprintf(outbuf, buf, marker);
    va_end(marker);
}

/***********************************************************************************************************************
* Function Name: sub_buffer_printf
* Description  : Substitute sprintf 
* Arguments    : Character pointer (buf)
* Return Value : none
***********************************************************************************************************************/

void sub_buffer_printf (char buffer[],char *buf,...)
{
    va_list marker;
    va_start(marker, buf);
    vsprintf(buffer, buf, marker);
    va_end(marker);
}

/***********************************************************************************************************************
* Function Name: Echo_Disable
* Description  : Disable Echo (Command ATE0)
* Arguments    : None
* Return Value : Boolean
***********************************************************************************************************************/

uint8_t Echo_Disable()
{
  int i = 0;
  sub_printf("\r\nATE0\r\n");
  while(outbuf[i] != '\0')
  {
    SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
    i++;
  }
 if(!(Response_handle()))
      {   
          return(0);      
      } 
return(1);
} 

/***********************************************************************************************************************
* Function Name: Auto_Renable
* Description  : Disable Echo (Command ATE0)
* Arguments    : None
* Return Value : Boolean
***********************************************************************************************************************/

uint8_t Auto_Renable()
{
  int i = 0;
  sub_printf("ATO\r\n");
  while(outbuf[i] != '\0')
  {
    SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
    i++;
  }
 if(!(Response_handle()))
      {   
          return(0);      
      } 
return(1);
} 

/***********************************************************************************************************************
* Function Name: Verbose_Enable
* Description  : Enable Verbose (Command ATV1)
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/


uint8_t Verbose_Enable()
{
  int i = 0;
  sub_printf("ATV1\r\n");
  while(outbuf[i] != '\0')
  {
    SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
    i++;
  }
  if(!(Response_handle()))
      {   
          return(0);      
      } 
return(1);
} 

/***********************************************************************************************************************
* Function Name: Echo_Confirm
* Description  : Confirm Response (Command AT)
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Echo_Confirm()
{
  int i = 0;
  sub_printf("AT\r\n");
  while(outbuf[i] != '\0')
  {
   SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
   i++;
  } 
  if(!(Response_handle()))
      {   
          return(0);      
      } 
return(1);
} 

/***********************************************************************************************************************
* Function Name: Auto_Network
* Description  : Setup Network parameters for Auto Connect 
                (Command AT+NAUTO=<1/0>,<1/0>,<Server IP>,<Port>)
                  1/0 - TCP/UDP
                  1/0 - Server/Client
                  Server IP - if UDP/TCP Client only
                  Port - Port Number
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Auto_Network()
{
    int i =0 ;
    sub_printf("AT+NAUTO=1,1,,8010\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i++;
    }
  if(!(Response_handle()))
    {   
        return(0);      
    }
return(1);
}

/***********************************************************************************************************************
* Function Name: Auto_Wifi
* Description  : Setup Wireless parameters for Auto Connect
                 (Command AT+WAUTO =<1/0>,<ssid>)
                  1/0 - Adhoc/Infrastructure
                  ssid - Wireless SSID to host/connect to
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Auto_Wifi()
{
    int i = 0;
    sub_printf("AT+WAUTO=0,wifidemo\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i ++;
    }
  if(!(Response_handle()))
    {   
        return(0);      
    }
return(1);
}

/***********************************************************************************************************************
* Function Name: Auto_Enable
* Description  : Enable Auto Connect                  
                 (Command ATC<1/0>)
                  1/0 - Enable/Disable Auto Connect Profile
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Auto_Enable()
{
    int i = 0;
    sub_printf("ATC1\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i++;
    }
  if(!(Response_handle()))
    {   
        return(0);      
    }
return(1);
}

/***********************************************************************************************************************
* Function Name: Save_Profile
* Description  : Save_Profile
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Save_Profile()
{
    int i = 0;
    sub_printf("AT&W0\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i++;
    }
  if(!(Response_handle()))
    {   
        return(0);      
    }
return(1);
}

/***********************************************************************************************************************
* Function Name: Auto_Start
* Description  : Start Auto Connect (Command ATA)
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Auto_Start()
{
    int i = 0;
	  PTE->PCOR = MASK(31);
    Delay(1000);
    sub_printf("ATA\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i++;
    }
  Data_handle(IP);
return(1);
}

/***********************************************************************************************************************
* Function Name: Set_Password
* Description  : Set WPA Password
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Set_Password()
{
    int i = 0;
    sub_printf("ATWWPA=\"password\"\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i++;
    }
  Data_handle(IP);
return(1);
}

/***********************************************************************************************************************
* Function Name: Disable_AutoConnect
* Description  : Disable Auto Connect Using GPIO8 on GS1011Mx
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Disable_AutoConnect()
{
   int i = 0;
	 PTE->PSOR = MASK(31);
   Delay(100);
	 PTE->PCOR = MASK(31);
return(1);
}


/***********************************************************************************************************************
* Function Name: Disassociate
* Description  : Disassociate from wireless networks
                 (Command AT+WD)
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t Disassociate()
{
   int i = 0;
   sub_printf("AT+WD\r\n");
   while(outbuf[i] != '\0')
   {
     SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
     i++;
   }
  if(!(Response_handle()))
    {   
        return(0);      
    }
return(1);
}

/***********************************************************************************************************************
* Function Name: DeepSleep
* Description  : DeepSleep
                 (Command AT+PSDPSLEEP)
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t DeepSleep()
{
   int i = 0;
   sub_printf("AT+PSDPSLEEP=10,0,0\r\n");
   while(outbuf[i] != '\0')
   {
     SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
     i++;
   }
return(1);
}

/***********************************************************************************************************************
* Function Name: DHCP_Enable
* Description  : Enable DHCP
                (Command AT+NDHCP=<1/0>)
                 1/0 - Enable/Disable Dynamic IP
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t DHCP_Enable()
{
    int i = 0;
    sub_printf("AT+NDHCP=1\r\n");
    while(outbuf[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &outbuf[i], 1);
      i++;
    }
  if(!(Response_handle()))
    {   
        return(0);      
    }
return(1);
}


/***********************************************************************************************************************
* Function Name: Auto_Setup
* Description  : This function sets up Auto Connection
* Arguments    : type 
                    0 - TCP server
* Return Value : None
***********************************************************************************************************************/

void Auto_Setup(uint8_t type)
{
	int i=0; 
	Control_RGB_LEDs(0,0,1);
  switch(type)
  {
    case 0:
			Disable_AutoConnect();
      Echo_Disable();             // Disable Echo for each command ( easier to parse)
     while (!(Echo_Confirm()));   // Send AT command till response OK 
     if(!Disassociate())          // Disassociate if currently associated to any network
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }
      if(!DHCP_Enable())         // Enable Dynamic IP (or Dynamic Host Configuration Protocol)
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }
			/*if(!Set_Password())         // Enable Connection to WPA AP 
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }*/
      if(!Auto_Wifi())           // Configure Auto Connect Wifi settings
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }
      if(!Auto_Network())       // Configure Auto Connect Network Settings 
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }
      if(!Auto_Enable())        // Enable Auto Connect Profile
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }
      if(!Auto_Start())         // Start Auto Connect
      {
				Control_RGB_LEDs(1,0,0);
        break;
      }
			Control_RGB_LEDs(1,0,1); // On Purple LED connect with client
      break;
    default:
      break;
  }
// Auto connection done 
// Now Actual Application  
  Data_handle(CLIENT);    //wait For Client Connection Details
	Control_RGB_LEDs(0,1,0);
	Delay(9000);
	while(i<10)
	{
	WIFI_SendString(outbuf);
	Delay(90);
  Disp_Client_Data();
	WIFI_idle();
	Delay(9000);
	WIFI_wake();
	i++;
	}
	
}      

/***********************************************************************************************************************
* Function Name: Disp_Client_Data
* Description  : This function Parses Data and changes the LED Color correspondingly
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void Disp_Client_Data()
{
  unsigned char buf;

  while(1)
  {
    while(!WIFI_ReceiveByte(&buf)){};

      if(buf == 'X')
      {
        break;
      }
			if(buf== 'Y' || buf=='y')
			{
					Control_RGB_LEDs(1,1,0);
			    break;
			}
			if(buf== 'P' || buf=='p')
			{
					Control_RGB_LEDs(1,0,1);
					break;
			}
			if(buf== 'W' || buf=='w')
			{
					Control_RGB_LEDs(1,1,1);
					break;
			}
	}
}


/***********************************************************************************************************************
* Function Name: Parse_Data
* Description  : This function Parses data from TCP Client
* Arguments    : buf (character)
* Return Value : 
                  0 - Skip Character
                  1 - Completed
                  2 - Store Character
* Message format: <0x1B>S<CID><Data><0x1B>E (0x1B = ESC_CHAR)
***********************************************************************************************************************/

uint8_t Parse_Data(unsigned char buf)
{
  static uint8_t START_ESC = 0,START_S = 0;
  static uint8_t END_ESC = 0, END_E = 0;
  static uint8_t BODY = 0, CID = 0;
      switch(buf) 
      {
          case SPI_CHAR_IDLE:
                      if(END_E && END_ESC && BODY)
                      {
                        START_S = 0; START_ESC = 0;
                        END_E = 0;END_ESC = 0; BODY = 0;
                        CID = 0;
                           return (1);
                      }
                      break;
          case('\0'):
                      break;
          case(ESC_CHAR):
                     if(!START_ESC)
                          {
                            START_ESC = 1;
                          }
                     else if(START_ESC && BODY)
                          {  
                              END_ESC = 1;
                          }
                     break;
           case('S'):
                  if(START_ESC && !START_S)
                      {
                        START_S = 1;
                      }
                  else if( START_ESC && START_S && CID)
                      {
                          BODY = 1;
                          return(2);
                      }
                  break;
            case('E'):
                  if(START_ESC && !END_E && END_ESC && START_S && CID)
                      {
                        END_E = 1;
                      }
                  else if(START_ESC && START_S && CID)
                      {
                          BODY = 1;
                          return(2);
                      }
                  break;
             case('1'):                        // <CID> = 1 for only 1 client can be modified for multiple clients
               if(START_ESC && START_S && !CID)
                      {
                        CID = 1;
                      }
                  else if(START_ESC && START_S && CID)
                      {
                          BODY = 1;
                          return(2);
                      }
               break;
            default:                    
                    if(START_ESC && START_S && CID)
                    {
                        BODY = 1;
                        return(2);
                    }
                        break;
       }
     return (0);
}

/***********************************************************************************************************************
* Function Name: Format_Data
* Description  : This function formats data for continuous input on ST7579 LCD (when using glyph libraries)
* Arguments    : buf (Character)
                 line 
                 column
* Return Value : None
***********************************************************************************************************************/

void Format_Data(char buf, uint8_t* line, uint8_t* column)
{
  
    if(buf == '\r' || buf == '\n')
    {
      if(*line == 7)
      {
        *line = 1;
        //LCDClear();
      }
      else
        (*line)++;
    }
    else if ( buf >= 'a'  || buf <= 'z' || buf >='A' || buf <= 'Z' 
                || buf >= '1' || buf <= '9')
    {
        //LCDPrintf(*line,*column,"%c",buf);
        if(*column == 16)
        {
          if(*line == 7)
          {
            *line = 1;
            //LCDClear();
          }
          else 
            (*line)++;
          *column = 0;
        }
        else
          (*column)++;
    }
    //else if ( buf == SPI_CHAR_IDLE);
    else
    {
      //LCDPrintf(8,0,"Invalid Char");
    }
}

/***********************************************************************************************************************
* Function Name: WIFI_idle
* Description  : Make the Module Enter Deep Sleep
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void WIFI_idle()
{
	Control_RGB_LEDs(1,0,0);
	Delay(90);
		if(!Disable_AutoConnect())
			{
				Control_RGB_LEDs(1,0,0);
      }
		PTC->PSOR = MASK(12);
		if(!DeepSleep())         // Enter DeepSleep
      {
				Control_RGB_LEDs(1,0,0);
      }
}

/***********************************************************************************************************************
* Function Name: WIFI_wake
* Description  : Wake Wifi using ALARM1 pin on GS1011Mx
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void WIFI_wake()
{
		PTC->PSOR = MASK(12);
	  Delay(90);
		PTC->PCOR  = MASK(12);
		while(!((PTB->PDIR & (0x800))));
		while (!(Echo_Confirm()));   // Confirm a OK response to AT
	  while (!(Echo_Confirm())); 
	  Control_RGB_LEDs(0,1,0);
	if(!Auto_Renable())         // Start Auto Connect
      {
				Control_RGB_LEDs(1,0,0);
      }
		Control_RGB_LEDs(1,0,1);	
}

/***********************************************************************************************************************
* Function Name: WIFI_SendString
* Description  : 
* Arguments    : None
* Return Value : Succesful or Failure
***********************************************************************************************************************/

uint8_t WIFI_SendString(char buffer[])
{
    int i = 0;
		i=0;
		Delay(10);
		//sub_printf("Hello World!");  // To test 
    while(buffer[i] != '\0')
    {
      SPI1_Send(SPI_WIFI,(uint8_t *) &buffer[i], 1);
      i++;
    }
		i=0;
		Delay(10);
return(1);
}

/***********************************************************************************************************************
* Function Name: WIFI_RecieveByte
* Description  : This function Retrieves a Byte at a time from the SPI Channel
* Arguments    : buf (Character pointer)
* Return Value : True or False 
* P7.7 - WIFI_SPI_IRQ - High when Data is available to be recieved
* SPI_CHAR_IDLE - XON(0xF5) - When SPI is active idle character sent to flush message from WIFI SPI BUFFER
***********************************************************************************************************************/

uint8_t WIFI_ReceiveByte(unsigned char* buf)
{
  uint8_t WIFI_Idle_Char;
    if((PTB->PDIR & (0x800)))
      {   WIFI_Idle_Char = SPI_CHAR_IDLE; 
          SPI_CS_Start(SPI_WIFI);
          SPI1_SendReceive(SPI_WIFI, (uint8_t *)&WIFI_Idle_Char, 1, (uint8_t *)buf);
          SPI_CS_End(SPI_WIFI);
          return true;  
      }
    else 
     return false;
}


/***********************************************************************************************************************
* Function Name: Recieved_Char_Handle
* Description  : This function Parses Data retrieved and Extracts only the "BODY"
                 of the data
* Arguments    : Character
* Return Value : Integer return indicating state of the parsing state machine.
* Message Response from WIFI Module : \r\n<OK/ERROR/message>\r\n
***********************************************************************************************************************/

uint8_t Received_Char_Handle(unsigned char buf)
{
  static uint8_t START_LF = 0,START_CR = 0;
  static uint8_t END_LF = 0, END_CR = 0;
  static uint8_t BODY = 0;
      switch(buf) 
      {
          case SPI_CHAR_IDLE:
                      if(END_LF && END_CR && BODY)
                      {
                        START_LF = 0; START_CR = 0;
                        END_LF = 0;END_CR = 0; BODY = 0;
                           return (1);
                      }
                      break;
          case('\0'):
                      break;
          case('\n'):
                     if(!START_LF)
                          {
                            START_LF = 1;
                          }
                     else if(START_LF && BODY)
                          {  
                              END_LF = 1;
                          }
                     break;
           case('\r'):
                     if(!START_CR)
                          {
                            START_CR = 1;
                          }
                     else if(START_CR && BODY)
                          {  
                            END_CR = 1;
                          }
                     break;
          default:
                    if(START_CR && START_LF)
                    {
                        BODY = 1;
                        return(2);
                    }
                        break;
       }
     return (0);
}


/***********************************************************************************************************************
* Function Name: Response_Handle
* Description  : This function Handles Response to AT Commands
* Arguments    : None
* Return Value : ERROR or OK
***********************************************************************************************************************/
         
uint8_t Response_handle()
{
   uint8_t inbuf[64];
   uint32_t i =0; 
   uint8_t chk = 0;
   unsigned char buf;
  while(!((PTB->PDIR & (0x800))));
  while (1)
  { 
    while(!WIFI_ReceiveByte(&buf));   // Checks and flushes data from WIFI SPI buffer one character at a time
    chk = Received_Char_Handle	(buf);  // State Machine to Extract message from formated message response
       if(chk == 2)
       {
          inbuf[i++] = buf;
          if(i > 64)
          {
            //LCDPrintf(7,0,"OVERFLOW");   
            i = 0;
          }
       }
       else if (chk == 1)
       { 
         break;
       }
  }
   inbuf[i] = '\0';
   if(inbuf[0] == 'O')
     return (1);
   else if(inbuf[0] == 'E')
     return (0);
   else
   {
     return (0);
   }     
}

/***********************************************************************************************************************
* Function Name: Data_Handle
* Description  : This function None Response Data
* Arguments    : type 
                  IP - for IP address input from connection establishment
                  CLIENT - Check for successful connected Client and Display IP of Client                  
* Return Value : None
***********************************************************************************************************************/
         
void Data_handle(uint8_t type)
{
   uint8_t inbuf[64];
   uint32_t i =0; 
   uint8_t chk = 0;
   unsigned char buf;
  while(!((PTB->PDIR & (0x800))));
  while (1) 
  { 
        while(!WIFI_ReceiveByte(&buf));   // Checks and flushes data from WIFI SPI buffer one character at a time
       chk = Received_Char_Handle(buf);   // State Machine to Extract message from formated message response
       if(chk == 2)
       {
          inbuf[i++] = buf;
          if(i > 64)
          {
            //LCDPrintf(7,0,"OVERFLOW");   
            i = 0;
          }
       }
       else if (chk == 1)
       { 
         break;
       }
  }
inbuf[i] = '\0';
switch(type)
{
  case IP: IP_extract(inbuf,1);break;
  case CLIENT: 
    if(inbuf[0] == 'C')
    {
      //LCDPrintf(1,0,"Client Connected");
      //LCDPrintf(8,0,"SW2 for Data");
      connection_id = inbuf[10];
      IP_extract(inbuf,2);
    }
}
}


/***********************************************************************************************************************
* Function Name: IP_extract
* Description  : This function None Response Data
* Arguments    : buf (Character Pointer) 
                type 
                  1 - extract IP address from connection establishment response
                  2 - extract IP of Client when client connects                  
* Return Value : None
***********************************************************************************************************************/

void IP_extract(uint8_t * buf, uint8_t type)
{
  int i,j;
  char ip_address[32];
  switch(type)
  {
    case(1):
      for(i = 0; buf[i] != ':';i++)    // Message Format: <IP>:<Subnet>
      {
        ip_address[i] = (char)buf[i];
      }
      ip_address[i]='\0';
      //LCDPrintf(2,0,"IP:");
      //LCDPrintf(3,0,"%s",ip_address);
      //LCDPrintf(4,0,"Port: 8010");
      break;
    case(2):
      for(i = 12,j=0; i<=24;i++,j++)    // From Message Buffer               
      {                                 // Message Format = CONNECTED<CID><0/1> <IP>        
        ip_address[j] = (char)buf[i];   //                                      ^
      }                                 //                                      |
      ip_address[j]='\0';               //                                     12th bit 
      //LCDPrintf(2,0,"Client IP:");
      //LCDPrintf(3,0,"%s",ip_address);
      break;
      
}     
}   

