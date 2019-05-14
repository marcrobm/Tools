#include <Arduino.h>
#include <SD.h>

/*
Supported Types
  -Int 
  -Float
  -String
  
Functions                   
  DB Database(size);               create database with maxsize
  DB(File Load)
  DB(File Load,int size)
  Settype(String Key,type value)  D.SetInt("I",2)
  Gettype(String Key)             D.int i = GetInt("I");
  
*/

class DB{
 union {
  
  char CHAR;
  int INT;
  long LONG;
  float FLOAT;
  double DOUBLE;
  char CHARS[8]; 
 }CONV;
 public:
  char * Buf; 
  int s = 0;
  int smax;

DB(){
  
}
~DB(){
  free(Buf);
}
DB(File Load){
 
  int smax = Load.size();
  Buf = (char*)malloc(smax);
  Load.read(Buf,smax);
  Serial.println("DB Size:"+(String)smax);
  s = smax;
   Load.close();
}
void Save(File Sa){
  Sa.write(Buf,s);
  Sa.close();
}


DB(int sizes){
  Buf = (char*)malloc(sizes);
  smax = sizes;
}
//Universal
void ADD(String Key,char * value,uint16_t length){
  //2 bytes length
  Buf[s] = (char)(length & 0xFF);
  Buf[s+1] = (char)(length >> 8);
  //Serial.println((String)Buf[s] + " " + Buf[s+1] +" "+ (String)length);
  s+= 2;
  // Key Add \0 terminated String 
  for(int i = 0;Key[i] != '\0';i++){
   Buf[s] = Key[i];
   s++;
  }
  Buf[s] = '\0';
  s++;
  //Add Data
  for(int i = 0;i<length;i++){
    Buf[s] = value[i];
    s++;
  }

//Serial.println("DB LOAD:"+(String)s);
}
void dump(){
  Serial.println("SMAX:"+(String)smax);
  Serial.println("Dump:");
for(int i = 0;i<smax;i++){  
      Serial.print("[");
      Serial.print((String)(int)Buf[i]);
      Serial.print("]");   
  }
  }
char * CGet(String KeyW){
  //Serial.println("CGET["+KeyW+"]");
  int p = 0;
  while(p<s){
   //Get Data Length
    int leng = (Buf[p+1] << 8) | Buf[p];//Packet length;
   //Get Key and Keylength
    String Key = "";//inefficient but lazy
    int Keylength = 0;
    for(int i = 0;Buf[2+p+i] != '\0';i++){
      Key += Buf[2+p+i];
      Keylength++;
    }
   // Serial.println("Key found:"+Key);
   //calculate datastart 
    int datastart = p+Keylength+2;
   //output datastart
   // Serial.println("datastart:"+(String)datastart);
 
    if(Key == KeyW){
  return(&Buf[datastart]);
    }
    
  p+= Keylength+3+leng;
  }  
  return(0);
  // Serial.print("ERROR:");
}
//StringEntry
void SetString(String KeyW, String Value){ 
  int l = Value.length()+1;
  char * charBuf = (char*)malloc(l);
  Value.toCharArray(charBuf, l) ;
  ADD(KeyW,charBuf,(uint16_t)l);
  free(charBuf);
}
String  GetString(String KeyW){
  char * CA = CGet(KeyW); 
   if(CA==0){
    return("null");
  }
  return((String)&CA[1]);
}
//IntEntry
void SetInt(String KeyW, int value){
  CONV.INT = value;
  ADD(KeyW,&CONV.CHARS[0],4);//4 bytes
}
int GetInt(String KeyW){
  char * A = CGet(KeyW);
   if(A==0){
    return((int)0);
  }
  for(int i = 0;i<4;i++){
     CONV.CHARS[i] = A[i+1]; 
  }
  return(CONV.INT);
}
//FloatEntry
void SetFloat(String KeyW, float value){
  CONV.FLOAT = value;
  ADD(KeyW,&CONV.CHARS[0],4);//4 bytes
}
float GetFloat(String KeyW){
   char * A = CGet(KeyW);
  if(A==0){
    return((float)0);
  }
  for(int i = 0;i<4;i++){
     CONV.CHARS[i] = A[i+1]; 
  }
  return(CONV.FLOAT);
}
//

};

