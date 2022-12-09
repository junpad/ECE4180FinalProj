/*
    Jared Walker
    Ridwan Sadiq
    Emanuel Abdul-Salaam
    Juan Padilla
*/

#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#include <ctime>
#include "mbed.h"
#include "uLCD_4DGL.h"
#include "PinDetect.h"
#include "MMA8452.h"
#include "Speaker.h"


BusOut myled(LED1,LED2,LED3,LED4);
Serial blue(p9,p10);
Serial pc(USBTX, USBRX);
uLCD_4DGL ulcd(p13,p14,p11);
Speaker mySpeaker(p21);

MMA8452 acc(p28, p27, 40000); //instantiate an acc object
PinDetect pb2(p24,PullUp);//check later if thats ok
PinDetect pb1(p25,PullUp);//check later if thats ok

int screenBreak = 0;
int but_pushed1 = 0; // ==1 -> pb1 pushed, == 2 -> pb2 pushed
int but_pushed2 = 0;
bool selectedDiceArray[] = {0, 0, 0, 0, 0};
bool startGame = 0;
bool rolled = 0;
int rolledTimes = 0;
bool resetRound = 0;
int faceValCarry[5];
int randoArray[5]; 
int accFlag = 0; // this is gonna be the variable that the accelerometer flips to roll the dice


int selectedDice = 0;
int selectedDiceTotal = 0;

void selectDice()
{
    char bnum=0;
    int diceNum = 0;
    
    while(1){
        bnum = blue.getc();

        if ((bnum>='1')&&(bnum<='5')){
            break;
        }
    }
  
    selectedDice = diceNum = bnum - 48;
    if (!selectedDiceArray[diceNum-1]){ 
        selectedDiceArray[diceNum-1]= 1;
    } else{
        selectedDiceArray[diceNum-1] = 0;
    }
    
    myled = bnum - '0';
    wait(0.1);
    myled = '0';
    
}

void draw_start(){
    ulcd.text_width(2); 
    ulcd.text_height(2);
    ulcd.locate(2,2);
    ulcd.printf("\n YAHTZEE");
    
    ulcd.text_width(1); 
    ulcd.text_height(1);
    ulcd.locate(4,8);
    ulcd.printf("\n   Press to Roll");
}

void draw_readyRoll(){
    ulcd.cls();
    ulcd.text_width(1); 
    ulcd.text_height(1);
    ulcd.locate(4,1);
    ulcd.printf("\n   Ready to Roll");
    
    ulcd.text_width(2); 
    ulcd.text_height(2);
    ulcd.locate(2,2);
    ulcd.printf("SHAKE");
    ulcd.locate(3,4);
    ulcd.printf("TO");
    ulcd.locate(2,6);
    ulcd.printf("ROLL");
}

void draw_diceDisp(){
    ulcd.text_width(1); 
    ulcd.text_height(1);
    ulcd.locate(4,1);
    ulcd.printf("SELECT DICE");
    ulcd.text_width(1); 
    ulcd.text_height(1);
    ulcd.locate(2,15);
    ulcd.printf("PB2 to continue");
}

void draw_diceSelect(){
    ulcd.text_width(1); 
    ulcd.text_height(1);
    ulcd.locate(1,1);
    ulcd.printf("Press button 1 to roll again");
    ulcd.printf("\n Press button 2 to break");
}


void draw_scoreScreen(){
    ulcd.text_width(1); 
    ulcd.text_height(1);
    ulcd.locate(1,1);
    ulcd.printf("Score your Round");

    ulcd.filled_rectangle(1,115,120,150,BLACK);
    ulcd.locate(6,15);
    ulcd.printf("RESET");
}

int roll(){
    int rando = (rand()%6)+1; 
    return rando;
}

void playJingle()
{
    float g5=783.99; 
    float f5=698.46; 
    float e5=659.25; 
    float d5=587.33; 
    float g4=392;
    mySpeaker.PlayNote(g5,.32,.1);
    mySpeaker.PlayNote(f5,.32/2,.1);
    mySpeaker.PlayNote(e5,.32/2,.1);
    mySpeaker.PlayNote(d5,.32/2,.1);
    mySpeaker.PlayNote(g4,.32,.1);

}

void playEndJingle()
{
    float g5=783.99; 
    float f5=698.46; 
    float e5=659.25; 
    float d5=587.33; 
    float g4=392;
    mySpeaker.PlayNote(g4,.32,.1);
    mySpeaker.PlayNote(d5,.32/2,.1);
    mySpeaker.PlayNote(e5,.32/2,.1);    
    mySpeaker.PlayNote(f5,.32/2,.1);
    mySpeaker.PlayNote(f5,.32/2,.1);        
    mySpeaker.PlayNote(g5,.32,.1);
}

void playRollJingle()
{
    float g5=783.99; 
    float g4=392;
    mySpeaker.PlayNote(g4,.32,.1);      
    mySpeaker.PlayNote(g5,.32,.1);
    mySpeaker.PlayNote(g4,.32,.1);
    mySpeaker.PlayNote(g5,.32,.1);
    mySpeaker.PlayNote(g4,.32,.1);
    mySpeaker.PlayNote(g5,.32,.1);
    mySpeaker.PlayNote(g4,.32,.1); 
}

// This fuctuion prints a single dice to the screen, but can be used over and over to print every dice
int xarry[] = {22,64,105,22,64};//holds the center point for every dice on the X axis (for ulcd)
int yarry[] = {40,40,40, 90,90};//holds the center point for every dice on the Y axis (for ulcd)
void diceLoc(int diceLoc,int diceFaceNum,int color)//diceLoc is the location of the dice (1-5) and diceFaceNum is the number on the dice face (provided by a random number gen
{ 
            if (rolledTimes < 3){
                draw_diceDisp();
            } else {
                draw_scoreScreen();
            }
            int x1=xarry[diceLoc] + 20;
            int x2=xarry[diceLoc] - 20;
            int y1=yarry[diceLoc] + 20;
            int y2=yarry[diceLoc] - 20;
            if(color == 1){
                ulcd.rectangle(x1,y1,x2,y2,WHITE);
        switch(diceFaceNum){
            case 1://if dice =1
                ulcd.filled_circle(xarry[diceLoc] , yarry[diceLoc] ,3, WHITE);
            break;
            
            case 2://if dice =2
         
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10), 3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+10) ,3, WHITE);

            break;
            
            case 3://so on
                ulcd.filled_circle((xarry[diceLoc]) , (yarry[diceLoc]) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc] - 10) ,3, WHITE);

            break;  
                      
            case 4://so forth 
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]- 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]+10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+ 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10) ,3, WHITE);

            break; 
                       
            case 5:
              
                ulcd.filled_circle((xarry[diceLoc]) , (yarry[diceLoc]) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]+ 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]- 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10) ,3, WHITE);
            break;                
            case 6:
       
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]- 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]+10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+ 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]) ,3, WHITE);
                ulcd.filled_circle((xarry[diceLoc]- 10 ) , (yarry[diceLoc]) ,3, WHITE);
                break;  
            
            default:
                break;
            }
            }
            else if(color == 2){
                ulcd.rectangle(x1,y1,x2,y2,BLUE);
        switch(diceFaceNum){
            case 1://if dice =1
                ulcd.filled_circle(xarry[diceLoc] , yarry[diceLoc] ,3, BLUE);
            break;
            
            case 2://if dice =2
         
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10), 3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+10) ,3, BLUE);

            break;
            
            case 3://so on
                ulcd.filled_circle((xarry[diceLoc]) , (yarry[diceLoc]) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc] - 10) ,3, BLUE);

            break;  
                      
            case 4://so forth 
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]- 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]+10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+ 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10) ,3, BLUE);

            break; 
                       
            case 5:
              
                ulcd.filled_circle((xarry[diceLoc]) , (yarry[diceLoc]) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]+ 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]- 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10) ,3, BLUE);
            break;                
            case 6:
       
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]- 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]+10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]+ 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10) , (yarry[diceLoc]- 10) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]+ 10) , (yarry[diceLoc]) ,3, BLUE);
                ulcd.filled_circle((xarry[diceLoc]- 10 ) , (yarry[diceLoc]) ,3, BLUE);
                break;  
            
            default:
                break;
            }
            }

}

void shakeDetect()
{   
        acc.setBitDepth(MMA8452::BIT_DEPTH_12);
        acc.setDynamicRange(MMA8452::DYNAMIC_RANGE_4G);
        acc.setDataRate(MMA8452::RATE_100);// dont worry about this garbage. it just sets some internal values for the acc                  
        double x = 0;
        double y = 0;
        
        while(1)
        {      
            acc.readXGravity(&x); //notice this is passed by reference use pointers
            acc.readYGravity(&y);
            if(x >= 2.0 || y >= 2.0)
            {
                accFlag = 1;
                break;
            }        
        }
        
}

void start_game_btn (void) {
    if (!startGame){
        startGame = 1;
    }
}

//void selected_dice_bool_func (void) {
//    if (rolled){
//        selected_dice_bool = 1;
//    }
//}

int main()
{
    wait(0.001);
    ulcd.cls();
    ulcd.baudrate(3000000); //jack up baud rate to max for fast display

    pb1.attach_deasserted(&start_game_btn);
    pb1.setSampleFrequency();
//    pb2.attach_deasserted(&selected_dice_bool_func);
//    pb2.setSampleFrequency();

    while(!startGame){
        draw_start();
        playJingle();
    }
//    while(!resetRound){
    for (int j=0; j<3;j++)
    { //tbis is where the Game happens, breaks after j = 3 for three rounds   
        draw_readyRoll();
        while (accFlag != 1)
        {
            shakeDetect();
        }

        accFlag = 0;
        rolled = 1;
                    
        if (rolled){
            ulcd.cls();
            playRollJingle();
            
            for(int i = 0; i<5 ; i++)
            {  
                if(selectedDiceArray[i] == false)
                {
                    randoArray[i] = roll();
                    
                    //this wont reprint the values of the duce if the dice has been selected on the bluetooth   
                    diceLoc(i,randoArray[i],1);// print function for dice. i is the location of the dice and rando is a random number for the dice face
                } else
                {
                    diceLoc(i,randoArray[i],2);
                    //faceValCarry[i] = randoArray[i];// I think this works, what I am tring to do is retain the random value I generated earlier
                }
            }
            
            while(1){
                if (!pb2){
                    selectDice();
                    if (selectedDiceArray[selectedDice-1]){
                        diceLoc(selectedDice-1,randoArray[selectedDice-1],2);
                    } else{
                        diceLoc(selectedDice-1,randoArray[selectedDice-1],1);
                    }
                }
                if (!pb1) {
                    break;
                }
                
//                pc.printf("Selected Dice: '%d  %d %d'\n", selectedDice, selectedDiceArray[selectedDice-1], selectedDiceTotal);
            }
            wait(1);
            rolled = 0;
        }  
        
        rolledTimes += 1;
//        if (rolledTimes == 3) resetRound = 1;
//    }
    }
    
    for(int i = 0; i<5 ; i++){   
        diceLoc(i,randoArray[i],2);
    }
    playEndJingle();
    
//    if(!pb1) resetRound = 0;
     
}
