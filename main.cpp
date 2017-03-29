/**
PROJECT 2
**************

File: main.cpp

Date: 3/28/2017

Authors: Ryan Joiner (810638856)
         Emma Fick   (810609119)

Prof. Michael Cotterell
**************



This is the main function for Project2.
It takes in zero or one command line args
and uses these to run the awesome text editor!

**/

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

//##########
//PROTOTYPES
//##########
using namespace std;

//driver of program
int main(const int argc, const char * argv[]);

//IO related functions
bool file_exists(string filename);
bool save_file(string filename, string s);
string load_file(string filename);
bool close_file(string filename);
string get_filename(const char * fileptr);

//running editor and typing
void run_editor(const char * fileptr);
void data_to_screen(string data, WINDOW * win, int h, int w);
string add_char(WINDOW *win, int &x , int &y , int &ux, int &uy, int ch, string data, bool &saved);
string remove_char(string s);
void print_line_nums(WINDOW *win, int height, int x);

//windows, menus and user inputs with menus
int show_menu_window(int menu_h, int menu_w, int menu_y, int menu_x);
bool show_error_window(int code, int err_h, int err_w, int err_y, int err_x);
string show_open_window(int open_h, int open_w, int open_y, int open_x);
int show_save_window(int save_h, int save_w, int save_y, int save_x);
string show_saveas_window(int sa_h, int sa_w, int sa_y, int sa_x);


//########
//MAIN
//########

int main(const int argc, const char * argv[]){

  //No file: open editor to untitled blank file
  if(argc == 1){
    run_editor(nullptr);
  }
  //One file specified: editor opens to that file
  else if(argc == 2){
    run_editor(argv[1]);
  }
  //If >1 passed in, editor will not open and give this error mssg
  else{
    printf("ERROR: Too many arguments to ./1730ed\n");
    printf("Retry run command with proper syntax\n\n");
    printf("Ex: ./1730ed   or   ./1730ed filename.txt\n");
    return EXIT_FAILURE;
  }

  return 0;
}
