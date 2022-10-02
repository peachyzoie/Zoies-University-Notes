// Notes by Grace Austen @Grace-Austen on github
// A combination of what was shown on zoom (Lecture 9, Sep 27) and the notes on addressing from the end of lecture 8
// All non-professor notes will be signified by double forward slashes

/* simple example for PDP-11 address mode decoding 
 *   - global variables are common in simulators, but you could 
 *     consider designing a simulator with memory as a class 
 *     and processor as a class 
 *   - note that "register" is a reserved word in C 
 */ 
 
#include <stdio.h> 
#include <assert.h> 
 
#define MEM_SIZE_IN_WORDS 32*1024 
 
/* struct to help organize source and destination operand handling */ 
typedef struct ap{ 
  int mode;   // register mode (refer to lecture 7, 9/20 notes on addressing modes)
  int reg;    // register to refer to 
  int addr ;  /* used only for modes 1-7 */ 
  int value;  // value of final memory destination
} addr_phrase_t; 
 
void get_operand( addr_phrase_t *phrase ); 
 
int mem[MEM_SIZE_IN_WORDS]; 
int reg[8] = {0}; 
int ir; 
addr_phrase_t src, dst; 

//I have no idea why these are kept track of. Maybe for general records keeping?
//They were not shown on screen but I didn't like having those errors
int memory_reads = 0;
int inst_fetches = 0;

int main(){ 
  /* initialize memory with the word index for this test */ 
  /*   => will equal half the value of the address       */ 
  for( int i = 0; i < MEM_SIZE_IN_WORDS; i++ ){ 
    mem[i] = i; 
  } 
 
  // Instruction format for PDP-11 double operand instruction:
  //  15     12  11    9  8     6   5   3    2   0    bit number
  // | _ _ _ _ |  _ _ _  | _ _ _ |  _ _ _  | _ _ _ | 
  //   op code  adr mode  src reg  adr mode dest reg

  ir = 062501;    /* add (r5)+,r1 */ //this is the octal form of the instructions
  reg[1] = 022; 
  reg[5] = 0400; 
 
  /* we are only interested in the operand handling in this test */ 
 
  src.mode = (ir >> 9) & 07;  /* three one bits in the mask */ // you could also start with 07 and shift it left 9 spaces
  src.reg = (ir >> 6) & 07;   /* decimal 7 would also work  */ 
 
  get_operand( &src ); 
 
  printf("source address is 0%o and value is 0%o\n", src.addr, src.value); 
  printf("register 5 contents are now 0%o\n", reg[5]); 
 
  dst.mode = (ir >> 3) & 07; 
  dst.reg = ir & 07; 
 
  get_operand( &dst ); 
 
  printf("destination value for use in adding is 0%o\n", dst.value); 
 
  return 0; 
} 

/* assumes reg[7] has been updated to point beyond first word of inst. */
void get_operand( addr_phrase_t *phrase ){ // mode and reg have to be filled in in order for this function to work properly
 
  assert( (phrase->mode >= 0) && (phrase->mode <= 7) ); 
  assert( (phrase->reg  >= 0) && (phrase->reg  <= 7) ); 
 
  switch( phrase->mode ){ 
 
    /* register */ 
    case 0: 
      phrase->value = reg[ phrase->reg ]; /* value is in the register */
      assert( phrase->value < 0200000 ); 
      phrase->addr = 0;  /* unused */ 
      break;

    /* register indirect */
    case 1:
      phrase->addr = reg[ phrase->reg ]; /* address is in the register */
      assert( phrase-> addr < 0200000 );
      phrase->value = mem[ phrase->addr >> 1 ]; /* adjust to word addr */
      assert( phrase->value < 0200000 ); 
      if( phrase-> reg == 7 ) {
        printf("-- note use of R7 in mode 1\n");
      }
      memory_reads++;
      break;

    /* autoincrement (post reference) */ 
    case 2: 
      phrase->addr = reg[ phrase->reg ]; 
      assert( phrase->addr < 0200000 ); 
      phrase->value = mem[ phrase->addr >> 1 ]; /* adjust to word addr */ 
      assert( phrase->value < 0200000 ); 
      reg[ phrase->reg ] = ( reg[ phrase->reg ] + 2 ) & 0177777; 
      break; 

    /* autoincrement (post reference) indirect */
    case 3:
      phrase->addr = reg[ phrase->reg ]; /* addr of addr is in reg */
      assert( phrase-> addr < 0200000 );
      phrase->addr = mem[ phrase->addr >> 1 ]; //getting address since it was stored in memory
      assert( phrase-> addr < 0200000 );
      phrase->value = mem[ phrase->addr >> 1 ]; /* adjust to word addr */
      assert( phrase->value < 0200000 );
      reg[ phrase->reg ] = ( reg[ phrase-> reg ] + 2 ) & 0177777;
      if( phrase->reg == 7 ){
        inst_fetches++;
        memory_reads++;
      }else{
        memory_reads += 2;
      }
      break;

    /* autodecrement */
    case 4:
      reg[ phrase->reg ] = ( reg[ phrase-> reg ] + 2 ) & 0177777;
      phrase->addr = reg[ phrase->reg ]; /* addr is in the register */
      assert( phrase->addr < 0200000 );
      phrase->value = mem[ phrase->addr >> 1]; /* adjust to word addr */
      assert( phrase->value < 0200000 );
      break;

    /* autodecrement indirect */
    case 5:
      reg[ phrase->reg ] = ( reg[ phrase-> reg ] + 2 ) & 0177777;
      phrase->addr = reg[ phrase->reg ]; /* addr of addr in the register */
      assert( phrase->addr < 0200000 );
      phrase->addr = mem[ phrase->addr ]; //getting address since it was stored in memory
      assert( phrase->addr < 0200000 );
      phrase->value = mem[ phrase->addr >> 1]; /* adjust to word addr */
      assert( phrase->value < 0200000 );
      if( phrase->reg == 7 ){
/*  ---- PAST THIS POINT I AM GUESSING BECAUSE IT DIDN'T SHOW UP ON SCREEN ----  */
        inst_fetches++;
        memory_reads++;
      }else{
        memory_reads += 2;
      }
    default: 
      printf("unimplemented address mode %o\n", phrase->mode); 
  } 
}