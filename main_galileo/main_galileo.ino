// Digital ports (PCF)
int sda = 8;
int scl = 9;
int pinInt0 = 2; 

/* Analog ports
* A0 - temp Out
* A1 - temp In
* A2 - temp Engine
*/

// Car object
struct car {
 int doors;
 int seatbelts;
 int lights;
 int immo;
 int r;
 int tempOut;
 int tempIn;
 int tempEngine;
};

// Delay time
int d = 1;

struct car *audi;

void setup() {
   pinMode(13, OUTPUT);    
   Serial.begin(9600);
   Serial.println("Setup...");
   pinMode(sda, OUTPUT);
   pinMode(scl, OUTPUT);
   digitalWrite(scl, HIGH);
   digitalWrite(sda, HIGH);
   
   audi = readData();
      
   // Set interrupt - changes in pin0 (HIGH) execute function checkChanges()
   attachInterrupt(pinInt0, checkChanges, FALLING);
}

// Reading value from analog ports (temperatures)
int readTemp(int portNumber){
  return analogRead(portNumber);
}

// Generate START bit and address PCF
int readPCF(char adres){
  int m, ack, wynik = 0;
  digitalWrite(sda, LOW);
  delay(d);
  digitalWrite(scl, LOW);

  for(m = 0x80; m; m >>= 1){ // address transfer MSB->LSB
    if(adres & m)         
      digitalWrite(sda,HIGH);
    else
      digitalWrite(sda,LOW);
        
   digitalWrite(scl,HIGH); // generate clock pulse
   delay(d);
   digitalWrite(scl,LOW); 
   delay(d);
  }

   pinMode(sda,INPUT);
   digitalWrite(scl,HIGH);
   delay(d);
   
   //Read ACL
   ack = digitalRead(sda);
   digitalWrite(scl,LOW);
   delay(d);
   
   for(m = 0x80; m; m>>=1){
     digitalWrite(scl,HIGH);
     delay(d);
    
     if(digitalRead(sda)){
       wynik |= m;
     }
     
     digitalWrite(scl, LOW);
     delay(d);
   }
   
   // Generate STOP bit
   pinMode(sda,OUTPUT);
   digitalWrite(scl, HIGH);
   delay(d);
   digitalWrite(sda, HIGH);
   delay(d);
   return wynik;
}

// DEBUG
void printObj(struct car * obj){
  Serial.println("+--------------------+");
  Serial.println("| Audi object:       |");
  Serial.print("| Doors = ");
  Serial.println(obj->doors);
  Serial.print("| Seatbelts = ");
  Serial.println(obj->seatbelts);
  Serial.print("| Immo= ");
  Serial.println(obj->immo);
  Serial.print("| Lights = ");
  Serial.println(obj->lights);
  Serial.print("| R = ");
  Serial.println(obj->r);
  Serial.print("| Temp out = ");
  Serial.println(obj->tempOut);
  Serial.print("| Temp In = ");
  Serial.println(obj->tempIn);
  Serial.print("| Temp Engine = ");
  Serial.println(obj->tempEngine);
  Serial.println("+-------------------+");
}

struct car * readData(){
  struct car * tmp = (struct car *)malloc(sizeof(struct car));
  
  // Read data from first PCF device
  int data = readPCF(0x41);
  tmp->doors = (data & 3) << 2;
  tmp->seatbelts = data & 12;
  tmp->immo = (data & 16) >> 4;
  
  // Read data from second PCF device
  data = readPCF(0x43);
  tmp->doors |= data & 3;
  tmp->doors |= (data & 32) >> 1;
  tmp->seatbelts |= (data & 12) >> 2;
  tmp->lights = (data & 16) >> 4;
  tmp->r = (data & 64) >> 6;
  tmp->tempOut = readTemp(0);
  tmp->tempIn = readTemp(1);
  tmp->tempEngine = readTemp(2);
  return tmp;
}

void checkChanges(){
  digitalWrite(13, HIGH);
  struct car * tmp = readData();
  
  if(tmp->immo != audi->immo)
    Serial.println("Immo!!");
  if(tmp->doors != audi->doors)
    Serial.println("Drzwi sie zmienily");
  if(tmp->seatbelts != audi->seatbelts)
    Serial.println("Pasy sie zmienily");
  if(tmp->r != audi->r)
    Serial.println("Wsteczny!!");
  if(tmp->lights != audi->lights)
    Serial.println("Światła sie zmienily");
  if(tmp->tempOut != audi->tempOut)
    Serial.println("TempOut sie zmienilo");
  if(tmp->tempIn != audi->tempIn)
    Serial.println("TempIn sie zmienilo");
  if(tmp->tempEngine != audi->tempEngine)
    Serial.println("TempEngine sie zmienilo");
  
  audi = tmp;
  printObj(audi);
  digitalWrite(13, LOW);
}

void loop() {
  
  /* Version with ask about changes in loop
  * checkChanges();
  * delay(1000);
  */
}
