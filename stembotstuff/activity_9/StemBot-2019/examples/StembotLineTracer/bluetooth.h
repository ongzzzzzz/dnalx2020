//****************** BT Setting ******************//
#include <SoftwareSerial.h>
const int8_t rx = 7, tx = 8, key = 6;
SoftwareSerial BT(rx, tx); // RX, TX
//****************** BT Setting ******************//

char get_bt = ' ';
char BT_receiver(int8_t display_getBT) {
  if (BT.available() > 0) {
    get_bt = BT.read();

    if (display_getBT == 0);
    else if (display_getBT == 1) {
      #ifdef OLED
      oledDisplay(2, 0, 0);
      display.println("Controller");
      display.print("signal = ");
      display.print(get_bt);
      display.display();
      #endif
    }
  }
  return (get_bt);
}

void rename_bt(char btname[], int8_t display_name) {
  BT.print("AT+NAME");
  BT.print(btname);
  BT.print("\r\n");
  delay(500);
  BT.println("AT+RESET\r\n");
  delay(500);

  if (display_name == 0);
  else if (display_name == 1) {
    #ifdef OLED
    oledDisplay(2, 0, 0);
    display.println("My name is");
    display.println("----------");
    display.println(btname);
    display.display();
    #endif
  }
  
  delay(2000);
}

void bt_setup() {
  //****************** BT setup ******************//
  pinMode(key, OUTPUT); digitalWrite(key, LOW);
  BT.begin(9600);
  //****************** BT setup ******************//
}
