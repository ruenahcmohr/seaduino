
#define _GNU_SOURCE

#include <stdint.h>  // utin8_t
#include <stdio.h> // asprintf, normal printf, fopen
#include <stdlib.h> // strtoul
#include <string.h>
#include "returnValues.h"
#include "ssls3.h"

/*
xxxxx initialize console at 1Mbaud with no echo by doing
xxxxx BR 3

This requires MUCH MUCH longer reply time than the dumper.
  I should have just used a state machine...

*/

#define SERIALPORT  "/dev/ttyUSB1"

#define TIMEOUT 5000


void chomp(char * string);
int value(char *s);
int StrAccumChar( char ** s, char  c) ;
int termCom(SSLS_t * this, char **string) ;

int main(int argc, char** argv) {

 SSLS_t port;
 char * s;
 int d, r;
 FILE * input;
 
 if (argc < 2) {                          // do we have input paramiters??   
      puts("Need filename to load\n");
      return 0;    
  }
  
  if ((input = fopen(argv[1], "rb")) == NULL) {  //open text file 'param 1' w/ err chk 
      printf("Unable to open %s for input.\n", argv[1]);
      return -1;
  }

 printf("Putting down cup of coffee...\n");

 SerInit (&port);
 SerOpen (&port, SERIALPORT, 38400); // yea this really should check for errors...
 
 
 // sync.
 printf("Synchronizing host...\n"); // well it sounds good anyhow
 s = strdup("?");
 termCom(&port, &s);
 free(s);

 // set address
 printf("Setting target address...\n");
 s = strdup("AP 00000080\r");  // our code goes to 0x80, this seems to have the most ram room.
 termCom(&port, &s);

 printf("Uploading artistic binary media to RAM...\n");
 while ( (d = fgetc(input)) != EOF) {
   free(s);  s = NULL; asprintf(&s, "WT %02X\r", d);
    termCom(&port, &s);

   if (s) {
     //printf(": %s", s);
     if ((r = value(s)) != d) {
       printf("\nError, mismatch! }:( %02X != %02X\n", r, d);
       break; // hopefully this doesn't happen... ever casue otherwise I'll have write more code...
     }
   } else {
       printf("\nError, data didn't ack! (:/ \n");
       break; // hopefully this doesn't happen... ever casue otherwise I'll have write more code...  
   }
 } 
 printf("\n");

 if (d == EOF) { 
   printf("SUCCESS!!!!\n"); // �\_('')_/�
 }
 
 printf("AAAAAnd resuming coffee consumption...\n");

 fclose(input);
 SerFini (&port);

  return 0;
}


void chomp(char * string) {
  int target;
  
  target = strcspn(string,"\r\n");
  string[target] = '\0';
  
}


int value(char *s){

  unsigned long a;
  uint8_t d;

   
 if (strlen(s) < 28) { printf("Arg, partial reply!?? >%s<\n", s); return -1; }

    chomp(s+8);  
     
     if (strstr(s+8, "Addr") == (s+8)) {
       a = strtoul(s+13, NULL, 16); //  printf( " a=0x%08X", a);
       d = strtoul(s+26, NULL, 16); //  printf( " d=0x%02X\n", d);
           
       if ((a & 0x0F) == 0) { printf("0x%08X\r", a); fflush(stdout);}
       
       return d;
       
     }
  return -1;
}



// These boards seem to stall for a LONG time sometimes, FSM for efficiency
int termCom(SSLS_t * this, char **string) {

  char buff[2];
  uint8_t flag = 0;
  int retval;

  if ((retval = SerWrite(this, *string, strlen(*string))) != OK) 
       return retval;
  
  free(*string);  *string = NULL;
  
  while ( read(this->fd, buff, 1) != -1) {
          
    if (StrAccumChar( string, buff[0]) == -1)     return -1;
    
    // ARG MADE ME WRITE A STATE MACHINE.
    if (buff[0] == 0x0A) {                        flag = 1;
    } else if ((buff[0] == '>') && (flag == 1)) { flag = 2;
    } else if ((buff[0] == ' ') && (flag == 2)) { return 1;
    } else                                        flag = 0;
    
    
  }
  
  return -1;

}


// those who see my hacks are doomed to repeat them

int StrAccumChar( char ** s, char  c) {

   char * tmp;
   if (*s) {
     if (asprintf(&tmp, "%s%c", *s, c) == -1) return -1;
   } else {
     if (asprintf(&tmp, "%c", c) == -1) return -1;
   }  
     
   free(*s);   *s = tmp;
   
   return 1;
   
}



































