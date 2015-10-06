#include <Arduino.h>
#include <MemoryFree.h>
#include <Wire.h>

#include <AT24Cx.h>


// Globals

//I have an AT24C32 EEPROM. For a different EEPROM change the following values according to the datasheet.
#define ADDRESS 0x57
#define CAPACITY 4096
#define PAGE_SIZE 32
#define WRITE_CYCLE 10

AT24Cx eeprom = AT24Cx(ADDRESS,CAPACITY, PAGE_SIZE, WRITE_CYCLE);


class Person {
public:
    uint8_t age ;
    char name[10];

    Person(const char name[], uint8_t age) :
            age(age) {
        strcpy(this->name, name);
    }

    Person() :
            age(0) {
        strcpy(this->name, "");
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


  /**
  * Writes bytes overflowing the EEPROM's capacity.
  * By factory default data would be continue to be written
  * wraping at the beginning. Instead, this lib truncates data.
  *
  * Means overflows are truncated and ignored.
  */
  WriteOverflow();
  
  
  //WriteObject();

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


void WriteOverflow() {
  Serial.println("WriteOverflow()") ;

  // fill an array of bytes
  uint8_t dataSize = 20;
  uint8_t data[dataSize] ;
  for (int i = 0; i < dataSize; i++) {
    data[i] = i;
  }

  // lastTwoPagesAddress starts 2 pages before Capacity().
  uint16_t lastTwoPagesAddress = (eeprom.Capacity() / eeprom.PageSize() - 2) * eeprom.PageSize();

  eeprom.Clear(0, eeprom.PageSize()); // clear first page.
  eeprom.Clear(lastTwoPagesAddress, eeprom.Capacity() - lastTwoPagesAddress); // clear last two pages

  uint16_t startingAddress = eeprom.Capacity() - dataSize/2 ; // will overflow by half the data...

  Serial.println("We'll write " + String(dataSize) + " bytes at position: " + String(startingAddress) + ". According to specs it would wrap to the first page. This lib truncates ;).");

  AT24Cx::ReturnCode rt = eeprom.Write(startingAddress, data, dataSize);
  Serial.println("Return Code: " + String(rt) + " ... zero means success ;)");

  eeprom.Print(0, eeprom.PageSize(), "First page:");
  eeprom.Print(lastTwoPagesAddress, eeprom.Capacity() - lastTwoPagesAddress, "Last two pages:");
}


void WriteObject() {
  Serial.println("WriteObject(): Writes two objects. The first one across two pages");

  Person p1 = Person("John Doe", 30);
  Person p2 = Person("Jane Doe", 28);

  uint16_t startingAddress = eeprom.PageSize() - sizeof(Person) / 2 ;
  Serial.println("Page Size: " + String(eeprom.PageSize()) + " StartingAddress: " + String(startingAddress) + " sizeof(Person): " + String(sizeof(Person)));

  eeprom.Clear(startingAddress - 5, 3 * sizeof(Person)); // clear a bit wider that what we're writing...

  eeprom.Write(startingAddress, p1);
  eeprom.Write(startingAddress + sizeof(Person), p2);

  Person readPerson;

  eeprom.Read(startingAddress, readPerson);
  Serial.println(readPerson.ToString());

  eeprom.Read(startingAddress + sizeof(Person), readPerson);
  Serial.println(readPerson.ToString());

  //Serial.println("Showing all the bytes written starting 5 before and up sizeof(Person) * 3");
  //eeprom.Print(startingAddress - 5, 3 * sizeof(Person));

}
