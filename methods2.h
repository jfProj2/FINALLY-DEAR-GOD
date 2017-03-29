/**
PROJECT 2
**************

File: methods2.h

Date: 3/28/2017

Authors: Ryan Joiner (810638856)
         Emma Fick   (810609119)

Prof. Michael Cotterell
**************

This is the second supporting method file for Project2.
It handles running the editor, menu logic, typing and printing file contents.
**/

#ifndef METHODS2_H
#define METHODS2_H

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

#include "methods_main.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

using namespace std;


//these functions handle running editor, typing/printing file contents
void run_editor(const char * fileptr);
void data_to_screen(string data, WINDOW * win, int h, int w);
string add_char(WINDOW *win, int &x , int &y , int &ux, int &uy, int ch, string data, bool &saved);
string remove_char(string s);
void print_line_nums(WINDOW *win, int height, int x);


/**Run the text editor with command line arg 
 */
void run_editor(const char *fileptr){
  WINDOW *term_win;
  WINDOW *box_win; 
  WINDOW *edit_win;
  string filename = get_filename(fileptr);
  bool saved = false;
  initscr();
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
   init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
  cbreak();
  cbreak();
  noecho();
  int term_h, term_w;
  getmaxyx(stdscr, term_h, term_w);
  int menu_h = term_h/2;
  int menu_w = term_w/2;
  int menu_x = (term_w / 4), menu_y = (term_h / 4);
  int box_x = 1, box_y = 1;
  int edit_x = 1, edit_y = 4;
  int box_h = (term_h * 0.95);
  int box_w = (term_w * 0.95);
  int edit_h = box_h - 2;
  int edit_w = (box_w * 0.95);
  term_win = newwin(term_h, term_w, 0, 0);
  wattron(term_win,COLOR_PAIR(1));
  box_win = derwin(term_win, box_h, box_w, box_x, box_y);
  edit_win = derwin(box_win, edit_h, edit_w, edit_x, edit_y);
  wattron(box_win,COLOR_PAIR(3));
  wattron(edit_win,COLOR_PAIR(2));
  int ex = 2;
  int ey = 2;
  touchwin(edit_win);
  keypad(term_win, true);
  keypad(box_win, false);
  keypad(edit_win, true);
  refresh();
  mvwprintw(term_win, 0, 0, "F1: Menu/Cancel");
  mvwprintw(term_win, 0, (term_w/2)-8, "CSCI 1730 Editor");
  mvwprintw(term_win, (term_h-1), 0, filename.c_str());
  box(box_win, 0, 0);
  
  string text = load_file(filename);
  data_to_screen(text, edit_win, edit_h, edit_w);
  wrefresh(term_win);
  wrefresh(box_win);
  wrefresh(edit_win);
  int key = 1;
  bool running = true;
  int ux = 2;
  int uy = 2;
  while(running){
    box(box_win, 0, 0);
    print_line_nums(box_win, box_h, 1);
    wclear(term_win);
    wclear(box_win);
    box(box_win, 0,0);
    mvwprintw(term_win, 0, 0, "F1: Menu/Cancel");
    mvwprintw(term_win, 0, (term_w/2)-8, "CSCI 1730 Editor");
    mvwprintw(term_win, (term_h-1), 0, (filename.c_str()));
    print_line_nums(box_win, box_h, 1);
    data_to_screen(text, edit_win, edit_h, edit_w);
    wrefresh(box_win);
    wrefresh(edit_win);
    key = wgetch(edit_win);
   
    text = add_char(edit_win,ex, ey, ux, uy, key, text, saved);
    box(box_win, 0, 0);
    print_line_nums(box_win, box_h, 1);
    wrefresh(term_win);
    wrefresh(box_win);
    wrefresh(edit_win);
    touchwin(term_win);
    data_to_screen(text, edit_win, edit_h, edit_w);
    print_line_nums(box_win, box_h, 1);
    refresh();
    switch(key){
    	case KEY_F(1):
	  int selection = show_menu_window(menu_h, menu_w, menu_y, menu_x);
      wrefresh(term_win);
      if(selection == 0) {
        wrefresh(term_win);
        wrefresh(edit_win);
      } else if(selection == 1){
	int savePrompt = 0;
	bool success = true;
	if(!saved){
	  savePrompt = show_save_window(menu_h, menu_w, menu_y, menu_x);
	  if(savePrompt == 1){
	    while(success && savePrompt != 0){
	      success = save_file(filename, text);
	      if(success){
		saved = true;
		savePrompt = 0;
	      } else {
		bool oppTry = show_error_window(3, menu_h, menu_w, menu_y, menu_x);
		if(oppTry) {
		  savePrompt = 2;
		} else {
		  success = save_file(filename, text);
		}
	      }
		
	  }
	}
	if(savePrompt == 0){
	  bool opened = false;
	  while(!opened){
	    string new_filename = "";
	    new_filename += show_open_window(menu_h, menu_w, menu_y, menu_x);
	    if(new_filename == ""){
	      saved = false;
	      break;
	    }
	    saved = true;
	    box(box_win, 0, 0);
	    refresh();
	    wrefresh(term_win);
	    if(file_exists(new_filename)){
	      text = load_file(new_filename);                       
	      data_to_screen(text, edit_win, edit_h, edit_w);
	      mvwprintw(term_win, (term_h-1), 0, new_filename.c_str());
	      filename = new_filename;
	      opened = true;
	       wrefresh(edit_win);
	       touchwin(term_win);
	       refresh();
	    } else {
	      opened = show_error_window(1, menu_h, menu_w, menu_y, menu_x);
	      wrefresh(edit_win);      
	    }
	  }
	  mvwprintw(term_win, (term_h-1), 0, filename.c_str());
	  refresh();
	  wrefresh(edit_win);
	  touchwin(term_win);
         } else { 
	  wclear(edit_win);
	   refresh();
	   wrefresh(term_win);
	   touchwin(term_win);
	   data_to_screen(text, edit_win, edit_h, edit_w);
        }
	}
      }else if(selection == 2){
	bool success = false;
	bool run = true;
	if(strcmp(filename.c_str(), "<N/A>") == 0){
	  filename = "";
	  filename += show_saveas_window(menu_h, menu_w, menu_y, menu_x);    
	}
	while(!success && run){
	  success = save_file(filename, text);
	  if(success){
	    saved = true;
	    data_to_screen(text, edit_win, edit_h, edit_w);
	    wclear(edit_win);
	    refresh();
	    wrefresh(edit_win);
	    touchwin(term_win);
	    mvwprintw(term_win, (term_h-1), 0, filename.c_str());
	    data_to_screen(text, edit_win, edit_h, edit_w);
	    refresh();
	    wrefresh(edit_win);
	    touchwin(term_win);
	  } else {
	      bool oppTry = show_error_window(3, menu_h, menu_w, menu_y, menu_x);
	      if(oppTry) {
		run = false;
	      }else {
		success = save_file(filename, text);
	      }
	  }

	}
      }
      else if(selection == 3){
	bool overwrite = true;
	while(!saved && overwrite){
	  filename = "";
	  filename += show_saveas_window(menu_h, menu_w, menu_y, menu_x);
	  if(!(file_exists(filename))){
	    save_file(filename, text);                   
	    saved = true;
	    refresh();
	    data_to_screen(text, edit_win, edit_h, edit_w);
	    wclear(edit_win);
	    wrefresh(edit_win);
	    touchwin(term_win);
	    mvwprintw(term_win, (term_h-1), 0, filename.c_str());
	    data_to_screen(text, edit_win, edit_h, edit_w);
	    wrefresh(edit_win);
	    touchwin(term_win);
	    refresh();
	  } else {
	    overwrite = show_error_window(2, menu_h, menu_w, menu_y, menu_x);
	    if(!overwrite){
	      save_file(filename, text);                  
	      saved = true;
	      data_to_screen(text, edit_win, edit_h, edit_w);
	      wclear(edit_win);
	      wrefresh(edit_win);
	      touchwin(term_win);
	      mvwprintw(term_win, (term_h-1), 0, filename.c_str());
	      data_to_screen(text, edit_win, edit_h, edit_w);
	      wrefresh(edit_win);
	      touchwin(term_win);
	      refresh();
	    } else {
	      saved = false;
	      refresh();               
	      wrefresh(term_win);
	      data_to_screen(text, edit_win, edit_h, edit_w);
	      mvwprintw(term_win, (term_h-1), 0, filename.c_str());    
	      wrefresh(edit_win);
	      touchwin(term_win);
	      refresh();
	    }
	  }
	}
	mvwprintw(term_win, (term_h-1), 0, filename.c_str());
	refresh();
	wrefresh(edit_win);
	touchwin(term_win);
      }
      else if(selection == 4){
	int savePrompt = show_save_window(menu_h, menu_w, menu_y, menu_x);
	    if(savePrompt == 0){
	      running = false;
	    }
	    else if(savePrompt == 1){
	      if(strcmp(filename.c_str(), "<N/A>") == 0){
		filename = "";
		filename += show_saveas_window(menu_h, menu_w, menu_y, menu_x);
	      }
	      save_file(filename, text);
	      saved = true;
	      running = false;	    }
	    else {
	      wclear(edit_win);
	      box(edit_win, 0, 0);
	      refresh();
	      wrefresh(term_win);
	      touchwin(term_win);
	      data_to_screen(text, edit_win, edit_h, edit_w);
	    }
	  }
    }
  }
  delwin(edit_win);
  delwin(box_win);
  delwin(term_win);
  endwin();
}


/**Takes in string of file contents and uses xy-coordinates to print 
   it on the specified window with dimensions height, width */
void data_to_screen(string data, WINDOW * win, int height, int width){
  int x = 1;
  int y = 1;
  int w = width - 1;
  for(unsigned int i = 0; i < data.size(); i++){
    char ch = data[i];
    mvwprintw(win, y, x, "%c", ch);
    if(x == (w - 1) || ch == '\n'){
      y++;
      x = 0;
    }
    x++;
  }
  refresh();
  wrefresh(win);
  touchwin(win);
}


/**Returns string plus character just typed
   Constantly edits the text based on key input */
string add_char(WINDOW * win, int& x, int& y, int &ux, int &uy, int ch, string s, bool &saved){
  int max_x, max_y;
  getmaxyx(win, max_y, max_x);
  string data = "";
  if(ch == KEY_BACKSPACE|| ch == 127){
    if(s.size() > 0){
      data = remove_char(s);
      if(x == 2 && y > 2){
        x = max_x -1;
        y--;
      }
      else{
        x--;
      }
      saved = false;
    }
  }
  else if(ch == 10){
    if(y < max_y-1){
      y++;
      x = 2;
    }
    data = s + '\n';
    saved = false;
  } else if(ch == KEY_DOWN){
    if(y < max_y -1){
      y++;
      uy++;
    }
    data = s;
  } else if(ch == KEY_UP){
    if(y > 2){
      y--;
      uy--;
    }
    data = s;
  } else if(ch == KEY_LEFT){
    if(x > 2){
      x--;
      ux--;
    }
    data = s;
  } else if(ch == KEY_RIGHT){
    if(x < max_x - 1){
      x++;
      ux++;
    }
    data = s;
  } else {
    if(x < max_x -1)
      x++;
    if(x == max_x-1){
      y++;
      x = 2;
    }
    saved = false;
    data = s;
    data += ch;
  }
  return data;
}


/**Deals with backspace
   Returns post-backspace string  with last char removed */
string remove_char(string s){
  string data = s.substr(0,s.size()-1);
  return data;
}

/**Line numbers! */
void print_line_nums(WINDOW *win, int height, int x){
  for(int i = 1; i < height -1; i++){
    mvwprintw(win, i, x, "%i", i-1);
    wrefresh(win);
  }
}

#endif
