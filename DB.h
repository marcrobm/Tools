#include <string>
using namespace std;
class DB {
  union {
    char CHAR;
    int INT;
    long LONG;
    float FLOAT;
    double DOUBLE;
    char CHARS[8];
  } CONV;

 public:
  bool ERROR = false;
  uint RamUsage();
  void Setstring(string KeyW, string Value);
  string Getstring(string KeyW);
  void SetInt(string KeyW, int value);
  int GetInt(string KeyW);
  void SetFloat(string KeyW, float value);
  float GetFloat(string KeyW);
  DB(int sizes);
  ~DB() { free(Buf); }
#ifdef SDSupport
  DB(File Load);
  Save(File Sa);
#endif
 private:
  char* Buf;
  uint Keycount;
  uint s = 0;  // Currently used space
  uint cbufs = 0;  // Current Buffer size
  uint sizeincrements = 10;  // low value results in low ram usage, high value
                             // results in higher speed

  void shrink();
  void Extend(int size);
  uint available();
  void ADD(string Key, char* value, uint16_t length);
  char* CGet(string KeyW);
};