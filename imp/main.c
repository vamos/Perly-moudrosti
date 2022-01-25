/*******************************************************************************
author: Libor Dvoracek
login:  xdvora2t
title:  Caesar cypher device
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
int char_cnt;                    // max 32 chars
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

int shift = 0;


int code(char *input, int shift_in) {

   LCD_clear();
   int i;
   char tmp_ch;
   for ( i = 0; input[i] != '\0'; ++i ) {
      tmp_ch = input[i];
      if ( tmp_ch >= 'A' && tmp_ch <= 'Z' ) {

         tmp_ch = tmp_ch + shift;
         if (tmp_ch > 'Z' ) {
            int nr_of_abcs = shift / 26;
            tmp_ch = tmp_ch - (26*nr_of_abcs);
         }
         input[i]=tmp_ch;
      }
   }
   LCD_append_string(input);
   return 0;
}


int keyboard_idle() {

    ch = key_decode(read_word_keyboard_4x4());
    if (ch != last_ch) {
      last_ch = ch;

      if (ch == 'C' ) { // code
         code(str, shift);
         term_send_str_crlf(str);
      }
      else if (ch ==  '*') { // restart
         fpga_initialized();

         strcpy(str, "");
         term_send_str_crlf(str);
      }
      else if (ch ==  '#') { // help
         fpga_initialized();
         LCD_append_string("C=code D=decode shift=0 +-A/B");
      }
      else if (ch ==  '2') {
         btn_cnt_2++;
         term_send_str_crlf("btn_cnt_2++");
      }
      else if (ch ==  '3') {
         btn_cnt_3++;
         term_send_str_crlf("btn_cnt_3++");
      }
      else if (ch ==  'A') {
         shift++;
         term_send_str_crlf("shift++");
      }
      else if (ch ==  'B') {
         shift--;
         term_send_str_crlf("shift--");
      }
    }
    return 0;
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


   if ( btn_cnt_2 == 1 ) {
      i_ch = 'A';
      str[index] = i_ch;
      str[index+1] = '\0';
      index++;
      term_send_str_crlf(str);
      term_send_str_crlf("A");
      LCD_append_char('A');
   }
   if ( btn_cnt_2 >= 2 ) {
      i_ch = 'B';
      str[index] = i_ch;
      str[index+1] = '\0';
      index++;
      term_send_str_crlf(str);
      term_send_str_crlf("B");
      LCD_append_char('B');
   }
   if ( btn_cnt_3 == 1 ) {
      i_ch = 'D';
      str[index] = i_ch;
      str[index+1] = '\0';
      index++;
      term_send_str_crlf(str);
      term_send_str_crlf("D");
      LCD_append_char('D');
   }
   if ( btn_cnt_3 >= 2 ) {
      i_ch = 'E';
      str[index] = i_ch;
      str[index+1] = '\0';
      index++;
      term_send_str_crlf(str);
      term_send_str_crlf("E");
      LCD_append_char('E');
   }
   btn_cnt_2 = 0;
   btn_cnt_3 = 0;
   flip_led_d5();
   TBCCR0 += 0xFFFE;
   i_ch = '\0';
}
