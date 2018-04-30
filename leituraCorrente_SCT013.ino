/*
  Associação Paraibana de Ensino Renovado - ASPER 
            Trabalho de Conclusão de Curso

  Curso: Ciência da Computação
  Aluno: Ramon Quirino Bezerril
  Título: SUSTENTABILIDADE MODULAR: Automatização em ambiente
  com baixo custo dando ênfase na economia de recursos naturais.

  Orientador: Professor Msc. Messias R. Batista 
  Data: 13/04/2017
  Versão: 0.8.6
  Todo o conteúdo deste trabalho se encontra disponível
  gratuitamente em:
  https://github.com/Bezerril/SustentabilidadeModular.git
  
*/

//Carrega as bibliotecas
#include "EmonLib.h" 
#include <LiquidCrystal.h> 
#include <DS1307.h> //Carrega a biblioteca do RTC DS1307
#include <EEPROM.h>

DS1307 rtc(A4, A5); //Modulo RTC DS1307 ligado as portas A4 e A5 do Arduino 
Time t, d;
boolean estado = true;
String dia; 

EnergyMonitor emon1;
LiquidCrystal lcd(7, 6, 5, 4, 3, 8);  //Display LCD
int rede = 220.0;  //Tensao da rede eletrica
int pino_sct = A1;  //Pino do sensor SCT
float kWh, kWh_gerado;
const float valor_publica = 6.02;
const float bandeira_verm = 2.67;
const float tarifa = 0.736050;
int parado = 0;
int minuto_desligado = 0;
int minuto_ligado = 0;
int minuto_result = 0;
int minuto_atual = 0;
int desligado = 0;
int parado1, parado2 = 0;
int parado_anterior = 0;
int minuto_anterior = 0;
int funcionando = 0;
int atual = 0;
int ligado = 0;
byte cont = 0;
int led = 9;

void setup() 
{
  pinMode(led, OUTPUT); // led
  //EEPROM.begin(4);//Inicia a EEPROM com tamanho de 4 Bytes (minimo).
  rtc.halt(false);  //Aciona o relogio
  lcd.begin(16, 2);
  Serial.begin(9600);  
  emon1.current(pino_sct, 23.6);  //Pino, calibracao - Cur Const= Ratio/BurdenR. 1800/62 = 29. 
  //Definicoes do pino SQW/Out
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);
  rtc.setTime(12, 1, 0);  // Configura a hora - formato 24hr 17:00:00, após configurado, comentar
  //rtc.setDate(20, 02, 2018);
  lcd.setCursor(3, 1);
  lcd.print("Carregando...");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Box Modular v.86");
  lcd.setCursor(0, 1);
  lcd.print("  SCT013 ativo  ");
  delay(4000);
  lcd.setCursor(0, 0);
  lcd.print("Monitoramento em");
  lcd.setCursor(0, 1);
  lcd.print("   Tempo Real   ");
  delay(2000);
  lcd.clear();
} 
 
void loop()
{
  double Irms = emon1.calcIrms(1480);  // calcula
  t = rtc.getTime();
  d = rtc.getTime();
  lcd.setCursor(0, 0);
  lcd.print(rtc.getTimeStr());
  digitalWrite(led, LOW);
  if(Irms < 0.2){  //  Evita leitura fantasma
    parado = (t.min);
    minuto_desligado = parado - minuto_result;
    EEPROM.write(1, minuto_desligado);
    lcd.setCursor(11, 0);
    lcd.print("D: ");
    minuto_desligado = EEPROM.read(1);
    lcd.print(minuto_desligado);
    parado = 0;
    lcd.setCursor(10, 1);
    lcd.print("W: OFF");
  }
  if(Irms > 0.2){  // Quando detecta corrente
    lcd.setCursor(11, 0);
    lcd.print("L: ");
    atual = (t.min);
    if(atual != minuto_anterior){
      minuto_anterior = atual;
      minuto_result = atual - minuto_desligado;
      EEPROM.write(0, minuto_result);
    }
    minuto_result = EEPROM.read(0);
    lcd.print(minuto_result);
    //****************** Calculo **********************
    //         Watts         Horas    Dias
    kWh = (((Irms*220.0) *  (minuto_result)  *30)  /1000);
    kWh_gerado = kWh_gerado = kWh;
    lcd.setCursor(0, 1);
    lcd.print("R$: ");
    lcd.print((kWh_gerado * tarifa) + bandeira_verm + valor_publica );
    lcd.print("  W: ");
    lcd.print(Irms*220);
  }
 delay(1000);  // Atualiza o TIME
 }
}
