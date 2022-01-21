/*******************************************************************************
author: Libor Dvoracek
login:  xdvora2t
title:  Cezarovo sifrovaci zarizeni v2.0 
        FIT VUTBR IMP 2019/2020 Project
*******************************************************************************/

#include <fitkitlib.h>
#include <lcd/display.h>
#include <keyboard/keyboard.h>
#include <stdlib.h>
#include <string.h>


volatile char last_ch;           // last character for comparison
volatile char i_ch;              // interupt aux. variable
volatile char ch;                // current character
int char_cnt = 0;                // max 32 chars
volatile char str[32] = "";      // string for storing input from keypad
int index = 0;                   // index in str;

int btn_cnt_2 = 0;               // button count 
int btn_cnt_3 = 0;
int btn_cnt_4 = 0;
int btn_cnt_5 = 0;
int btn_cnt_6 = 0;
int btn_cnt_7 = 0;
int btn_cnt_8 = 0;
int btn_cnt_9 = 0;
int btn_cnt_0 = 0;

int shift = 3;                   // puvodni Cesaruv posun

/*******************************************************************************
 * Sifrovani 
*******************************************************************************/
int code(char *input) {

   LCD_clear();
   int i;
   char tmp_ch;
   for ( i = 0; input[i] != '\0'; ++i ) {
      tmp_ch = input[i];
      if ( tmp_ch >= 'A' && tmp_ch <= 'Z' ) {

         tmp_ch = tmp_ch - shift;
         if (tmp_ch < 'A' ) {
            tmp_ch = tmp_ch + 'Z' - 'A' + 1;
         }
         input[i]=tmp_ch;
      }
   }
   //LCD_append_char('L');
   LCD_append_string(input);
   return 0;     
}

/*******************************************************************************
 * Desifrovani 
*******************************************************************************/
int decode(char *input) {

   LCD_clear();
   //LCD_append_string(input);
   int i;
   char tmp_ch;
   for ( i = 0; input[i] != '\0'; ++i ) {
      tmp_ch = input[i];
      if ( tmp_ch >= 'A' && tmp_ch <= 'Z' ) {

         tmp_ch = tmp_ch + shift;
         if (tmp_ch > 'Z' ) {
            tmp_ch = tmp_ch - 'Z' + 'A' - 1;
            //tmp_ch = tmp_ch - (26*nr_of_abcs);
         }
         input[i]=tmp_ch;
      }
   }
   //LCD_append_char('L');
   LCD_append_string(input);
   return 0;     
}

/*******************************************************************************
 * Napoveda na LCD 
*******************************************************************************/
void help () {

   LCD_init();
   LCD_append_string("1.enter string  ");
   LCD_append_string("2.shift=3 +-A/B ");
   delay_ms(1000);
   LCD_init();
   LCD_append_string("2.shift=3 +-A/B ");
   LCD_append_string("3.C=code D=dcode");
   delay_ms(1000);
   LCD_init();
   LCD_append_string("3.C=code D=dcode");
   LCD_append_string("4.* new entry   ");
   delay_ms(1000);
   LCD_init();
   return;
} 

/*******************************************************************************
 * Osetreni klavesnice
*******************************************************************************/
int keyboard_idle() {
     
    ch = key_decode(read_word_keyboard_4x4());
    if (ch != last_ch) {
      last_ch = ch;

      if (char_cnt < 31 ){
         if (ch == 'C' ) {                            // code

            code(str);
            //str[0] = '\0';
            term_send_str_crlf(str);
         } 
         else if (ch == 'D' ) {                       // decode

            decode(str);
            term_send_str_crlf(str);
         } 
         else if (ch ==  '*') {                       // restart
            
            _DINT();                                  // zakaz preruseni 
            WDTCTL = WDTCNTCL + WDTIS0;               // reset
            term_send_str_crlf(str);
         }
         else if (ch ==  '#') {                       // help
            help();
         }
         else if (ch ==  '2') {                       // button press counters
            btn_cnt_2++;
            term_send_str_crlf("BUTTON 2 PRESSED");
         }
         else if (ch ==  '3') {
            btn_cnt_3++;
            term_send_str_crlf("BUTTON 3 PRESSED");
         }
         else if (ch ==  '4') {
            btn_cnt_4++;
            term_send_str_crlf("BUTTON 4 PRESSED");
         }
         else if (ch ==  '5') {
            btn_cnt_5++;
            term_send_str_crlf("BUTTON 5 PRESSED");
         }
         else if (ch ==  '6') {
            btn_cnt_6++;
            term_send_str_crlf("BUTTON 6 PRESSED");
         }
         else if (ch ==  '7') {
            btn_cnt_7++;
            term_send_str_crlf("BUTTON 7 PRESSED");
         }
         else if (ch ==  '8') {
            btn_cnt_8++;
            term_send_str_crlf("BUTTON 8 PRESSED");
         }
         else if (ch ==  '9') {
            btn_cnt_9++;
            term_send_str_crlf("BUTTON 9 PRESSED");
         }
         else if (ch ==  '0') {
            btn_cnt_0++;
            term_send_str_crlf("BUTTON 0 PRESSED");
         }
         else if (ch ==  'A') {
            shift++;     
            term_send_str_crlf("SHIFT++");
            //char buffer;
            //sprintf( buffer, "%d", shift );
            //buffer = shift;
            LCD_write_string("SHIFT++");
            delay_ms(200);
            LCD_write_string(str);
         }
         else if (ch ==  'B') {
            shift--;     
            term_send_str_crlf("SHIFT--");
            LCD_write_string("SHIFT--");
            delay_ms(200);
            LCD_write_string(str);
         }
      } else {
         if (ch == 'C' ) {                            // code

            code(str);
            //str[0] = '\0';
            term_send_str_crlf(str);
            char_cnt = 0;
         } 
         else if (ch == 'D' ) {                       // decode

            decode(str);
            term_send_str_crlf(str);
            char_cnt = 0;
         } 
      }
    }
    return 0;
}

/*******************************************************************************
 * Osetreni nekolikanasobneho stisknuti tlacitka pri preruseni 
*******************************************************************************/
void button_handler(char input_char) {

   //i_ch = 'A';
   str[index] = input_char;
   str[index+1] = '\0';
   index++;
   term_send_str_crlf(str);
   term_send_str_crlf(input_char);
   //_DINT();                                  // zakaz preruseni 
   LCD_clear();
   LCD_write_string(str);
   char_cnt++;
   //_EINT();                                  // povoleni preruseni 
}

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{

}

/*******************************************************************************
 * Dekodovani uzivatelskych prikazu a jejich vykonavani
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
   return (CMD_UNKNOWN);
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized() {
    LCD_init();
    //LCD_append_string("C - code        ");
    //LCD_append_string("D - decode      ");
    //LCD_send_cmd(LCD_DISPLAY_ON_OFF | LCD_DISPLAY_ON | LCD_CURSOR_OFF, 0);
}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{

   initialize_hardware();
   keyboard_init();
   WDG_stop();
   term_send_str_crlf("Sifrovaci stroj spusten!");
   set_led_d5(1);  //rozsvitit LED D5

   // nastaveni casovace
   TBCCTL0 = CCIE;
   TBCCR0 = 0xFFFE;
   TBCTL = TBSSEL_1 + MC_1;

   last_ch = 0;
   char_cnt = 0;
   
   while (1) {
      keyboard_idle();	// obsluha klavesnice
      terminal_idle();  // obsluha terminalu
   }

}

/*******************************************************************************
 * Obsluha preruseni 
*******************************************************************************/
interrupt (TIMERB0_VECTOR) Timer_B (void) {


   // button 2     
   if ( btn_cnt_2 == 1 ) {
      button_handler('A');
   }
   if ( btn_cnt_2 == 2 ) {
      button_handler('B');
   }
   if ( btn_cnt_2 == 3 ) {
      button_handler('C');
   }

   // button 3
   if ( btn_cnt_3 == 1 ) {
      button_handler('D');
   }
   if ( btn_cnt_3 == 2 ) {
      button_handler('E');
   }
   if ( btn_cnt_3 == 3 ) {
      button_handler('F');
   }
   
   // button 4
   if ( btn_cnt_4 == 1 ) {
      button_handler('G');
   }
   if ( btn_cnt_4 == 2 ) {
      button_handler('H');
   }
   if ( btn_cnt_4 == 3 ) {
      button_handler('I');
   }
   
   // button 5
   if ( btn_cnt_5 == 1 ) {
      button_handler('J');
   }
   if ( btn_cnt_5 == 2 ) {
      button_handler('K');
   }
   if ( btn_cnt_5 == 3 ) {
      button_handler('L');
   }

   // button 6
   if ( btn_cnt_6 == 1 ) {
      button_handler('M');
   }
   if ( btn_cnt_6 == 2 ) {
      button_handler('N');
   }
   if ( btn_cnt_6 == 3 ) {
      button_handler('O');
   }
   
   // button 7
   if ( btn_cnt_7 == 1 ) {
      button_handler('P');
   }
   if ( btn_cnt_7 == 2 ) {
      button_handler('R');
   }
   if ( btn_cnt_7 == 3 ) {
      button_handler('S');
   }
   
   // button 8
   if ( btn_cnt_8 == 1 ) {
      button_handler('T');
   }
   if ( btn_cnt_8 == 2 ) {
      button_handler('U');
   }
   if ( btn_cnt_8 == 3 ) {
      button_handler('V');
   }

   // button 9
   if ( btn_cnt_9 == 1 ) {
      button_handler('W');
   }
   if ( btn_cnt_9 == 2 ) {
      button_handler('X');
   }
   if ( btn_cnt_9 == 3 ) {
      button_handler('Y');
   }
   
   // button 0
   if ( btn_cnt_0 == 1 ) {
      button_handler('Q');
   }
   if ( btn_cnt_0 == 2 ) {
      button_handler('Z');
   }
   btn_cnt_2 = 0;
   btn_cnt_3 = 0;
   btn_cnt_4 = 0;
   btn_cnt_5 = 0;
   btn_cnt_6 = 0;
   btn_cnt_7 = 0;
   btn_cnt_8 = 0;
   btn_cnt_9 = 0;
   btn_cnt_0 = 0;
   flip_led_d5();
   TBCCR0 += 0xFFFE;
   i_ch = '\0';
}
