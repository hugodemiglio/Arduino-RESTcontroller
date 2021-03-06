/* File generated by Hugo Arduino Organizator (php) */
/* Generated at 28/12/2011 02:46:17 - hgbrasil.com */
/* Developer Hugo Leonardo Demiglio - hugodemiglio@gmail.com (RESTcontroller.ino 0.0.1) */

#define DEBUG 1

#include "EtherShield.h"
#include <Stepper.h>
#include <LiquidCrystal.h>
/* Keyboard */
int key = 0;
int lastKey = 0;
int keyboardPin = A0;
int drawerStatus = 0;

/* Leds */
const int led1 = 6, led2 = 5, led3 = 4, downLed = 7, lcdLed = 3;
const int ledPower = 50, downLedPower = 100, lcdLedPower = 100;
int ledStatus = 0, downLedStatus = 0;

/* Motor */
const int stepsPerRevolution = 200;
int steps = 1000;
Stepper myStepper(stepsPerRevolution, 8,9,10,11);

/* Sensor of light */
const int lightSensor = A1;
int light = 0;

/* System */
int randomic = 0;
int serialRead = 0;

  LiquidCrystal lcd(48, 8, 9, 49, 10, 46);
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
void loop_keyboard(){
  key = keyboard(keyboardPin);
  /*if (key != lastKey) {
    if (key == 1) {
      digitar(1);
    }
  }
  
  if (key != lastKey) {
    if (key == 2) {
      digitar(2);
    }
  }*/
  
  if (key != lastKey) {
    if (key == 5 && drawerStatus != 2) {
      close();
    }
  }
  
  if (key != lastKey) {
    if (key == 6 && drawerStatus != 1) {
      open();
    }
  }
  
  lastKey = key;
}

int keyboard(int keyboardPin){
 int value;
 value = analogRead(keyboardPin);
 if(value > 900 && value < 999){
   return 1;
 } else if(value > 870 && value < 899){
   return 2;
 } else if(value > 400 && value < 499){
   return 3;
 } else if(value > 800 && value < 860){
   return 4;
 } else if(value > 700 && value < 799){
   return 5;
 } else if(value > 600 && value < 699){
   return 6;
 } else {
   return 0;
 }
}

void debug(char *message){
#ifdef DEBUG
  Serial.println(message);
#endif
}

void lcd_print(char *line1, char *line2){
  lcd.clear();
  lcd_leds_on();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void lcd_clear(){
  lcd.clear();
  lcd_leds_off();
}

void get_light(int light, int drawerStatus){
 if(light < 200 && drawerStatus != 2){
   if(ledStatus == 0){
#ifdef DEBUG
     Serial.println("Sensor: Ambiente escuro detectado.");
#endif
     down_leds_on();
     drawer_leds_on();
   }
 } else {
   if(ledStatus == 1){
#ifdef DEBUG
     Serial.println("Sensor: Ambiente claro detectado.");
#endif
     down_leds_off();
     drawer_leds_off();
   }
 }
 delay(100);
}
static uint8_t mymac[6] = { 0x54,0x55,0x58,0x10,0x20,0x35}; 
static uint8_t myip[4] = { 192,168,0,115};

#define MYWWWPORT 80   

#define BUFFER_SIZE 800
static uint8_t buf[BUFFER_SIZE+1];
#define STR_BUFFER_SIZE 22
static char strbuf[STR_BUFFER_SIZE+1];

EtherShield es=EtherShield();
uint16_t http200ok(void){
  return(es.ES_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n")));
}

uint16_t http404(void){
  return(es.ES_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 404 OK\r\nContent-Type: text/html\r\n\r\n")));
}

uint16_t print_webpage(uint8_t *buf){
  uint16_t plen;
  plen=http200ok();
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("{\"drawer_status\":\""));
  //plen=es.ES_fill_tcp_data_p(buf,plen,PSTR(drawerStatus));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("\",\"ambient_light\":\"false\"}"));
  return(plen);
}

uint16_t print_404(uint8_t *buf){
  uint16_t plen;
  plen=http404();
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<h1>Not found</h1>"));
  return(plen);
}

void send_home(uint16_t dat_p){
  dat_p=print_webpage(buf);
  es.ES_www_server_reply(buf,dat_p);
}

void send_404(uint16_t dat_p){
  dat_p=print_404(buf);
  es.ES_www_server_reply(buf,dat_p);
}

#define CMDBUF 50

void process_request(char *str, uint16_t dat_p){
  int8_t r=-1;
  int8_t i = 0;
  char clientline[CMDBUF];
  int index = 0;
  int plen = 0;

  char ch = str[index];
  
  while( ch != ' ' && index < CMDBUF) {
    clientline[index] = ch;
    index++;
    ch = str[index];
  }
  clientline[index] = '\0';

  // convert clientline into a proper
  // string for further processing
  String urlString = String(clientline);

  // extract the operation
  String op = urlString.substring(0,urlString.indexOf(' '));

  // we're only interested in the first part...
  urlString = urlString.substring(urlString.indexOf('/'), urlString.indexOf(' ', urlString.indexOf('/')));

  // put what's left of the URL back in client line
  urlString.toCharArray(clientline, CMDBUF);

  // get the first two parameters
  char *pin = strtok(clientline,"/");
  char *value = strtok(NULL,"/");

  // this is where we actually *do something*!
  char outValue[10] = "MU";
  char jsonOut[50];

  if(pin != NULL){
    if(value != NULL){
      
      debug("Analizing data...");
      
      if(strncmp(pin, "effect", 6) == 0){
        
        debug("HTTP request effect");
        
        if(strncmp(value, "1", 1) == 0){
          debug("Processing effect 1");
          send_home(dat_p);
          drawer_leds_effect(1);
        }
        
        else if(strncmp(value, "2", 1) == 0){
          debug("Processing effect 2");
          send_home(dat_p);
          drawer_leds_effect(2);
        }
        
        else {
          send_404(dat_p);
        }
        
      }
      
    }
  } else {  
    send_404(dat_p);
  }
}
void open(){
#ifdef DEBUG
  Serial.println(" ====== Pedido recebido ====== ");
  Serial.println("Pedido para abrir a gaveta recebido.");
  Serial.println("Abrindo a gaveta...");
#endif
  drawer_leds_on();
  myStepper.step(-steps);
  stopMotor();
  drawerStatus = 1;
  drawer_leds_off();
#ifdef DEBUG
  Serial.println("Gaveta aberta com sucesso!");
  Serial.println(" ====== Pedido concluido ====== ");
#endif
}

void close(){
#ifdef DEBUG
  Serial.println(" ====== Pedido recebido ====== ");
  Serial.println("Pedido para fechar a gaveta recebido.");
  Serial.println("Fechando a gaveta...");
#endif
  randomic = random(1, 4);
  drawer_leds_on();
  myStepper.step(steps);
  stopMotor();
  drawerStatus = 2;
  drawer_leds_off();
  drawer_leds_effect(randomic);
#ifdef DEBUG
  Serial.println("Gaveta fechada com sucesso!");
  Serial.println(" ====== Pedido concluido ====== ");
#endif
}

void stopMotor(){
#ifdef DEBUG
  Serial.println("Desligando motor principal.");
#endif
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}
void drawer_leds_on(){
#ifdef DEBUG
  Serial.println("Acendendo as luzes.");
#endif
  ledStatus = 1;
  fadeIn(0, ledPower, led1, 1);
  fadeIn(0, ledPower, led2, 1);
  fadeIn(0, ledPower, led3, 1);
#ifdef DEBUG
  Serial.println("Luzes acessas!");
#endif
}

void drawer_leds_off(){
#ifdef DEBUG
  Serial.println("Apagando as luzes.");
#endif
  ledStatus = 0;
  fadeOut(ledPower, 0, led1, 1);
  fadeOut(ledPower, 0, led2, 1);
  fadeOut(ledPower, 0, led3, 1);
#ifdef DEBUG
  Serial.println("Luzes apagadas!");
#endif
}

void down_leds_on(){
  downLedStatus = 1;
  fadeIn(0, downLedPower, downLed, 1);
}

void down_leds_off(){
  downLedStatus = 0;
  fadeIn(downLedPower, 0, downLed, 1);
}

void lcd_leds_on(){
  fadeIn(0, lcdLedPower, lcdLed, 1);
}

void lcd_leds_off(){
  fadeIn(lcdLedPower, 0, lcdLed, 1);
}

void drawer_leds_effect(int effect){
#ifdef DEBUG
  Serial.println("Efeito de luzes solicitado.");
  Serial.println("Efeito em execucao...");
#endif
  int i;
  int j;
  int z = 0;
  int delay_time = 1;
  switch(effect){
    case 1:
      for(i = 0;i < 5;i++){
        drawer_leds_on();
        drawer_leds_off();
      }
      break;
    case 2:
      for(i = 0;i < 5;i++){
        fadeIn(0, 100, led1, delay_time);
        for(j = 100;j >= 0;j--){
          analogWrite(led1, j);
          if(j <= 50){
            analogWrite(led2, z);
            z++;
          }
          delay(delay_time);
        }
        fadeIn(50, 100, led2, delay_time);
        z = 0;
        for(j = 100;j >= 0;j--){
          analogWrite(led2, j);
          if(j <= 50){
            analogWrite(led3, z);
            z++;
          }
          delay(delay_time);
        }
        fadeIn(50, 100, led3, delay_time);
        fadeOut(100, 0, led3, delay_time);
      }
      break;
    case 3:
      for(i = 0;i < 50;i++){
        analogWrite(led1, 100);
        delay(delay_time * 20);
        analogWrite(led1, 0);
        analogWrite(led2, 100);
        delay(delay_time * 20);
        analogWrite(led2, 0);
        analogWrite(led3, 100);
        delay(delay_time * 20);
        analogWrite(led3, 0);
      }
      break;
    case 4:
      for(j = 0; j < 1;j++){
        for(int i = 1;i <= 100;i++){
          analogWrite(led1, i);
          analogWrite(led2, i);
          analogWrite(led3, i);
          delay(15);
        }
        delay(100);
        for(int i = 100;i >= 1;i--){
          analogWrite(led1, i);
          analogWrite(led2, i);
          analogWrite(led3, i);
          delay(15);
        }
        delay(100);
      }
      break;
  }
#ifdef DEBUG
  Serial.println("Efeito concluido com sucesso!");
#endif
}

void fadeOut(int start, int end, int port, int delay_time){
 for(int i = start;i > 0;i--){
  analogWrite(port, i);
  delay(delay_time);
 }
 analogWrite(port, end);
}

void fadeIn(int start, int end, int port, int delay_time){
 for(int i = start;i < end;i++){
  analogWrite(port, i);
  delay(delay_time);
 }
 analogWrite(port, end);
}
void setup(){
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);
  
  down_leds_on();
  drawer_leds_on();
  lcd_leds_on();
  lcd.begin(16, 2);
  
  lcd_print("HG HTTP Control", "Reading... 0.0.1");
  
  /*lcd.print("HG HTTP Control");
  lcd.setCursor(0, 1);
  lcd.print("Reading... 0.0.1");*/
  
  // Init SPI
   es.ES_enc28j60SpiInit();
  
  // initialize enc28j60
  es.ES_enc28j60Init(mymac);  //, 8);

  //init the ethernet/ip layer:
  es.ES_init_ip_arp_udp_tcp(mymac,myip,80);
  
  Serial.begin(9600);
  
#ifdef DEBUG
  Serial.println("ENC28J60 RESTduino");

  Serial.print( "ENC28J60 version " );
  Serial.println( es.ES_enc28j60Revision(), HEX);
  if( es.ES_enc28j60Revision() <= 0 ) 
    Serial.println( "Failed to access ENC28J60");
#endif

#ifdef DEBUG
  Serial.println("Ready");
#endif
  
  lcd_print("HG HTTP Control", "Ready.");
  
  /*cd.setCursor(0, 1);
  lcd.print("Ready.");*/
  delay(1000);
  down_leds_off();
  drawer_leds_off();
  lcd_clear();

}
void loop(){
  loop_ethernet();
  loop_keyboard();
  get_light(analogRead(lightSensor), drawerStatus);
}
bool loop_ethernet(){
  uint16_t plen, dat_p;
  int8_t cmd;
  
  // read packet, handle ping and wait for a tcp packet:
  dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));

  /* dat_p will be unequal to zero if there is a valid http get */
  if(dat_p==0){
    //last_command_received = 0;
    return false;
  } else {
    // tcp port 80 begin
    if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
      // head, post and other methods:
      dat_p = print_webpage(buf);
      es.ES_www_server_reply(buf,dat_p);
    }

    // just one web page in the "root directory" of the web server
    else if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0){
      dat_p=print_webpage(buf);
      es.ES_www_server_reply(buf,dat_p);
      lcd_print("GET /", (char *)&(buf[dat_p+4]));
    }
    
    // process REST request
    else {
      process_request((char *)&(buf[dat_p+4]), dat_p);
    }
    
    // tcp port 80 end
    return true;
  }
}
