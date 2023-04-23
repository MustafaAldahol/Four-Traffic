/*
 * this program made to control Four traffic without delay function
 * it's work fine in real world, but doesn't in simulation due to displaying method we use. 
 */

#define shift_clock 13
#define data        12
#define shift_store 11
#define enable      10 // activated in LOW case

class Traffic {
  private :
  const int GREEN_TIME = 1500 , RED_TIME = 4500 ,ORANGE_TIME = 700; //cahnge these values to control light time
    bool green = false , orange = false , red = false;
    byte period = 1; // takes only two value 1 and 2, and it's refere to green or red period
    
  public :
    unsigned long green_timer , red_timer;
    char set_light(){
      char state;
      if( (millis() - green_timer <= GREEN_TIME) && period == 1){
        green = true;
        state ='G';
      }
      else if( (millis() - green_timer > GREEN_TIME) && period == 1){
        green = false;
        period = 2;
        red_timer = millis(); 
      }
      
      else if( (millis() - red_timer <= RED_TIME) && period == 2){
          red = true;
          state ='R';
          if(millis() - red_timer >= (RED_TIME - ORANGE_TIME) ){ 
            orange = true;
            state ='O';
          }
      }
      else if( (millis() - red_timer > RED_TIME) && period == 2){
        red = false;//Serial.println("    OK  ");
        orange = false;
        period = 1;
        green_timer = millis();
      }
  return (state);
}

  
};


Traffic traf1 ,traf2 ,traf3 ,traf4;
char    light1,light2,light3,light4;
byte Data = 0b00000000;
unsigned long traffic_time = 0 , dely = 1500 ,control_Time = 0;
bool program_started = 1; // use once throw program
byte T = 0;              // use once throw program
void setup() {
  pinMode(data ,OUTPUT);
  pinMode(shift_clock ,OUTPUT);
  pinMode(shift_store ,OUTPUT);
  pinMode(enable ,OUTPUT);
  Serial.begin(9600);
  traffic_time = millis();
}

void loop() {
  
  traffics_control();
  set_data(light1 ,light2 ,light3 ,light4);
  send_byte(Data);

}

//-----control 4 Traffics-----//
void traffics_control(){
/*
 * starts traffic respectivly, and we need that due to delay 
 * between each two traffic, and after make delay between them,
 * keep every traffic work with its speacial delay and specification 
 * if you look at code below, you will see that we initailize traffic's time for one time 
 * and make delay between each tow initailization
 * we didn't initialize them in the Traffic class because 
 * we don't need that evry time we call the object
 */
  if(program_started == 1){
    light1 = traf1.set_light(); // turn first traffic on
    if(millis() - traffic_time > dely ){  // make delay between each two traffics
      if(T==0){
        traf2.green_timer = millis() ; traf2.red_timer = millis();//initialize traffic2's timers, do this only one time
        T = 1;
      }
      light2 = traf2.set_light(); // turn second traffic on
    }
    if(millis() - traffic_time > 2*dely){ // make delay between each two traffics
      if(T==1){ 
        traf3.green_timer = millis() ; traf3.red_timer = millis();//initialize traffic3's timers, do for one time
        T = 2;
      }
      light3 = traf3.set_light();
    }
    if(millis() - traffic_time > 3*dely){ // make delay between each two traffics
        if(T==2){ //initialize traffic's timers, do for one time
        traf4.green_timer = millis() ; traf4.red_timer = millis();//initialize traffic4's timers, do for one time
        T = 3;
      }
      light4 = traf4.set_light();
      program_started = 0; // initailization is done.
    }
  }
  else{ //after initialization is done, call all traffic for ever 
    light1 = traf1.set_light();
    light2 = traf2.set_light();
    light3 = traf3.set_light();
    light4 = traf4.set_light();
  }
}
//------turn light's state to Data form-----//
void set_data(char light1 ,char light2 ,char light3 ,char light4){
/*
 * due to using shift register chip, we should divide a display operation 
 * to 4 sections, evrey one take 250 micro seconds
 * that means in every time, one traffic will be turn on  
*/  
  if(micros() - control_Time < 250 ){
    switch(light1){   
      case 'G' : Data = 0b01110100;break;
      case 'O' : Data = 0b01110011;break;
      case 'R' : Data = 0b01110001;break;
    };
  }
  else if(micros() - control_Time < 500){
    switch(light2){    
      case 'G' : Data = 0b01101100;break;
      case 'O' : Data = 0b01101011;break;
      case 'R' : Data = 0b01101001;break;
    };
  }
  else if(micros() - control_Time < 750){
    switch(light3){     
      case 'G' : Data = 0b01011100;break;
      case 'O' : Data = 0b01011011;break;
      case 'R' : Data = 0b01011001;break;
    };
  }
  else if(micros() - control_Time < 1000 ){   
    switch(light4){
    case 'G' : Data = 0b00111100;break;
    case 'O' : Data = 0b00111011;break;
    case 'R' : Data = 0b00111001;break;    
    };
  }
  else{
    control_Time = micros();
  }

}

//-----sending data-----//
void send_byte(byte Data){
/*
 * you can write this func without delay, and it will work fine 
 * that's because digitalWrite func take time that enough to work with shift register
 * it's work fine with 74HC595 shift register chip, if you use anthor chip 
 * try firstly this func by uncomment delayMicroseconds(1) func
 */
  digitalWrite(enable ,LOW);
  for(int i = 7 ;i >= 0 ;i--){
    digitalWrite(shift_clock ,LOW);
//    delayMicroseconds(1);
    digitalWrite(data ,bitRead(Data ,i));
//    delayMicroseconds(1);
    digitalWrite(shift_clock ,HIGH);
//    delayMicroseconds(1);
  }
  digitalWrite(shift_store ,HIGH);
  
  digitalWrite(shift_store ,LOW);
//  digitalWrite(enable ,HIGH);
//  delayMicroseconds(1);

}



