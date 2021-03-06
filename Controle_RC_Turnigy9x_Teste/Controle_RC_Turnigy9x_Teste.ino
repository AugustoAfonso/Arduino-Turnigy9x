#define ch1 11
#define ch2 10 
#define MotorA 5 //Velocidade Motor A(0-255)
#define MotorB 6 //Velocidade Motor B(0-255)
#define DirA 7 //Direção Motor A
#define DirB 8 //Direção Motor B
double ch1Raw = 0;
double ch1Adj = 0;
double ch2Raw = 0;
double ch2Adj = 0;
double r=0;
double rAdj=0;
float theta=0;
float thetaD=0;
float velB=0;
float velA=0;
int dir=0;
/*dir=0 centro
  dir=1 frente esquerda
  dir=2 frente direita
  dir=3 ré esquerda
  dir=4 ré direita
  dir=5 frente total
  dir=6 ré total
  dir=7 esquerda total
  dir=8 direita total
  
*/
void setup() {
 pinMode(11,INPUT);
 pinMode(10,INPUT);
 pinMode(MotorA,OUTPUT);
 pinMode(MotorB,OUTPUT);
 pinMode(DirA,OUTPUT);
 pinMode(DirB,OUTPUT);
 Serial.begin(9600);

}

void loop() {
  readCh();
  adjCh();
  //printCh();
  //printAdjCh();
  recToPolar();
  pwmMotor();
  
  
  /*Serial.print("thetaD:");
  //Serial.println(thetaD);
  Serial.print("r:");
  Serial.println(r);
  Serial.print("velA:");
  Serial.println(velA);
  Serial.print("velB:");
  Serial.println(velB);
  delay(500);*/
  
  
}
void readCh(){ 
  ch1Raw = pulseIn(ch1,HIGH);//Lê  o tempo que os pinos conectados aos canais do receptor 
  ch2Raw = pulseIn(ch2,HIGH);//Turnigy 9X permaneceram ativos durante o pulso recebido
}

void adjCh(){
  ch1Adj=map(ch1Raw,1030,1860,-255,255);//Remapeia os valores recebidos dos canais, ajustando-os 
  ch2Adj=map(ch2Raw,1030,1860,-255,255);//ajustando-os para valores entre -255 e +255
  if(ch1Adj>-10 && ch1Adj<10){
    ch1Adj=0;                  //Cria deadzones, ou zonas mortas p/ compensar variações 
  }                            //naturais na centralização do joystick
  if(ch2Adj>-20 && ch2Adj<20){
   ch2Adj=0;
  }
}

void printCh(){
  Serial.print("Ch1 = ");
  Serial.print(ch1Raw);
  Serial.print(" -  ");  //Imprime os Valores lidos pelos canais (somente DEBUG)
  Serial.print("Ch2 = ");
  Serial.print(ch2Raw);
  Serial.println(" -  ");
  delay(500);
}
void printAdjCh(){
  Serial.print("ch1Adj = ");
  Serial.print(ch1Adj);
  Serial.print(" -  ");     //Imprime os valores já remapeados (somente DEBUG).
  Serial.print("ch2Adj = ");
  Serial.print(ch2Adj);
  Serial.println(" -  ");
  delay(500);
}
void recToPolar(){
  r=sqrt(pow(ch1Adj,2)+pow(ch2Adj,2));//Executa a conversão das coordenadas retangulares(ch1Adj e ch2Adj)
  theta=atan2(ch2Adj,ch1Adj);         //em coordenada polares( raio e ângulo), de forma trigonométrica.
  if(ch1Adj<-5&&ch2Adj>5){ //Realiza as conversões necessárias entre o resultado em radianos gerado pelas funções 
    thetaD=theta*(180/PI);//trigonométricas da Linguagem C, para Graus, e define os quadrantes do movimento segundo a tabela acima (dir)
    dir=1;
  }
  if(ch1Adj>5&&ch2Adj>5){
    thetaD=theta*(180/PI);
    dir=2;
  }
  if(ch1Adj<-5&&ch2Adj<-5){
    thetaD=360+(theta*(180/PI));
    dir=3;
  }
  if(ch1Adj>5&&ch2Adj<-5){
    thetaD=360+(theta*(180/PI));
    dir=4;
  }
  if(ch1Adj>-5&&ch1Adj<5&&ch2Adj>-5&&ch2Adj<=255){
    r=ch2Adj;
    thetaD=90;
    dir=5;
  }
  if(ch1Adj>-5&&ch1Adj<5&&ch2Adj<5&&ch2Adj>=-255){
    r=-ch2Adj;
    thetaD=270;
    dir=6;
  }
  if(ch1Adj<5&&ch1Adj>=-255&&ch2Adj>-5&&ch2Adj<5){
    r=-ch1Adj;
    thetaD=180;
    dir=7;
  }
  if(ch1Adj>-5&&ch1Adj<=255&&ch2Adj>-5&&ch2Adj<5){
    r=ch1Adj;
    thetaD=0;
    dir=8;
  }
  if(r>-10&&r<10){
    r=0;
    thetaD=0;
    dir=0;
  }
 //---------------------------------------------------------------------------------------------------------- 
}
void pwmMotor(){ 
  /*Realiza o controle dos motores de forma diferencial, de acordo  
  com a direção e o quadrante que a alavanca do joystick se encontra.
  Utiliza-se da entidade trigonométrica do seno, pois a velocidade de 
  um dos motores deve ser menor para que o robô consiga realizar curvas, logo
  um dos motores recebe o valor total do raio como PWM, e o seu par recebe a 
  variável velA ou velB que é constituída pela multiplicação entre o raio,o  seno 
  do ângulo naquele momento e um fator de correção (possibilita curvas mais fechadas ou mais abertas)
  */
  rAdj=map(r,0,361,0,255);
 
  if(dir==0&&thetaD==0){//parada
   digitalWrite(DirA,LOW); 
   digitalWrite(DirB,LOW); 
   analogWrite(MotorA,0); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,0); //Velocidade Motor B = 0 a 255  
  }
  if(dir==1&&(thetaD!=0&&thetaD!=90&&thetaD!=180&&thetaD!=270)){//frente esquerda
   digitalWrite(DirA,HIGH); //Motor A para FRENTE
   digitalWrite(DirB,HIGH); //Motor B para FRENTE
   velA = 0.7*rAdj*sin(theta);
   analogWrite(MotorA,velA); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,rAdj); //Velocidade Motor B = 0 a 255  
  }
  if(dir==2&&(thetaD!=0&&thetaD!=90&&thetaD!=180&&thetaD!=270)){//frente direita
   digitalWrite(DirA,HIGH); //Motor A para FRENTE
   digitalWrite(DirB,HIGH); //Motor B para FRENTE
   analogWrite(MotorA,rAdj); //Velocidade Motor A = 0 a 255
   velB = 0.7*rAdj*sin(theta);
   analogWrite(MotorB,velB); //Velocidade Motor B = 0 a 255  
  }
  if(dir==3&&(thetaD!=0&&thetaD!=90&&thetaD!=180&&thetaD!=270)){//ré esquerda
   digitalWrite(DirA,LOW); //Motor A para TRÁS
   digitalWrite(DirB,LOW); //Motor B para TRÁS
   velA = 0.7*(-rAdj)*sin(theta);
   analogWrite(MotorA,velA); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,rAdj); //Velocidade Motor B = 0 a 255  
  }
  if(dir==4&&(thetaD!=0&&thetaD!=90&&thetaD!=180&&thetaD!=270)){//ré direita
   digitalWrite(DirA,LOW); //Motor A para TRÁS
   digitalWrite(DirB,LOW); //Motor B para TRÁS
   analogWrite(MotorA,rAdj); //Velocidade Motor A = 0 a 255
   velB = 0.7*(-rAdj)*sin(theta);
   analogWrite(MotorB,velB); //Velocidade Motor B = 0 a 255  
  }  
  if(dir==5&&thetaD==90){//frente
   digitalWrite(DirA,HIGH); //Motor A para FRENTE
   digitalWrite(DirB,HIGH); //Motor B para FRENTE
   analogWrite(MotorA,rAdj); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,rAdj); //Velocidade Motor B = 0 a 255
  }
  if(dir==6&&thetaD==270){//ré
   digitalWrite(DirA,LOW); //Motor A para TRÁS
   digitalWrite(DirB,LOW); //Motor B para TRÁS
   analogWrite(MotorA,rAdj); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,rAdj); //Velocidade Motor B = 0 a 255
  }
  if(dir==7&&thetaD==180){//esquerda
   digitalWrite(DirA,LOW); //Motor A para FRENTE
   digitalWrite(DirB,HIGH); //Motor B para TRÁS
   analogWrite(MotorA,rAdj); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,rAdj); //Velocidade Motor B = 0 a 255
  }    
  if(dir==8&&thetaD==0){//direita
   digitalWrite(DirA,HIGH); //Motor A para TRÁS
   digitalWrite(DirB,LOW); //Motor B para FRENTE
   analogWrite(MotorA,rAdj); //Velocidade Motor A = 0 a 255
   analogWrite(MotorB,rAdj); //Velocidade Motor B = 0 a 255
  }
}


