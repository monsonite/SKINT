// SKINT  - Hybrid of MINT and STABLE in C++ for Arduino IDE compatible targets 
// Ken Boak October 10th 2023
// Stack manipulation model adapted from STABLE, function names as per MINT
// Limited support for calling MINT Extended Functions
// MINT arrays removed 

// Set PC to 0, SP to 100 and RSP to 60
 int p=0 ;         // The pointer to the character in the program buffer
 int s=100 ;       // Stack Pointer
 int r=60 ;        // The return stack pointer
 int i ;           // A working register used for evaluating numbers and some stack manipulations
 int v ;           // Variable number
 int f ;           // The function number calculated as ascii value - 35 so A is function 30, Z is function 56
 int st[256] ;     // Stack
 int d=13 ;
 unsigned int x=0 ;         // Working register to evaluate number strings 
 char t=0 ;
 char c ;         // The character in the file record to be written into pr memory
 char u ;         // The character at the current program location in pr memory
 char k=0 ;       // A flag that denotes that the character is a variable
 char pr[1000] ;  // The memory containing the program
 char buf[64];  
 char* addr;

// ************************************************************************************************
// Numerical and alpha character handling
// ************************************************************************************************  

void N(){;}                                                                            //  No Operation  ascii 0 - 32 
void f9(){;}                                                                           //  Decimal digits 0 to 9
void fHEX(){}                                                                          //  Hex operator #
void fA(){r++; st[r]=p; p=st[u-35]; u=pr[p]; p--;}                                     //  Uppercase used for functions A-Z
void fa(){k=1; v=u-97; }                                                               //  Lowercase used for variables a-z
 
// ************************************************************************************************
// Stack group
// ************************************************************************************************

void fDUP(){s++; st[s]=st[s-1];}                                                         //  "   DUP
void fSWAP(){i=st[s]; st[s]=st[s-1]; st[s-1]=i;}                                         //  $   SWAP
void fOVER(){i=st[s];st[s]=st[s-2];st[s-2]=st[s-1];st[s-1]=i;}                           //  %   OVER 
void fDROP(){s--;}                                                                       //  '   DROP

// ************************************************************************************************
// Logical group
// ************************************************************************************************

void fAND(){st[s-1]&=st[s]; s--; }                                                      //  &  AND
void fOR(){st[s-1]|=st[s]; s--; }                                                       //  |  OR
void fXOR(){st[s-1]^=st[s]; s--; }                                                      //  ^  XOR
void fNOT(){st[s]=  ~st[s]; }                                                           //  ~  NOT
void fNEG(){st[s]=-st[s]; }                                                             //  _  NEG
void fSHL(){st[s]= st[s]<<1 ; }                                                         //  {  SHL
void fSHR(){st[s]= st[s]>>1 ; }                                                         //  }  SHR

// ************************************************************************************************
// Arithmetic group
// ************************************************************************************************

void fMUL(){st[s-1]*=st[s]; s--; }                                                      //  *  MUL
//void fADD(){ if(k==0){st[s-1]+=st[s]; s--; }else{st[v]++;} }                          //  +  ADD  or INC register 
//void fSUB(){ if(k==0){st[s-1]-=st[s]; s--; }else{st[v]--;} }                          //  -  SUB  or DEC register
void fADD(){ st[s-1]+=st[s]; s--; }
void fSUB(){ st[s-1]-=st[s]; s--; }
void fDIV(){st[s-1]/=st[s]; s--; }                                                      //  /  DIV
//f37(){st[s-1]%=st[s]; s--; }                                                          //  %   MOD

// ************************************************************************************************
// Printing and I/O group
// ************************************************************************************************

void fEMIT(){Serial.write(st[s]); s--; }                                                 //  ,  Send ascii code to terminal
void fDOT(){Serial.println(st[s]); s--;}                                                 //  .  DOT  Print value from top of stack                                                 
void fTICK(){p++;u=pr[p];while(u!='`'){Serial.print(u);p++;u=pr[p];}}                    //  `  Print String between ticks
void fQUERY(){if((c=getc(stdin))==EOF) { c=0; } s++; st[s]=c; }                          //  ?  KEY - read key from terminal

// ************************************************************************************************
// Register group
// ************************************************************************************************

void fSTORE(){st[v]=st[s]; s--; }                                                         //  !  Store value in register                                                        
void fFETCH(){s++; st[s]=st[v]; }                                                         //  @  Fetch value from register
//f63(){s++; st[s]=st[st[v]]; }                                                     //  ?  Fetch memory from address in register
//f33(){st[st[v]]=st[s]; s--; }                                                     //  !   Store value to address in register

// ************************************************************************************************
// Conditional and Looping group
// ************************************************************************************************

void fLT(){if(st[s]> st[s-1]){st[s]=0;}else{st[s]=1;}}                                 //  <  LT
void fEQ(){if(st[s]==st[s-1]){st[s]=0;}else {st[s]=1;}}                                //  =  EQ
void fGT(){if(st[s]< st[s-1]){st[s]=0;}else{st[s]=1;}}                                 //  >  GT
void fBEGIN(){if(st[s]==0){s--;p++;u=pr[p];while(u!=')'){p++;u=pr[p];}} else{s--;}}    //  (  Execute code between brackets if f=TRUE else skip
void fOPEN(){r++;st[r]=p;if(st[s]==0){p++;u=pr[p]; while(u!=']'){ p++; u=pr[p];}}}     //  [  BEGIN WHILE
void fCLOSE(){if(st[s]!=0) p=st[r]; else r--; s--; }                                   //  ]  END WHILE   

// ************************************************************************************************
// Colon Definition group
// ************************************************************************************************

void fCOL(){f=pr[p+1]-35;st[f]=p+2;while(u!=';'){p++;u=pr[p];}}                        //  :  Start Colon Definition
void fSEM(){p=st[r];r--;}                                                              //  ;  End Colon Definition

// ************************************************************************************************
// Misc group
// ************************************************************************************************

void fEXT(){p++;u=pr[p];while(u!='\''){s++;st[s]=u;p++;u=pr[p];}}                       //  \   Call an extended function n

void (*q[127])()={
N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,
fSTORE,fDUP,fHEX,fSWAP,fOVER,fAND,fDROP,fBEGIN,N,fMUL,fADD,fEMIT,fSUB,fDOT,fDIV,
f9,f9,f9,f9,f9,f9,f9,f9,f9,f9,
fCOL,fSEM,fLT,fEQ,fGT,fQUERY,fFETCH,
fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,fA,
fOPEN,fEXT,fCLOSE,fXOR,fNEG,fTICK,
fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,fa,
fSHL,fOR,fSHR,fNOT};

void setup() {  
 Serial.begin(115200);
 pinMode(d,OUTPUT);
 Serial.println("STINT V1.0 10/10/23");
}

void loop()
{ 
  txtRead(pr, 64);     // Read input string  
  txtEval(pr);         // Evaluate instructions         
}
 
void txtRead (char *p, byte n) {
  byte i = 0;
  while (i < (n-1)) {
    while (!Serial.available());
    char ch = Serial.read();
    if (ch == '\r' || ch == '\n') break;
    if (ch >= ' ' && ch <= '~') {
      *p++ = ch;
      i++;
    }
  }
  *p = 0;                        // Put a null at the end of the input string
}

// ------------------------------------------------------------------------------------------------ 
  void txtEval (char *pr) {
  unsigned int k = 0;
  char ch;
  while ((ch = *pr++)) {
    u= ch ;
    if (u>='0'&& u<='9');{
       switch (ch) {
//---------------------------------------------------------------------     
// Detect numerical characters and form a 16-bit integer to place on top of stack       
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    x = ch - '0'; while (*pr >= '0' && *pr <= '9') { x = x*10 + (*pr++ - '0'); } s++; st[s] =x;  break ;
       }
    }
   
    q[u](); 
    if(u<'a')k=0; else if(u>'z')k=0;            // Fetch character, call function from table, set k flag
   }                             
}

