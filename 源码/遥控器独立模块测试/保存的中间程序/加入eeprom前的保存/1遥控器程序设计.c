#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P1^4  ;  //定义DS18B20端口DQ
sbit BEEP=P1^3  ; //蜂鸣器驱动线

uchar Menu_INT1=0;
uchar count=0;

uchar BK_count; //报警专用

bit presence;
bit at=0;

sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;

uchar code  cdis1[ ] = {"Position:model1."};
uchar code  cdis2[ ] = {" WENDU:    .  C "};
uchar code  cdis3[ ] = {" DS18B20  ERR0R "};
uchar code  cdis4[ ] = {"  PLEASE CHECK  "};
uchar code  cdis5[ ] = {"Position:model2."};
uchar code  cdis6[ ] = {"Position:model3."};


/*************************************/

sbit PT2262_Data0=P1^0;
sbit PT2262_Data1=P1^1;
sbit PT2262_Data2=P1^2;
sbit PT2262_Data3=P1^5;
sbit PT2262_TE=P1^6;		/*模式三中要置为一*/
sbit System=P2^0;
sbit MoveUp=P2^1;
sbit MoveDown=P2^5;
sbit SelfAdd=P2^6;
sbit Bu_OpenUp=P3^0;
sbit Bu_CloseDown=P3^1;
sbit Bu_SlowlyUp=P3^4;
sbit Bu_SlowlyDown=P3^5;
sbit Bu_SetZero=P1^7;
sbit Bu_MoveUp=P3^6;
sbit Bu_MoveDown=P3^7;
sbit Bu_AllowSelfControl=P2^7;
sbit LED_Model1=P3^2;


char key=0;
uchar Button_Command;


uchar code Function0[ ]={" Happy everyday "};
uchar code Function1[ ]={"    Open UP     "};
uchar code Function2[ ]={"   Close Down   "};
uchar code Function3[ ]={"   Slowly Up    "};
uchar code Function4[ ]={"   Slowly Down  "};
uchar code Function5[ ]={"  Wrong Command "};
uchar code Function6[ ]={"  Self Control  "};
uchar code Function7[ ]={"     Move Up    "};
uchar code Function8[ ]={"    Move Down   "};
uchar code Function9[ ]={"  Now,set zero  "};
uchar code Function10[ ]={"AllowSelfcontrol"};
uchar code Function11[ ]={"No Self control"};

/*************************************/

uchar code PrintString[]={"CLOCK!"};
uchar code word[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x20};
uchar dispbuf[8],h,m,s,counter;









unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*小数位表，四舍五入*/
//
void beep() ;
unsigned char code  mytab[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00} ;

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

/*******************************************************************/
void delay1(int ms)
{
 uchar y;
  while(ms--)
 {
  for(y = 0 ; y<250 ; y++)
  {
   _nop_();
   _nop_();
   _nop_();
   _nop_();
  }
 }
}

/******************************************************************/
/*检查LCD忙状态                                                   */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。   */
/******************************************************************/
bit lcd_busy()
 {
    bit result ;
    LCD_RS = 0 ;
    LCD_RW = 1 ;
    LCD_EN = 1 ;
    delayNOP() ;
    result = (bit)(P0&0x80) ;
    LCD_EN = 0 ;
    return(result) ;
 }

/*写指令数据到LCD                                                  */
/*RS=L，RW=L，E=高脉冲，D0-D7=指令码。          */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
{
   while(lcd_busy()) ;
    LCD_RS = 0 ;
    LCD_RW = 0 ;
    LCD_EN = 0 ;
    _nop_() ;
    _nop_() ;
    P0 = cmd ;
    delayNOP() ;
    LCD_EN = 1 ;
    delayNOP() ;
    LCD_EN = 0 ;
}

/*******************************************************************/
/*写显示数据到LCD                            */
/*RS=H，RW=L，E=高脉冲，D0-D7=数据。       */
/*******************************************************************/
void lcd_wdat(uchar dat)
{
   while(lcd_busy()) ;
    LCD_RS = 1 ;
    LCD_RW = 0 ;
    LCD_EN = 0 ;
    P0 = dat ;
    delayNOP() ;
    LCD_EN = 1 ;
    delayNOP() ;
    LCD_EN = 0 ;
}


/*  LCD初始化设定                                                  */
/*******************************************************************/
void lcd_init()
{
    delay1(15) ;
    lcd_wcmd(0x01) ;      //清除LCD的显示内容
    lcd_wcmd(0x38) ;      //16*2显示，5*7点阵，8位数据
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;

    lcd_wcmd(0x0c) ;      //显示开，关光标
    delay1(5) ;
    lcd_wcmd(0x06) ;      //移动光标
    delay1(5) ;
    lcd_wcmd(0x01) ;      //清除LCD的显示内容
    delay1(5) ;
}

/*  设定显示位置                                    */
/*******************************************************************/
void lcd_pos(uchar pos)
{
  lcd_wcmd(pos | 0x80) ;  //数据指针=80+地址变量
}

/*自定义字符写入CGRAM                                   */
/*******************************************************************/
void  writetab()
{
    unsigned char i ;
    lcd_wcmd(0x40) ;            //写CGRAM
    for (i = 0 ; i< 8 ; i++)
    lcd_wdat(mytab[ i ]) ;
}

/*us级延时函数                                        */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num ) ;
}

/*初始化ds1820                                      */
/*******************************************************************/
Init_DS18B20(void)
{
     DQ = 1 ;      //DQ复位
     Delay(8) ;    //稍做延时

     DQ = 0 ;      //单片机将DQ拉低
     Delay(90) ;   //精确延时 大于 480us

     DQ = 1 ;       //拉高总线
     Delay(8) ;

     presence = DQ ;    //如果=0则初始化成功 =1则初始化失败
     Delay(100) ;
     DQ = 1 ;

     return(presence) ; //返回信号，0=presence,1= no presence
}


/* 读一个字节                     */
/*******************************************************************/
 ReadOneChar(void)
{
unsigned char i = 0 ;
unsigned char dat = 0 ;

for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ; // 给脉冲信号
    dat >>= 1 ;
    DQ = 1 ; // 给脉冲信号

    if(DQ)
     dat |= 0x80 ;
    Delay(4) ;
  }

    return (dat) ;
}

/* 写一个字节                                         */
/*******************************************************************/
 WriteOneChar(unsigned char dat)
{
  unsigned char i = 0 ;
  for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ;
    DQ = dat&0x01 ;
    Delay(5) ;

    DQ = 1 ;
    dat>>=1 ;
  }
}

/* 读取温度                            */
/*******************************************************************/
 Read_Temperature(void)
{
     Init_DS18B20() ;

     WriteOneChar(0xCC) ;  // 跳过读序号列号的操作
     WriteOneChar(0x44) ;  // 启动温度转换

     Init_DS18B20() ;
     WriteOneChar(0xCC) ;  //跳过读序号列号的操作
     WriteOneChar(0xBE) ;  //读取温度寄存器

     temp_data[0] = ReadOneChar() ;   //温度低8位
     temp_data[1] = ReadOneChar() ;   //温度高8位
}

/* 数据转换与温度显示                                              */
/*******************************************************************/
 Disp_Temperature()
{
  display[4]=temp_data[0]&0x0f ;
  display[0]=ditab[display[4]]+0x30 ;     //查表得小数位的值

  display[4]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4) ;
  display[3]=display[4]/100+0x30 ;
  display[1]=display[4]%100 ;
  display[2]=display[1]/10+0x30 ;
  display[1]=display[1]%10+0x30 ;

    if(display[3]==0x30)        //高位为0，不显示
   {
     display[3]=0x20 ;
     if(display[2]==0x30)      //次高位为0，不显示
     display[2]=0x20 ;
   }

     lcd_pos(0x48) ;
     lcd_wdat(display[3]) ;        //百位数显示
     lcd_pos(0x49) ;
     lcd_wdat(display[2]) ;        //十位数显示
  lcd_pos(0x4a) ;
     lcd_wdat(display[1]) ;        //个位数显示
  lcd_pos(0x4c) ;
     lcd_wdat(display[0]) ;        //小数位数显示
}

/*******************************************************************/
/* 蜂鸣器响一声                 */
/*******************************************************************/
void beep()
  {
    unsigned char y ;
    for (y=0 ;y<100 ;y++)
    {
      Delay(60) ;
      BEEP=!BEEP ;                //BEEP取反
    }
    BEEP=1 ;                      //关闭蜂鸣器
 Delay(40000) ;
  }

/* DS18B20 OK 显示菜单                                             */
/*******************************************************************/
void  Ok_Menu()
{
    uchar  m ;
    lcd_init() ;                //初始化LCD

    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //显示字符
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //显示字符
       m++ ;
     }

     writetab() ;               //自定义字符写入CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //显示自定义字符
}


/* DS18B20 ERROR 显示菜单                             */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m ;
     lcd_init() ;                //初始化LCD

    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
     while(cdis3[m] != '\0')
     {                         //显示字符
       lcd_wdat(cdis3[m]) ;
       m++ ;
     }

     lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
     while(cdis4[m] != '\0')
     {
       lcd_wdat(cdis4[m]);      //显示字符
       m++ ;
     }
}






/*******更新缓冲区子程序*******/
void newbuf()
{
 dispbuf[0]=s%10;
 dispbuf[1]=s/10;
 dispbuf[3]=m%10;
 dispbuf[4]=m/10;
 dispbuf[6]=h%10;
 dispbuf[7]=h/10;
}

 

/*******显示子程序**********/
void disp(uchar dispadd)
{
   uchar tmp;
   lcd_wcmd(dispadd);
   tmp=dispbuf[7];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[6];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[5];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[4];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[3];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[2];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[1];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[0];
   tmp=word[tmp];
   lcd_wdat(tmp); 
}

 

 

/*********************键盘子程序***********************/



/**************************************************************以下键盘扫描不再用*************************************************************************/
/*
uchar keypro(void)
{
  uchar scanl,scanh;
  P1=0xf0;
  if((P1&0xf0)!=0xf0)
  {
    delay1(1);
    if((P1&0xf0)!=0xf0)
    {
      scanl=0xfe;
      while((scanl&0x10)!=0)
      {
        P1=scanl;
        if((P1&0xf0)!=0xf0)
        {
          scanh=(P1&0xf0)|0x0f;
          return (~scanh)+(~scanl);
         }
        else scanl=(scanl<<1)|0x01;
      }
    } 
  } 
  return 0;
}
*/
/************************************************************以上为注释掉的键盘扫描***************************************************************/





/***************************************************全新键盘扫描**********************************************************************************/
uchar KeyScan_1(void){

key=0;
System=1;
MoveUp=1;
MoveDown=1;
SelfAdd=1;
Bu_OpenUp=1;
Bu_CloseDown=1;
Bu_SlowlyUp=1;
Bu_SlowlyDown=1;
Bu_MoveUp=1;
Bu_MoveDown=1;
Bu_SetZero=1;
Bu_AllowSelfControl=1;


if((Bu_OpenUp==1)&&(Bu_CloseDown==1)&&(Bu_SlowlyUp==1)&&(Bu_SlowlyDown==1)&&(System==1)&&(MoveUp==1)&&(MoveDown==1)&&(SelfAdd==1)&&(Bu_MoveUp==1)&&(Bu_MoveDown==1)&&(Bu_SetZero==1)&&(Bu_AllowSelfControl==1)){
delay1(5);
if((Bu_OpenUp==1)&&(Bu_CloseDown==1)&&(Bu_SlowlyUp==1)&&(Bu_SlowlyDown==1)&&(System==1)&&(MoveUp==1)&&(MoveDown==1)&&(SelfAdd==1)&&(Bu_MoveUp==1)&&(Bu_MoveDown==1)&&(Bu_SetZero==1)&&(Bu_AllowSelfControl==1))
key=0;
}


if(Bu_OpenUp==0){
delay1(5);
if(Bu_OpenUp==0)
key=1;
while(Bu_OpenUp==0);
}



if(Bu_CloseDown==0){
delay1(5);
if(Bu_CloseDown==0)
key=2;
while(Bu_CloseDown==0);
}


if(Bu_SlowlyUp==0){
delay1(5);
if(Bu_SlowlyUp==0)
key=3;
while(Bu_SlowlyUp==0);
}


if(Bu_SlowlyDown==0){
delay1(5);
if(Bu_SlowlyDown==0)
key=4;
while(Bu_SlowlyDown==0);
}


if(System==0){
delay1(5);
if(System==0)
key=5;
while(System==0);
}


if(MoveUp==0){
delay1(5);
if(MoveUp==0)
key=6;
while(MoveUp==0);
}


if(MoveDown==0){
delay1(5);
if(MoveDown==0)
key=7;
while(MoveDown==0);
}


if(SelfAdd==0){
delay1(5);
if(SelfAdd==0)
key=8;
while(SelfAdd==0);
}


if(Bu_MoveUp==0){
delay1(5);
if(Bu_MoveUp==0)
key=9;
while(Bu_MoveUp==0);
}


if(Bu_MoveDown==0){
delay1(5);
if(Bu_MoveDown==0)
key=10;
while(Bu_MoveDown==0);
}


if(Bu_SetZero==0){
delay1(5);
if(Bu_SetZero==0)
key=11;
while(Bu_SetZero==0);
}


if(Bu_AllowSelfControl==0){
delay1(5);
if(Bu_AllowSelfControl==0)
key=12;
while(Bu_AllowSelfControl==0);
}


return key;
}

/****************************************************/

uchar KeyScan_2(void){

key=0;
System=1;
MoveUp=1;
MoveDown=1;
SelfAdd=1;
Bu_OpenUp=1;
Bu_CloseDown=0;
Bu_SlowlyUp=1;
Bu_SlowlyDown=1;
Bu_MoveUp=1;
Bu_MoveDown=1;
Bu_SetZero=1;
Bu_AllowSelfControl=1;


if((Bu_OpenUp==1)&&(Bu_SlowlyUp==1)&&(Bu_SlowlyDown==1)&&(System==1)&&(MoveUp==1)&&(MoveDown==1)&&(SelfAdd==1)&&(Bu_MoveUp==1)&&(Bu_MoveDown==1)&&(Bu_SetZero==1)&&(Bu_AllowSelfControl==1)){
delay1(5);
if((Bu_OpenUp==1)&&(Bu_SlowlyUp==1)&&(Bu_SlowlyDown==1)&&(System==1)&&(MoveUp==1)&&(MoveDown==1)&&(SelfAdd==1)&&(Bu_MoveUp==1)&&(Bu_MoveDown==1)&&(Bu_SetZero==1)&&(Bu_AllowSelfControl==1))
key=0;
}


if(Bu_OpenUp==0){
delay1(5);
if(Bu_OpenUp==0)
key=1;
while(Bu_OpenUp==0);
}


/*
if(Bu_CloseDown==0){
delay1(5);
if(Bu_CloseDown==0)
key=2;
while(Bu_CloseDown==0);
}
*/


if(Bu_SlowlyUp==0){
delay1(5);
if(Bu_SlowlyUp==0)
key=3;
while(Bu_SlowlyUp==0);
}


if(Bu_SlowlyDown==0){
delay1(5);
if(Bu_SlowlyDown==0)
key=4;
while(Bu_SlowlyDown==0);
}


if(System==0){
delay1(5);
if(System==0)
key=5;
while(System==0);
}


if(MoveUp==0){
delay1(5);
if(MoveUp==0)
key=6;
while(MoveUp==0);
}


if(MoveDown==0){
delay1(5);
if(MoveDown==0)
key=7;
while(MoveDown==0);
}


if(SelfAdd==0){
delay1(5);
if(SelfAdd==0)
key=8;
while(SelfAdd==0);
}


if(Bu_MoveUp==0){
delay1(5);
if(Bu_MoveUp==0)
key=9;
while(Bu_MoveUp==0);
}


if(Bu_MoveDown==0){
delay1(5);
if(Bu_MoveDown==0)
key=10;
while(Bu_MoveDown==0);
}


if(Bu_SetZero==0){
delay1(5);
if(Bu_SetZero==0)
key=11;
while(Bu_SetZero==0);
}


if(Bu_AllowSelfControl==0){
delay1(5);
if(Bu_AllowSelfControl==0)
key=12;
while(Bu_AllowSelfControl==0);
}


return key;
}


/********************时间调整子程序********************/
void adjustime()
{
  uchar k;
  static uchar add;
  k=KeyScan_2();
  switch(k)
  {
   case 5: if(!at){add=0xc1;EA=0;lcd_wcmd(0xc1);lcd_wcmd

(0x0f);at=1;}
               else {lcd_wcmd(0xc0);lcd_wcmd

(0x0c);at=0;EA=1;} break;

   case 6:  if(at)
             {
       if(add==0xc1){add=0xc7; lcd_wcmd(add);}
                 else {add=add-3;lcd_wcmd(add);}
               }
         break;
  
  case 7:  if(at)
             {
               if(add==0xc7) {add=0xc1;lcd_wcmd(add);}
                 else {add=add+3;lcd_wcmd(add);}
           }
       break;

   case 8: if(at)
            {
              if(add==0xc1) h++;
                if(h==24)
          h=0;
                if(add==0xc4) m++;
         if(m==60)
           m=0;
                if(add==0xc7) s++;
          if(s==60)
           s=0;
           newbuf();
      disp(0xc0);
      lcd_wcmd(add);
              }
               break;
  default: break;
  }
  if(k!=0)
   {
  while((P1&0xf0)!=0xf0)
  P1=0xf0;
 }

}

void Self_BK_Define2(void){  
	 uchar  m ;
    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //显示字符
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //显示字符
       m++ ;
     }

     writetab() ;               //自定义字符写入CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //显示自定义字符
}

void Self_BK_Define1(void){    //用于时间
  uchar i=0;
  lcd_pos(0);
  while(cdis5[i]!='\0'){
  lcd_wdat(cdis5[i]);
  i++;
  }
  lcd_wcmd(0xc9);
  for(i=0;i<6;i++)
  lcd_wdat(PrintString[i]);
  lcd_wcmd(0xc0);
}

/*********************初始化子程序**********************/
void init()
{
  TMOD=0x01;
  TH0=0x4c;
  TL0=0x00;
   IT1=1;
	PX1=1;
  EX1=1;
	EA=1;
  ET0=1;
  TR0=1;
  counter=0;
  h=12;m=0;s=0;
  dispbuf[2]=10;
  dispbuf[5]=10;
//  PT2262_TE=0;
}

void MainModel2(void){

Bu_CloseDown=0;
System=1;
LED_Model1=1;

 Self_BK_Define1();
  while(1)
  {
   adjustime();
   if(!at)
 {
     //闪烁
      if(counter<10)        
      {
       dispbuf[2]=10;
       dispbuf[5]=10;
      }
      else
      {            
       dispbuf[2]=11;
       dispbuf[5]=11;
      }
      //更新显示缓冲区及调用显示程序
      if(counter==0)
      {
       newbuf();
       disp(0xc0);
       }
      else if(counter==10)
      disp(0xc0);  
    }
	 if((((Menu_INT1)%4)==0)||(((Menu_INT1)%4)==2)||(((Menu_INT1)%4)==3))break;
   }
/*	Bu_CloseDown=0;
   System=1;
   LED_Model1=0;
*/
} 

/*void dely500(void){

	uchar i;
	for(i=250;i>0;i--){_nop_();
		
}
}
*/


/******************************************/

void delay500(void){

	uchar i;
	for(i=250;i>0;i--){_nop_();}
}


/*****************************************/

void MainModel1(void){
 
Self_BK_Define2();
System=0;
LED_Model1=1;
Bu_CloseDown=1;

 do
  {
      Read_Temperature() ;
  	   Disp_Temperature() ;
			
  if(display[2]>=51){for(BK_count=200;BK_count>0;BK_count--){
  
  				BEEP=~BEEP;
  				delay500();  
  }
  		for(BK_count=200;BK_count>0;BK_count--){
		
			BEEP=~BEEP;
			delay500();
			delay500();
		}
    }	
	


		
    if((((Menu_INT1)%4)==1)||(((Menu_INT1)%4)==2)||(((Menu_INT1)%4)==3))break;
     }
    while(!presence);

     Error_Menu();

 do
  {
    Init_DS18B20();
    beep();
     }
 while(presence);
/*System=1;
LED_Model1=0;
Bu_CloseDown=1;
*/ 
}



/**************************模块三加入******************************/
/*模式三，初始化显示子函数*/
/*********************************************************************************/

void Self_BK_Define3(void){    //用于模式三，显示按键操作

	uchar  m ;
    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis6[m]!= '\0')
     {                         //显示字符
       lcd_wdat(cdis6[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(Function0[m] != '\0')
     {
       lcd_wdat(Function0[m]) ;      //显示字符
       m++ ;
     }
}


/******************************************************************************/

void MainModel3(void){
	uchar n;
	LED_Model1=0;
Bu_CloseDown=1;
System=1;

	Self_BK_Define3();
	delay1(1000);
	
while(1){

//  Self_BK_Define3();
	Button_Command=KeyScan_1();
		PT2262_TE=0;
	if(Button_Command==0){//PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=1;
		
	lcd_pos(0x40);
	n=0;
	while(Function6[n]!='\0'){
	lcd_wdat(Function6[n]);
			n++;
			}
}	

else if(Button_Command==1){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function1[n]!='\0'){
	lcd_wdat(Function1[n]);
			n++;
			}
}	

else if(Button_Command==2){PT2262_Data0=0;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function2[n]!='\0'){
	lcd_wdat(Function2[n]);
			n++;
			}
}	


else if(Button_Command==3){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function3[n]!='\0'){
	lcd_wdat(Function3[n]);
			n++;
			}
}	


else if(Button_Command==4){PT2262_Data0=0;PT2262_Data1=1;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function4[n]!='\0'){
	lcd_wdat(Function4[n]);
			n++;
			}
}	


else if(Button_Command==8){PT2262_Data0=0;PT2262_Data1=0;PT2262_Data2=1;PT2262_Data3=1;
		
	lcd_pos(0x40);
	n=0;
	while(Function11[n]!='\0'){
	lcd_wdat(Function11[n]);
			n++;
			}
}	


else if(Button_Command==9){PT2262_Data0=1;PT2262_Data1=0;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function7[n]!='\0'){
	lcd_wdat(Function7[n]);
			n++;
			}
}	


else if(Button_Command==10){PT2262_Data0=0;PT2262_Data1=0;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function8[n]!='\0'){
	lcd_wdat(Function8[n]);
			n++;
			}
}


else if(Button_Command==11){PT2262_Data0=0;PT2262_Data1=0;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function9[n]!='\0'){
	lcd_wdat(Function9[n]);
			n++;
			}
}


else if(Button_Command==12){PT2262_Data0=0;PT2262_Data1=0;PT2262_Data2=0;PT2262_Data3=1;
		
	lcd_pos(0x40);
	n=0;
	while(Function10[n]!='\0'){
	lcd_wdat(Function10[n]);
			n++;
			}
}



else {//PT2262_Data0=0;PT2262_Data1=0;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function5[n]!='\0'){
	lcd_wdat(Function5[n]);
			n++;
			}
}	
delay1(200);
PT2262_TE=1;
 if((((Menu_INT1)%4)==1)||(((Menu_INT1)%4)==0)||(((Menu_INT1)%4)==3))break;
}
//LED_Model1=0;
}//模块3最后一行




/*********************************************************************************/

void MainModel4(void)
{
while(1){
_nop_();

if((((Menu_INT1)%4)==1)||(((Menu_INT1)%4)==0)||(((Menu_INT1)%4)==2))break;
}
}

/*****************************************************************/

/* 主函数     */
/************************************/
void main(void)
 {
 	Ok_Menu();
	init();


	while(1){
	if(((Menu_INT1)%4)==0){
   	MainModel1(); }
	else if(((Menu_INT1)%4)==1){
		MainModel2(); }
	else 	if(((Menu_INT1)%4)==2){
	PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=1;
	MainModel3();}
	else {MainModel4();}
}
}

		
/*************************定时器0的中断**********************/
void Time0() interrupt 1 using 2    //再次强调中断子程序执行时间越短越好
{
 // TH0=(65536-46075)/256;
 TH0=0x4c;
  //TL0=(65536-46075)%256;
  TL0=0x00;
  counter++;
  if(counter==20)
  {
    s++;
  counter=0;
  if(s==60)
  { 
    m++;
    s=0;
      if(m==60)
    {
      h++;
    m=0;
    if(h==24)
     h=0;
         }
       }

  }
}



/*外部中断*/
void int1_Menu_INT1() interrupt 2  
	{
		Menu_INT1++;
		
		if(Menu_INT1==8)Menu_INT1=0;
	
	}
