#include "DB.h"

#ifdef SDSupport
#include <SD.h>
#endif

#ifdef SDSupport
DB::DB(File Load) {
  cbufs = (uint)Load.size();
  Buf = (char*)malloc(cbufs);
  Load.read(Buf, cbufs);
  s = cbufs;
  Load.close();
}
void DB::Save(File Sa) {
  Sa.write(Buf, s);
  Sa.close();
}
#endif

DB::DB(int sizes) {
  sizeincrements = sizes;
  Buf = (char*)malloc(sizeincrements);
  cbufs = sizeincrements;
}
// Universal
void DB::ADD(string Key, char* value, uint16_t length) {
 while(available()<Key.size()+3+length){
 Extend(sizeincrements);
 }
  if (Key.length() < 1) {
    return;
  }
  char* pkey = CGet(Key);
  if (pkey != 0) {
    pkey[-1] = 1;  // rename(change last letter) existing entry to make place
                   // for new entry
  }
  // 2 bytes length
  Buf[s] = (char)(length & 0xFF);
  Buf[s + 1] = (char)(length >> 8);
  s += 2;
  // Key Add \0 terminated string
  for (int i = 0; Key[i] != '\0'; i++) {
    Buf[s] = Key[i];
    s++;
  }
  Buf[s] = '\0';
  s++;
  // Add Data
  for (int i = 0; i < length; i++) {
    Buf[s] = value[i];
    s++;
  }
  Keycount++;
}

char* DB::CGet(string KeyW) {
  int p = 0;
  while (p < s) {
    // Get Data Length
    int leng = (Buf[p + 1] << 8) | Buf[p];  // Packet length;
    // Get Key and Keylength
    string Key = "";  // inefficient but lazy
    int Keylength = 0;
    for (int i = 0; Buf[2 + p + i] != '\0'; i++) {
      Key += Buf[2 + p + i];
      Keylength++;
    }
    // calculate datastart
    int datastart = p + Keylength + 2;
    if (Key == KeyW) {
      return (&Buf[datastart]);
    }

    p += Keylength + 3 + leng;
  }
  return (0);
}
// stringEntry
void DB::Setstring(string KeyW, string Value) {
  char D[Value.size() + 1];
  copy(Value.begin(), Value.end(), D);
  D[Value.size()] = '\0';
  ADD(KeyW, D, Value.size() + 1);
}
string DB::Getstring(string KeyW) {
  char* CA = CGet(KeyW);
  string ret = "";
  for (int i = 1; CA[i] != 0; i++) {
    ret += CA[i];
  }
  return (ret);
}
// IntEntry
void DB::SetInt(string KeyW, int value) {
  CONV.INT = value;
  ADD(KeyW, &CONV.CHARS[0], 4);  // 4 bytes
}
int DB::GetInt(string KeyW) {
  char* A = CGet(KeyW);
  if (A == 0) {
    return ((int)0);
  }
  for (int i = 0; i < 4; i++) {
    CONV.CHARS[i] = A[i + 1];
  }
  return (CONV.INT);
}
// FloatEntry
void DB::SetFloat(string KeyW, float value) {
  CONV.FLOAT = value;
  ADD(KeyW, &CONV.CHARS[0], 4);  // 4 bytes
}
float DB::GetFloat(string KeyW) {
  char* A = CGet(KeyW);
  if (A == 0) {
    return ((float)0);
  }
  for (int i = 0; i < 4; i++) {
    CONV.CHARS[i] = A[i + 1];
  }
  return (CONV.FLOAT);
}
uint DB::RamUsage(){
  return(s);
}
void DB::shrink(){
 // DB temp(100);
   
}

void DB::Extend(int size) { Buf = (char*)realloc(Buf, cbufs + size);cbufs += size; }
uint DB::available(){
  return(cbufs-s);
}