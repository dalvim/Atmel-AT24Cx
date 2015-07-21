#include <Arduino.h>
#include <MemoryFree.h>
#include <Wire.h>

#include <AT24Cx.h>


// Globals
AT24Cx eeprom = AT24Cx(0x57,4096, 32, 10);
static const int NUM_BYTES = 96;
static const int FROM_WHERE = 4000;

class Person {
  public: 
    uint8_t age ;
    char name[10];
  
  Person(uint8_t age, char name[]) :
      age(age) {
    strcpy(this->name, name);
  }
  
  String ToString() {
    return "Name: " + String(name) + " Age: " + String(age) ;
  }
};

void setup() {
  Serial.begin(115200);
  
  // led off...
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW);


  //ClearExample(); 
  //FillWithNumbers();
  //WriteMoreThanOnePage() ;
  //WriteAStringBetweenPages() ;
  //LimitTests();
  WriteObject();

}

void loop() {}



void ClearExample() {
  Serial.println("ClearExample()");
  eeprom.Clear();
  eeprom.Print(0, eeprom.Capacity(), "Fully zeroed");
}

void FillWithNumbers() {
  Serial.println("FillWithNumbers()");
  for (int i=0; i < eeprom.Capacity(); i++)
  eeprom.Write(i, i);
  eeprom.Print(0, eeprom.Capacity(), "Chunks from 0 to 255 till the end.");
}

void WriteMoreThanOnePage() { 
  Serial.println("WriteMoreThanOnePage()");
  
  uint8_t chunkSize = 100;
  uint8_t data[chunkSize] ;
  uint8_t chunks = chunkSize < eeprom.PageSize() ? (eeprom.PageSize() / chunkSize + 1) : 1;


  int startingAddress = 10;
  eeprom.Clear(chunks * chunkSize + startingAddress + 10); // clear all and a bit more...
  
  // fill data with sequential numbers
  for (int i=0; i < chunkSize; i++) {
    data[i] = i;
  }
  
  // Write chunks over two pages
  for (int i = 0; i < chunks; i++) {
    eeprom.Write(startingAddress + chunkSize*i, data, chunkSize);
  }
  
  // Print the results
  eeprom.Print(0, startingAddress + (chunks * chunkSize) +  10);
}


void WriteAStringBetweenPages() {
  Serial.println("WriteAStringBetweenPages()");

  int startingAddress = eeprom.PageSize() - 5 ;
  char data[] = "Hello World" ; // strlen == 11 which overlaps page if written at starting Address..
  
  
  eeprom.Fill('_', startingAddress + strlen(data) + 10); // zeroes is printed as spaces so we use underscore
  eeprom.Write(startingAddress, data, strlen(data)) ;
  
  eeprom.Print(0, startingAddress + strlen(data) + 10); // prints the ASCII codes for Hello World
  
  
  // Print as chars
  Serial.println("As chars: ");
  char readData[startingAddress + strlen(data) + 10]; 
  eeprom.Read(0, readData, startingAddress + strlen(data) + 10);
  for (int i = 0 ; i < startingAddress + strlen(data) + 10; i++) {
    Serial.print(readData[i]);
  }
  Serial.print('\n');  
}


void LimitTests() {
  Serial.println("LimitTests()") ;
  
  Serial.println("Writing on the edge... ");
  uint8_t dataSize = 20;
  uint8_t data[dataSize] ;
  int startingAddress = (eeprom.Capacity() / eeprom.PageSize() - 2) * eeprom.PageSize();
  
  for (int i = 0; i < dataSize; i++) {
    data[i] = i;
  }
  
  
  eeprom.Clear(0, eeprom.PageSize()); // ckear first page.
  eeprom.Clear(startingAddress-5, eeprom.Capacity() - startingAddress); // clear last two pages
  
  
  AT24Cx::ReturnCode rt = eeprom.Write(eeprom.Capacity() - dataSize/2, data, dataSize);
  Serial.println("Return Code: " + rt);

  eeprom.Print(0, eeprom.PageSize(), "First page:"); 
  eeprom.Print(startingAddress, eeprom.Capacity() - startingAddress, "Last two pages:"); 
  
}


void WriteObject() {
  Serial.println("WriteObject(): Writes two objects. The first one across two pages");
  
  Person p1 = Person(101, "John Doe");
  Person p2 = Person(99, "Jane Doe");
  
  uint16_t startingAddress = eeprom.PageSize() - sizeof(Person) / 2 ;
  Serial.println("Page Size: " + String(eeprom.PageSize()) + " StartingAddress: " + String(startingAddress) + " sizeof(Person): " + String(sizeof(Person)));

  eeprom.Clear(startingAddress - 5, 3 * sizeof(Person)); // clear a bit wider that what we're writing...
  
  eeprom.Write(startingAddress, p1);
  eeprom.Write(startingAddress + sizeof(Person), p2);
  
  Person readPerson = Person(0, "");

  eeprom.Read(startingAddress, readPerson);
  Serial.println(readPerson.ToString());
  
  eeprom.Read(startingAddress + sizeof(Person), readPerson);
  Serial.println(readPerson.ToString());
  
  //Serial.println("Showing all the bytes written starting 5 before and up sizeof(Person) * 3");
  //eeprom.Print(startingAddress - 5, 3 * sizeof(Person));
  
}


