/*Time-stamp: <2014-12-09 16:33:50 root> */

/*************************************************************************
 *      Raspberry Pi Encoder and DC Load Controlling Programm            *
 *     --------------------------------------------------------          *
 *  This Program reads the values of a Maxon HEDS 5540 Motor Encoder     *
 *  and certain values of a Maynuo M9812 DC Load.                        *
 *  It's based on libWiringPi by Gordon Henderson and libmodbus by       *  
 *  Stéphan Raimbault. It's free software according to GPL v.2           *
 *              by Dominik Strebel dstrebel@hsr.ch                       *
 ************************************************************************/


#include<wiringPi.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<modbus/modbus.h>
#include<signal.h>
#include<sched.h>

#define MY_PRIORITY (35)


static FILE *fh,*csv; //Filehandle
static volatile long int counter=0; //RPM Counter 
static uint16_t reg[160]; //Maynuo DC Load Register
static modbus_t *ctx; //Modbus Connection Handler
static struct timeval start, diff, end;

/*
  Set Scheduler Priority, if no success, exit program.
*/
void rtsched(){
  struct sched_param param;
  param.sched_priority = MY_PRIORITY;
  if (sched_setscheduler(0, SCHED_FIFO, &param)== -1)
    {
      exit(-1);
    }
}



/*-----------------------------------------------------------------------------------------------------*/

/*
  Signalhandler um das Programm graceful zu beenden
  Schliesst das File mit einem Time/Date Stempel ab.
*/

void handler(int sig){
  time_t t;
  struct tm *tnow;
  time(&t);
  tnow=localtime(&t);
  gettimeofday(&end,0);
  timersub(&end,&start,&diff);
  if (sig == SIGINT){
    printf("Signal caught\nProgramm wird beendet.\n");
    fclose(csv);
    modbus_close(ctx);
    modbus_free(ctx);
    exit(0);
    
  }
}
/*-----------------------------------------------------------------------------------------------------*/

/*
  Open Connection to Mayno Load with modbus protocol
  Check for errors
*/

int openLoad(){
  ctx=modbus_new_rtu("/dev/ttyUSB0",115200,'N',8,1);
  if (modbus_connect(ctx) == -1){
    printf("Connection Problems\n");
    fflush(stdin);
    modbus_free(ctx);
    return -1;     
  }  
  else return 1;
}

/*-----------------------------------------------------------------------------------------------------*/

/*
  Read Register from Load.
  Check for errors.
*/

int readRegister(int addr){
  int i;
  i=modbus_read_registers(ctx,addr,31,reg);
  if (i == -1){
    printf("%s\n",modbus_strerror(errno));
    exit(-1);
  }
  return i;
}

/*------------------------------------------------------------------------------------------------------*/


/*
  Convert double Register from  uint16_t to floating point value by memcopy
*/

float convfl(uint16_t *tab, int idx){
  uint32_t a;
  float f;
  a = (((uint32_t)tab[idx]) << 16) + tab[idx+1];
  memcpy(&f, &a, sizeof(float));
  return f;
}

/*-------------------------------------------------------------------------------------------------------*/

/*
  Interrupthandler counts interrupts from selected RaspiPi Pin.
*/

void interrupt (void) {
  counter++;  
}

/*-------------------------------------------------------------------------------------------------------*/

/*
  Write fileheader with date/timestamp und selected delay und multiplicator values.
*/ 

void fileheaderdat(char *argv[]){
  time_t t;
  struct tm *tnow;
  char filename[200];
  time(&t);
  tnow=localtime(&t);
  snprintf(filename, sizeof(filename), "csv/%02d-%02d-%02d_%02d-%02d-%02d.csv",tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec);
  csv=fopen(filename,"a");
  fprintf(csv,"Datum;Timer;RPM;RPS;I;U;P;R\n");

  fflush(csv);
}

/*--------------------------------------------------------------------------------------------------------------------------*/

/*
  Write fileheader with argv[8].
*/
void fileheader(char *argv[]){
  time_t t;
  struct tm *tnow;
  char filename[200];
  time(&t);
  tnow=localtime(&t);
  snprintf(filename, sizeof(filename), "csv/%s.csv",argv[7]);
  csv=fopen(filename,"a");
  fprintf(csv,"Datum;Timer;RPM;RPS;I;U;P;R\n");

  fflush(csv);
}






/*
  Setup Raspi pi with wiringPi Library
*/


void pisetup(){
  wiringPiSetup();
  pinMode(2, INPUT);
  pullUpDnControl(2, PUD_UP);
  wiringPiISR(2,INT_EDGE_FALLING, &interrupt);
}

/*-----------------------------------------------------------------------------------------------------------------------------*/


/*
  Program without load!
*/

int withoutLoad(char *argv[]){
  float rps=0,rpm=0;
  time_t t;
  struct tm *tnow;
  for(;;){
    printf("\e[1;1H\e[2J");
    puts("Messung startet:");
    for(;;){
      rps=(counter/atof(argv[4]))*(1000/atof(argv[2]));
      rpm=rps*60;
      gettimeofday(&end,0);
      timersub(&end,&start,&diff);
      printf("RPM: %f\nRPS: %f\n",rpm, rps);
      counter = 0;
      printf("\e[1;1H\e[2J");
      time(&t);
      tnow=localtime(&t);
      fprintf(csv,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%fl;%fl;;;;\n", tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps);
      fflush(csv);
      delay(atoi(argv[2]));
    }
  }
}


/*---------------------------------------------------------------------------------------------------------------------------*/



/*
  Mess Loop with Load
*/

int withLoad(char *argv[]){
  float rps=0,rpm=0,i=0,u=0,p=0,r=0;
  time_t t;
  struct tm *tnow;
  int rc=0;

  for(;;){
    printf("\e[1;1H\e[2J");
    puts("Messung startet:");
    for(;;){
      
      rc=readRegister(0x0B00);
      rps=(counter/atof(argv[4]))*(1000/atof(argv[2]));
      rpm=rps*60;
      i=convfl(&reg[2],0);
      u=convfl(&reg[0],0);
      //p=convfl(&reg[5],0);
      //r=convfl(&reg[7],0);
      p=i*u;
      gettimeofday(&end,0);
      timersub(&end,&start,&diff);
      printf("RPM: %f\nRPS: %f\n",rpm, rps);
      //printf("I: %f oA \nU: %f V \nP: %f W \nR: %f Ohm \n", i,u,p,r);
      printf("I: %f A \nU: %f V \nP: %f W \n",i,u,p);
      counter = 0;
      printf("\e[1;1H\e[2J");
      time(&t);
      tnow=localtime(&t);
      fprintf(csv,"%02d.%02d.%02d %02d:%02d:%02d;%li.%0li;%f;%f;", tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm, rps);
      //fprintf(csv,"%f;%f;%f;%f\n",i,u,p,r);
      fprintf(csv,"%f;%f;%f\n",i,u,p);
      fflush(csv);  
      delay(atoi(argv[2]));
    }
  }
}
/*----------------------------------------------------------------------------------------------------------------------------*/



/*
  Main Programm.
  Checks arguments. runs main loops.
*/

int main(int argc, char *argv[] ) {
  struct timeval response_time;
  if (argc == 1) {
    printf("+---------------------------------------------------------------------------+\n");
    printf("| Dieses Programm berechnet die Umdrehungen über den                        |\n");
    printf("| Interrupt an anzugebenden Pin                                             |\n");
    printf("| Vorläufige Syntax: ./encoder  -delay [ms] -mult [mul] -l [y|n] [filename] |\n");
    printf("| delay = 10ms<delay<40000                                                  |\n");
    printf("+---------------------------------------------------------------------------+\n");
    return 0;
  }
  if (argc != 8) {
    printf("Bitte folgende Werte angeben: \n");
    printf("./encoder -delay [ms] -mult [mult] -l [y|n] [filename]\n");
    return 0;
  }
  if (strtol(argv[2],NULL,10)<10 || strtol(argv[2],NULL,10)>40000){ 
    printf("Bitte einen Wert grösser als 10ms und kleiner als 40000ms\n");
    printf("als Parameter angeben.\n");
    return 0;
  }
  if (!(strcmp("y",argv[6])== 0 || strcmp("n",argv[6])==0)){
    printf("Bitte Last mit y oder n aktivieren oder deaktivieren: \n");
    printf("./encoder -delay [ms] -mult [mult] -l [y/n]\n");
    return 0;
  }
  rtsched();
  signal (SIGINT, handler);
  pisetup();
  fileheader(argv); 
  gettimeofday(&start,0);
  if (strcmp("y", argv[6])==0)
    {
      openLoad();
      modbus_set_slave(ctx,1);
      modbus_set_response_timeout(ctx, 1, 0);
      withLoad(argv);
    }
  else
    withoutLoad(argv);
  
}

