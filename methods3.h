/**
PROJECT 2
**************

File: methods3.h

Date: 3/28/2017

Authors: Ryan Joiner (810638856)
         Emma Fick   (810609119)

Prof. Michael Cotterell
**************

This is the third supporting method file for Project2.
It handles menu/form creation, pop up windows, errors, and processes user 
input 
**/

#ifndef METHODS3_H
#define METHODS3_H

//########
//INCLUDES
//########
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include <curses.h>
#include <ncurses.h>
#include <menu.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <form.h>

#include "methods_main.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
using namespace std;

/* These functions create and display all notification windows
   They all require dimensions and xy-coords
*/
int show_menu_window(int menu_h, int menu_w, int menu_y, int menu_x);
bool show_error_window(int code, int err_h, int err_w, int err_y, int err_x);
string show_open_window(int open_h, int open_w, int open_y, int open_x);
int show_save_window(int save_h, int save_w, int save_y, int save_x);
string show_saveas_window(int sa_h, int sa_w, int sa_y, int sa_x);


/*Returns an int based on user's choice
  0 = user exited with f(1)
  1 = open
  2 = save 
  3 = save as
  4 = exit
*/ 
int show_menu_window(int menu_h, int menu_w, int menu_y, int menu_x){
  WINDOW *menu_win;
  WINDOW *menu_subwin;
  MENU *main_menu;
  ITEM **items;
  initscr();

  const char * choices[] = {"Open","Save","Save As", "Exit"};
  int n_choices = ARRAY_SIZE(choices);
  items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
  for(int i = 0; i < n_choices; i++){
    items[i] = new_item(choices[i], NULL);
  }

  items[n_choices] = (ITEM*)NULL;
  main_menu = new_menu((ITEM**)items);
  menu_win = newwin(menu_h, menu_w, menu_y, menu_x);
  menu_subwin = derwin(menu_win, (menu_h/2), (menu_w/2), (menu_h*0.4), (menu_w/3));
  keypad(menu_win, true);
  set_menu_win(main_menu, menu_win);
  set_menu_sub(main_menu, menu_subwin);
  set_menu_mark(main_menu, "* ");
  box(menu_win, 0, 0);
  wrefresh(menu_win);

  cbreak();
  post_menu(main_menu);
  wrefresh(menu_win);

  int key = 1;
  int choice = 0;
  bool selected = false;

  while(key != KEY_F(1) && !selected){
    key = wgetch(menu_win);
    switch(key){
    case KEY_DOWN:
      menu_driver(main_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(main_menu, REQ_UP_ITEM);
      break;
    case 10:
      selected = true;
      break;
    }
    wrefresh(menu_win);
  }

  if(selected){
    ITEM *cur_item = current_item(main_menu);
    const char * it = item_name((const ITEM*)cur_item);
    if(strcmp(it,"Open") == 0){
      choice = 1;
    } else if (strcmp(it,"Save") == 0){
      choice = 2;
    } else if (strcmp(it,"Save As") == 0){
      choice = 3;
    } else if (strcmp(it,"Exit") == 0){
      choice = 4;
    }
  }

  unpost_menu(main_menu);
  wborder(menu_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  free_menu(main_menu);
  for(int i=0; i < n_choices; i++){
    free_item(items[i]);
  }
  delwin(menu_subwin);
  delwin(menu_win);
  return choice; 
}


/**Returns a bool representing if user chose yes or no 
   param: code - an int representing error type
*/
bool show_error_window(int code, int err_h, int err_w, int err_y, int err_x){
  WINDOW *error_win;
  WINDOW *error_subwin;
  MENU *error_menu;
  ITEM **items;
  bool try_again;
  initscr();
  const char * choices[] = {"Yes", "No"};
  int n_choices = ARRAY_SIZE(choices);
  items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
  for(int i = 0; i < n_choices; i++){
    items[i] = new_item(choices[i], NULL);
  }
  items[n_choices] = (ITEM*)NULL;
  error_menu = new_menu((ITEM**)items);
  error_win = newwin(err_h, err_w, err_y, err_x);
  error_subwin = derwin(error_win, (err_h/2), (err_w/2), (err_h*0.4), (err_w/3));
  keypad(error_win, true);
  set_menu_win(error_menu, error_win);
  set_menu_sub(error_menu, error_subwin);
  set_menu_mark(error_menu, "* ");
  box(error_win, 0, 0);
  string err_mssg = "";
  
  wrefresh(error_win);
  cbreak();

  //a switch statement assigning a message based on error code
  switch(code){
  case 1:
    err_mssg += "File Does Not Exist. Try Again?";
    break;
  case 2:
    err_mssg += "File Already Exists. Overwrite?";
    break;
  case 3:
    err_mssg += "Error Saving File. Try Again?";
    break;
  case 4:
    err_mssg += "Error Closing File. Try Again?";
    break;
  }
  
 
  wrefresh(error_win);
  mvwprintw(error_win, 3, 1, err_mssg.c_str());
  post_menu(error_menu);
  
  int key = 1;
  bool selected = false;
  while(key != KEY_F(1) && !selected){
    key = wgetch(error_win);
    switch(key){
    case KEY_DOWN:
      menu_driver(error_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(error_menu, REQ_UP_ITEM);
      break;
    case 10:
      selected = true;
      break;
    }
    wrefresh(error_win);
  }

  if(selected){
    ITEM *cur_item = current_item(error_menu);
    const char * it = item_name((const ITEM*)cur_item);
    if(strcmp(it,"Yes") == 0){
      try_again = false;
    } else if (strcmp(it,"No") == 0){
      try_again = true;
    } 
  }
  wborder(error_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(error_win);
  wrefresh(error_win);
  wrefresh(error_win);
  unpost_menu(error_menu);
  free_menu(error_menu);
  for(int i=0; i < n_choices; i++){
    free_item(items[i]);
  }
  delwin(error_subwin);
  delwin(error_win);
  return try_again; 
}


/**Returns string of file name to open
   Uses form to retrieve text 
*/
string show_open_window(int open_h, int open_w, int open_y, int open_x){
  WINDOW *open_win;
  WINDOW *open_subwin;
  FORM *form;
  string filename = "";
  FIELD *field[2];
  initscr();
  cbreak();
  keypad(stdscr, true);
  field[0] = new_field(1, open_w/2, 0, 0, 0, 0);
  field[1] = NULL;
  set_field_back(field[0], A_UNDERLINE);
  form = new_form(field);
  open_win = newwin(open_h, open_w, open_y, open_x);
  open_subwin = derwin(open_win, (open_h/2), (open_w/2), (open_h*.4), (open_w/4));
  keypad(open_win, true);
  set_form_win(form, open_win);
  set_form_sub(form, open_subwin);
  box(open_win, 0, 0);
  post_form(form);
  mvwprintw(open_win, 1, open_w/2 -9, "Enter File to Open");
  mvwprintw(open_win, 2, open_w/2 -10, "And Press Enter Twice");
  wrefresh(open_win);
  refresh();
  int ch;
  bool selected = false;
  while((ch = wgetch(open_win)) != KEY_F(1) && !selected){
    if(ch >= 32 && ch < 127){
      form_driver(form, ch);
      filename += ch;
    }
    else if(ch == KEY_BACKSPACE || ch == 127){
      ch = REQ_PREV_CHAR;
      form_driver(form, ch);
      ch = REQ_DEL_CHAR;
      form_driver(form, ch);
      filename = filename.substr(0, filename.size()-1);
    }
    else if(ch == 10){
      selected = true;
    }
  }    
  unpost_form(form);
  free_form(form);
  free_field(field[0]);
  free_field(field[1]);
  wborder(open_win,' ',' ',' ',' ',' ',' ',' ',' ');
  wclear(open_win);
  wclear(open_subwin);
  delwin(open_win);
  delwin(open_subwin);
  refresh();
  return filename; 
}


/**Returns an int representing user response to save prompt
   0 = no
   1 = yes
   2 = cancel
*/
int show_save_window(int save_h, int save_w, int save_y, int save_x){
  WINDOW *save_win;
  WINDOW *save_subwin;
  MENU *save_menu;
  ITEM **items;
  initscr();
  const char * choices[] = {"Yes", "No", "Cancel"};
  int n_choices = ARRAY_SIZE(choices);
  items = (ITEM**)calloc(n_choices+1, sizeof(ITEM*));
  for(int i = 0; i < n_choices; i++){
    items[i] = new_item(choices[i], NULL);
  }
  items[n_choices] = (ITEM*)NULL;
  save_menu = new_menu((ITEM**)items);
  save_win = newwin(save_h, save_w, save_y, save_x);
  mvwprintw(save_win, 1, (save_w/2 - 9),"Do you wish to save?");
  save_subwin = derwin(save_win, (save_h/2), (save_w/2), (save_h*0.4), (save_w/3));
  keypad(save_win, true);
  set_menu_win(save_menu, save_win);
  set_menu_sub(save_menu, save_subwin);
  set_menu_mark(save_menu, "* ");
  box(save_win, 0, 0);
  wrefresh(save_win);
  refresh();
  cbreak();
  post_menu(save_menu);
  wrefresh(save_win);
  int key = 1;
  int choice = 0;
  bool selected = false;
  while(!selected){
    key = wgetch(save_win);
    switch(key){
    case KEY_DOWN:
      menu_driver(save_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(save_menu, REQ_UP_ITEM);
      break;
    case 10:
      selected = true;
      break;
    }
    wrefresh(save_win);
  }
  if(selected){
    ITEM *cur_item = current_item(save_menu);
    const char * it = item_name((const ITEM*)cur_item);
    if(strcmp(it,"No") == 0){
      choice = 0;
    } else if (strcmp(it,"Yes") == 0){
      choice = 1;
    } else if (strcmp(it,"Cancel") == 0){
      choice = 2;
    }
  }
  wborder(save_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  mvwprintw(save_win, save_y+10, save_x+10,"");
  wrefresh(save_win);
  wrefresh(save_win);
  wrefresh(save_win);
  unpost_menu(save_menu);
  free_menu(save_menu);
   for(int i=0; i < n_choices; i++){
    free_item(items[i]);
  }
  wclear(save_win);
  wclear(save_subwin);
  refresh();
  delwin(save_subwin);
  delwin(save_win);
  return choice; 
}


/**Returns string of file name to use to save with
   Uses form to retrieve text 
*/
string show_saveas_window(int sa_h, int sa_w, int sa_y, int sa_x){
  WINDOW *sa_win;
  WINDOW *sa_subwin;
  FORM *form;
  string filename = "";
  FIELD *field[2];
  initscr();
  cbreak();
  keypad(stdscr, true);
  field[0] = new_field(1, sa_w/2, 0, 0, 0, 0);
  field[1] = NULL;
  set_field_back(field[0], A_UNDERLINE);
  form = new_form(field);
  sa_win = newwin(sa_h, sa_w, sa_y, sa_x);
  sa_subwin = derwin(sa_win, (sa_h/2), (sa_w/2), (sa_h*.4), (sa_w/4)); 
  keypad(sa_win, true);
  set_form_win(form, sa_win);
  set_form_sub(form, sa_subwin);
  box(sa_win, 0, 0);
  post_form(form);
  mvwprintw(sa_win, 1, sa_w/2 -9, "Enter Name To Save As");
  mvwprintw(sa_win, 2, sa_w/2 -9, "And Press Enter Twice");
  wrefresh(sa_win);
  refresh();
  int ch;
  bool selected = false;
  while((ch = wgetch(sa_win)) != KEY_F(1) && !selected){
    if(ch >= 32 && ch < 127 && ch != KEY_BACKSPACE){
      form_driver(form, ch);
      filename += ch;
    }
    else if(ch == KEY_BACKSPACE || ch==127){
      ch = REQ_PREV_CHAR;
      form_driver(form, ch);
      ch = REQ_DEL_CHAR;
      form_driver(form, ch);
      filename = filename.substr(0, filename.size()-1);
    }
    else if(ch == 10){
      selected = true;
    }
  }
  unpost_form(form);
  free_form(form);
  free_field(field[0]);
  free_field(field[1]);
  delwin(sa_win);
  delwin(sa_subwin);
  wborder(sa_win,' ',' ',' ',' ',' ',' ',' ',' ');
  clear();
  return filename; 
}




#endif


