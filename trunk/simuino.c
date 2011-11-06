//================================================
//  Developed by Benny Saxen, ADCAJO
//================================================

#define HIST_MAX  100

#define DP 5
#define AP 15
#define RF 0
#define ER 1
#define SR 20

#define ON     1
#define OFF    0

#define YES    10
#define NO     20

// Configuration
int stepStep = 0;


// Init
int nloop         = 0;
int timeFromStart = 0;


void stepCommand();

#include "simuino_lib.c"
#include "arduino_lib.c"
#include "sketch/sketch.pde"


// ----- Commands ----------

void runSim(int n)
{
  int i;
  int currentLoop = nloop;

  if(n == 1)
    {
      nloop++;
      loop();
      Serial.flush();
    }
  else if(n > 1)
    {
      for(i=0;i<n-currentLoop;i++)
	{
	  nloop++;
	  wmove(com,1,1);
	  wprintw(com,"Loop:     %d",nloop);
	  wrefresh(com); 

	  loop();
	  Serial.flush();
	}
    }
  return;
}    

void stepCommand()
{
  int ch;

  wmove(com,1,1);
  wprintw(com,"Loop:     %d",nloop);
  wmove(com,2,1);
  wprintw(com,"Step:     %d",timeFromStart);
  wrefresh(com);
  if(stepStep == 1)
    {
      ch = getchar();
      if (ch=='q')stepStep = 0;
    }
}


//========================================
int main(int argc, char *argv[])
{
  int i,x;
  int ch;
  int nhist;

  getAppName(appName);

  initscr();
  clear();
  noecho();
  cbreak();

  /* record kb modes */
  tcgetattr(0, &orig);
  nnew = orig;
  nnew.c_lflag &= ~ICANON;
  nnew.c_lflag &= ~ECHO;
  nnew.c_lflag &= ~ISIG;
  nnew.c_cc[VMIN] = 1;
  nnew.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &nnew);

  getmaxyx(stdscr,row,col);
  start_color();
  init_pair(1,COLOR_BLACK,COLOR_BLUE);
  init_pair(2,COLOR_BLACK,COLOR_GREEN);
  init_pair(3,COLOR_BLUE,COLOR_WHITE); 
  init_pair(4,COLOR_RED,COLOR_WHITE); 
  init_pair(5,COLOR_MAGENTA,COLOR_WHITE); 
  
/*     COLOR_BLACK   0 */
/*     COLOR_RED     1 */
/*     COLOR_GREEN   2 */
/*     COLOR_YELLOW  3 */
/*     COLOR_BLUE    4 */
/*     COLOR_MAGENTA 5 */
/*     COLOR_CYAN    6 */
/*     COLOR_WHITE   7 */

// Board Window    
  uno=newwin(AP+4,70,0,0);
  wbkgd(uno,COLOR_PAIR(3));

  wmove(uno,DP-1,RF);waddch(uno,ACS_ULCORNER); 
  wmove(uno,DP-1,RF+60);waddch(uno,ACS_URCORNER); 
  wmove(uno,AP+1,RF);waddch(uno,ACS_LLCORNER); 
  wmove(uno,AP+1,RF+60);waddch(uno,ACS_LRCORNER); 
  for(i=1;i<60;i++)
    {
      wmove(uno,DP-1,RF+i);
      waddch(uno,ACS_HLINE);
      wmove(uno,AP+1,RF+i);
      waddch(uno,ACS_HLINE);
    }
  for(i=DP;i<AP+1;i++)
    {
      wmove(uno,i,RF);
      waddch(uno,ACS_VLINE);
      wmove(uno,i,RF+60);
      waddch(uno,ACS_VLINE);
    }

  // Pin positions on the board
  for(i=0;i<14;i++)digPinPos[13-i] = RF+4+4*i;
  for(i=0;i<6;i++) anaPinPos[i] = RF+26+5*i;

  for(i=0;i<14;i++){wmove(uno,DP+1,digPinPos[i]-2); wprintw(uno,"%3d",i);}
  for(i=0;i<14;i++){wmove(uno,DP,digPinPos[i]); waddch(uno,ACS_BULLET);}
  wmove(uno,DP+5,RF+6); wprintw(uno,"Sketch: %s",appName);  
  for(i=0;i<6;i++){wmove(uno,AP-1,anaPinPos[i]-1); wprintw(uno,"A%1d",i);}
  for(i=0;i<6;i++){wmove(uno,AP,anaPinPos[i]); waddch(uno,ACS_BULLET);}

  wmove(uno,0,5); 
  wprintw(uno,"SIMUINO - Arduino UNO Pin Analyzer v0.4");
  wrefresh(uno);

  // Serial Window
  ser=newwin(5,70,AP+5,0);
  wbkgd(ser,COLOR_PAIR(5));
  wmove(ser,0,0); 
  wprintw(ser,"[Serial Interface]");
  wrefresh(ser);

  // Log Window
  logSize = row;
  slog=newwin(row,29,0,col-30);
  wbkgd(slog,COLOR_PAIR(4));
  wrefresh(slog);

  // Command Window
  com=newwin(AP+10,20,0,72);
  wbkgd(com,COLOR_PAIR(4));
  wmove(com,0,0);

  wrefresh(com);


  boardInit();
  setup();

  nhist = readExt();
  wmove(com,0,1);
  wprintw(com,"Scenario: %d loops",nhist);
  wrefresh(com); 

  while((ch!='q')&&(ch!='Q'))  
    {
      ch = getchar();


      wmove(com,1,1);

      if (ch=='r')
	{
	  wprintw(com,"Loop:     %d",nloop+1);
	  wrefresh(com); 
	  runSim(1);
	}
      if (ch=='s') 
	{
	  //wprintw(com,"%c step %d",ch,timeFromStart);
	  //wrefresh(com); 
	  stepStep = 1;
	  runSim(1); 
	}
      if (ch=='g')
	{
	  //wprintw(com,"%c scenario %d",nloop+1,nhist);
	  //wrefresh(com); 
	  runSim(nhist);   
	}
    }
  
  tcsetattr(0,TCSANOW, &orig);
  delwin(uno);
  endwin();
}


