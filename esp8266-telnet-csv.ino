// Augusto Campos - makernews.info - 20.11.2015
// details available (in portuguese) here: http://br-arduino.org/2015/11/servidor-esp8266-telnet-csv.html

#define VELOCIDADE 115200

String REDE="casablanca";
String SENHA="marrocos";

String IPLIVRE="192.168.0.235";
String MASCARA="255.255.255.0";
String ROTEADOR="192.168.0.254";

#include <SoftwareSerial.h>
SoftwareSerial monitorSerial(9, 8); // RX, TX

#define debug
#define wxindows

void cmdESP(String cmd, String msg="", int limite=7000) {
  if (msg!="") monitorSerial.println(msg);
  if (cmd!="") Serial.println(cmd);
  unsigned long chegada=millis();
  boolean continuar=true; 
  String S="";
  unsigned long ultimochar=0;
  while (continuar) { 
    if (Serial.available()) {
      char c = Serial.read();
      ultimochar=millis();
      S=S+c;
      if (c==10) {  // LF, fim da linha recebida
        byte p=S.indexOf(13);
        String S1=S.substring(0,p);
        if (S1=="OK") continuar=false;
        else if (S1=="SEND OK") continuar=false;
        else if (S1=="ready") continuar=false;
        else if (S1=="no change") continuar=false;
        else if (S1=="ERROR") continuar=false;
        monitorSerial.print(S);
        S="";
      }  
    }  
    if (millis()-chegada > limite) continuar=false;
  }
  if (S!="") monitorSerial.print(S);      
}

void ESPsendStr(const char *ESPlinha) {
  int comprimento=strlen(ESPlinha);
  Serial.print("AT+CIPSEND=0,");
  Serial.println(comprimento,DEC);
  Serial.find("> ");  
  Serial.print(ESPlinha);
  cmdESP("","Aguardando envio");
}  

void conectaIPfixo() {
  String cmdConectar="AT+CWJAP=\""+REDE+"\",\""+SENHA+"\"";
  String cmdMudarIP="AT+CIPSTA_DEF=\"" + IPLIVRE + "\",\"" + ROTEADOR +"\",\"" + MASCARA + "\"";
  cmdESP("AT+CWMODE=3", "Modo de operacao misto, AP + STATION");
  cmdESP("AT+GMR", "Identifica versão do firmware");
  cmdESP(cmdConectar,"Conectando a uma rede WiFi",30000);
  cmdESP(cmdMudarIP,"Definindo nosso proprio IP fixo");
}


void ativaTelnet() {
  cmdESP("AT+CIPMUX=1", "Ativa multiplas conexoes, necessario para estabelecer servidor");
  cmdESP("AT+CIPSERVER=1,2222", "Estabelece servidor na porta 2222 (acessivel via telnet)");
}


int HIST[90];

boolean recebeConexao() {
  unsigned long limite=7000;
  unsigned long chegada=millis(); 
  char resp[100]="";
  unsigned long tempo;
  boolean continuar=true; 
  boolean timeout=false;
  int contaChars=0;
  while (continuar) { 
    if (Serial.available()) {
      unsigned long chegada=millis();   // recomeca a contar quando ha recepcao
      resp[contaChars] = Serial.read();
      contaChars++;
      if (contaChars>80) contaChars=0;  // aqui deveria haver uma condicao de erro
#ifdef debug
      monitorSerial.print(resp[contaChars-1]);
#endif
      if (resp[contaChars-1]==10) {  // LF, fim da linha recebida
        if (contaChars>1) {
          resp[contaChars-2]=char(0);
          if (0==strcmp(resp,"OK")) continuar=false;
          else if (0==strcmp(resp,"ERROR")) continuar=false;
          else if (0==strcmp(resp,"0,CONNECT")) {
            continuar=false;
            ESPsendStr("**+ Segue lista decrescente das leituras armazenadas:\n\r");
            for (byte _d=0;_d<=7;_d++) {
              for (byte _u=0;_u<=9;_u++) {
                char digitos[4];                
                itoa(HIST[_d*10+_u],digitos,10);
                ESPsendStr(digitos);
                ESPsendStr(";");
              }
#ifdef windows
              ESPsendStr("\n\r");   
#else
              ESPsendStr("\n");   
#endif              
            }
            ESPsendStr("**- Fim da lista. Desconectando.\n\n\r");
            cmdESP("AT+CIPCLOSE=0","Encerrando conexao recebida");
          }  
          contaChars=0;
        }  
      }  
    }  
    tempo=millis()-chegada; // tempo sem recepcao
    if (tempo > limite) {
      timeout=true;
      continuar=false;
    }  
  }
}



void setup() {
  for (byte i=0;i<80;i++) HIST[i]=random(0,1000);
  monitorSerial.begin(VELOCIDADE);
  monitorSerial.println("*** ESP8266 Static IP / IP fixo - BR-Arduino.org");
  delay(2000);
  Serial.begin(VELOCIDADE);
  Serial.setTimeout(5000);
  conectaIPfixo();
  ativaTelnet();
  monitorSerial.println("Aguardando conexoes");  
}


void loop() {
  recebeConexao();
}
 
