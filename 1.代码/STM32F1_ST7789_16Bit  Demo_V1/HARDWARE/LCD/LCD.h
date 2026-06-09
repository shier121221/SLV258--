#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 

	 
/////////////////////////////////////�û�������///////////////////////////////////	 
//֧�ֺ��������ٶ����л�
#define USE_LCM_DIR  	  0   	//����Һ����˳ʱ����ת���� 	0-0����ת��1-180����ת��2-270����ת��3-90����ת
#define USE_TP_TYPE   	RTP 	//���崥������  CTP = ����ģʽ , RTP = ����ģʽ

#define CTP 0X80		//����ģʽ
#define RTP 0X00		//����ģʽ	 
	 
	 
//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ����
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	wramcmd;		//��ʼдgramָ��
	u16 setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	 
}_lcd_dev; 	  

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 

#define LCD_RST_SET  GPIOA->BSRR=1<<11    //��λ  	    	 PC15
#define LCD_CS_SET                        //Ƭѡ�˿�  	     PA12
#define LCD_RS_SET   GPIOA->BSRR=1<<8     //����/����       PA15	   
#define LCD_WR_SET   GPIOA->BSRR=1<<9     //д����					 PC13
#define LCD_RD_SET   GPIOA->BSRR=1<<10    //������					 PC14

#define LCD_RST_CLR  GPIOA->BRR =1<<11    //��λ  	    		 PC15
#define LCD_CS_CLR                        //Ƭѡ�˿�  	     PA12
#define LCD_RS_CLR   GPIOA->BRR =1<<8     //����/����       PA15   
#define LCD_WR_CLR   GPIOA->BRR =1<<9     //д����					 PC13
#define LCD_RD_CLR   GPIOA->BRR =1<<10    //������					 PC14  	

#define LCD_LED PCout(14) //���� ����

// DB[6:0]=PA[6:0], DB7=PA12, DB[15:8]=PB[10:3]
#define DATAOUT(x) do { \
    GPIOA->ODR = (GPIOA->ODR & 0xEF80u) | ((u16)(x)&0x7Fu) | (((u16)(x)&0x80u)<<5); \
    GPIOB->ODR = (GPIOB->ODR & 0xF807u) | (((u16)(x)&0xFF00u)>>5); \
} while(0)
#define DATAIN ((GPIOA->IDR & 0x7Fu) | ((GPIOA->IDR>>5)&0x80u) | ((GPIOB->IDR<<5)&0xFF00u))


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ 
#define LGRAY 					 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    															  
	    															  
void LCD_Init(void);													   	//��ʼ��
void LCD_DisplayOn(void);													//����ʾ
void LCD_DisplayOff(void);													//����ʾ
void LCD_Clear(u16 Color);	 												//����
void LCD_SetCursor(u16 Xpos, u16 Ypos);										//���ù��
void LCD_DrawPoint(u16 x,u16 y);											//����
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								//���ٻ���
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);						 			//��Բ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);							//����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   				//������
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//���ָ����ɫ
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//��ʾһ���ַ���,12/16����

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void Load_Drow_Dialog(void);
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);	//���ô���				
void Show_Str(u16 x, u16 y,u8 *str,u8 size,u8 mode);//��ʾ����
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p); //��ʾ40*40 ͼƬ
void Gui_StrCenter(u16 x, u16 y,u8 *str,u8 size,u8 mode);//������ʾ
void LCD_Display_Dir(u8 dir); //����LCD��ʾ����
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);//��һ������
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);//��ʵ��Բ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);//��ˮƽ�� ���ݴ�����ר�в���
u16 LCD_Read_ID(u8 reg);//��ID
u16 LCD_ReadPoint(u16 x,u16 y);//��ȡָ��������ɫ



#endif













