//#include <Wire.h>

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
//#include <LiquidCrystal_I2C.h>
//#include "Wire.h"
#include "EmonLib.h" 
#include <LiquidCrystal.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x09, 0x82 };
IPAddress ip(192, 168, 0, 177); 
EthernetServer server(80);  

// Modulo I2C display no endereco 0x27
//LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 8);

File webFile;

#define REQ_BUF_SZ   40
char HTTP_req[REQ_BUF_SZ] = {
  0};
char req_index = 0;

int flag1, flag2, flag3, flag4 = 0;
const int carga1 = 2;
const int carga2 = A0;
const int carga3 = A2;
const int carga4 = A3;

int led = 9;

void setup()
{
  pinMode(carga1, OUTPUT);
  pinMode(carga2, OUTPUT);
  pinMode(carga3, OUTPUT);
  pinMode(carga4, OUTPUT);
  analogReference(INTERNAL);
  lcd.begin(16, 2);

  pinMode(led, OUTPUT); // led
  //Serial.begin(9600);       

  //Serial.println("Inicializando cartao microSD...");
  lcd.setCursor(0, 0);
  lcd.print("Carregando...   ");
  delay(1000);
  
  if (!SD.begin(4)) {
    //Serial.println("ERRO - inicializacao do cartao falhou!");
    lcd.setCursor(0, 0);
    lcd.print("ERRO no microSD ");
    return;    
  }
  //Serial.println("SUCESSO - cartao microSD inicializado.");
  lcd.setCursor(0, 0);
  lcd.print("Servidor Ativo  ");
  delay(1000);
  
  if (!SD.exists("index.htm")) {
    //Serial.println("ERRO - index.htm nao foi encontrado!");
    lcd.setCursor(0, 0);
    lcd.print("microSD Ausente  ");
    return;  
  }
  //Serial.println("SUCESSO - Encontrado arquivo index.htm.");
  lcd.setCursor(0, 0);
  lcd.print("Carregando index");
  delay(1000);

  lcd.setCursor(0, 0);
  lcd.print("Box Modular v.86");
  
  Ethernet.begin(mac, ip);  
  server.begin();

}

void loop()
{ 
  EthernetClient client = server.available();  //verifica se existe alguém querendo se conectar
  if (client) {  // existe cliente?
    
    boolean currentLineIsBlank = true;
    while (client.connected()) {    
      digitalWrite(led, HIGH);
      if (client.available()) {   // existe informacao vinda do cliente
        char c = client.read(); // le cada byte enviado pelo cliente, ou seja, cada caracter
        // por padrao, o ultimo caracter enviado pelo cliente (nosso navegador) é em branco  e termina com \n
        // dessa forma conseguimos saber se o cliente acabou de enviar informacoes para o servidor (Arduino)
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;          // salva os caracteres das solicitacoes do browser
          req_index++;
        }
        if (c == '\n' && currentLineIsBlank) {
          // envia para o cliente o protocolo padrao de sucesso HTTP
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          
          if (StrContains(HTTP_req,"ajax_carga1")) {
            SetCarga1();
          }
          else if (StrContains(HTTP_req,"ajax_carga2")) {
            SetCarga2();
          }
          else if (StrContains(HTTP_req,"ajax_carga3")) {
            SetCarga3();
          }
          else if (StrContains(HTTP_req,"ajax_carga4")) {
            SetCarga4();
          }
          else{
            // grava no arquivo webFile a página que temos no microSD
            webFile = SD.open("index.htm");        
            if (webFile) {
              while(webFile.available()) {
                client.write(webFile.read()); // envia para o cliente a página - nessa linha de fato o Arduino imprime no browser a página
              }
              webFile.close();
              digitalWrite(led, LOW);
            }
          }
          Serial.println(HTTP_req); //para debug, verifica no monitor serial a request
          //digitalWrite(ledFrontal, HIGH);
          req_index = 0; //reseta o index do buffer e a variável que armazena as requests
          StrClear(HTTP_req, REQ_BUF_SZ);
          
          break;
        }
        
        // toda linha de texto recebida do cliente termina com \r\n
        if (c == '\n') {
          //verifica se acabou a linha, já que \n é o ultimo caracter
          currentLineIsBlank = true;
          
        } 
        else if (c != '\r') {
          // o cliente ainda está enviando informações
          currentLineIsBlank = false;
            
        }
      } 
    } 
    delay(1);      // dá um tempo para o browser receber os dado
    client.stop(); // fecha a conexão
    
  }

}
//******************************************************************
void SetCarga1(){
  
  if(flag1 == 0){
    lcd.setCursor(0, 1);
    lcd.print("Sala          ON");
    digitalWrite(carga1, HIGH);
    flag1 = 1; 
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("Sala         OFF");
    digitalWrite(carga1, LOW);
    flag1 = 0;  
  }
}

void SetCarga2(){
  
  if(flag2 == 0){
   lcd.setCursor(0, 1);
   lcd.print("Quarto        ON");
   digitalWrite(carga2, HIGH);
   flag2 = 1; 
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("Quarto       OFF");
    digitalWrite(carga2, LOW);
    flag2 = 0;  
  }
  
}

void SetCarga3(){
  
  if(flag3 == 0){
   lcd.setCursor(0, 1);
   lcd.print("Cozinha       ON");
   digitalWrite(carga3, HIGH);
   flag3 = 1; 
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("Cozinha      OFF");
    digitalWrite(carga3, LOW);
    flag3 = 0;  
  }
  
}

void SetCarga4(){
  
  if(flag4 == 0){
   lcd.setCursor(0, 1);
   lcd.print("Climatizado   ON");
   digitalWrite(carga4, HIGH);
   flag4 = 1; 
  }
  else{
   lcd.setCursor(0, 1);
   lcd.print("Climatizado  OFF");
    digitalWrite(carga4, LOW);
    flag4 = 0;  
  }
  
}

// funcao para limpar arrays (no nosso caso, as variaveis que armazenam requests)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}

// funcao que procura pela string SFIND em STR
// retorna 1 se a string for encontrada
// retorna 0 se a setring não for encontrada
char StrContains(char *str, char *sfind)
{
  
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }
  return 0;
}

