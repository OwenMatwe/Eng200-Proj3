#include <LiquidCrystal.h>
LiquidCrystal lcd(11, 12, 7, 8, 9, 10);
#include "SevSeg.h"
SevSeg sevseg;
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <avr/wdt.h>


#define SS_PIN 4
#define RST_PIN 5  /* Reset Pin */
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}

int deck[52];
int suits[52];
int values[52];

void setupDeck() {
  int y = 1;
  for (int x = 0; x < 52; x++) {
    deck[x] = y;
    y++;
  }
  for (int i = 1; i < 52; i++) {
    int j = random(0, 52);
    int t = deck[i];
    deck[i] = deck[j];
    deck[j] = t;
  }
  y = 1;
  for (int x = 0; x < 13; x++) {
    values[x] = y;
    y++;
    suits[x] = 1;
  };
  y = 1;
  for (int x = 13; x < 26; x++) {
    values[x] = y;
    y++;
    suits[x] = 2;
  };
  y = 1;
  for (int x = 26; x < 39; x++) {
    values[x] = y;
    y++;
    suits[x] = 3;
  };
  y = 1;
  for (int x = 39; x < 52; x++) {
    values[x] = y;
    y++;
    suits[x] = 4;
  };
}

int handSuits[2][6];
int handRanks[2][6];
int player = 0;
int dealer = 1;
int cards_dealt = 0;
int cards[2];


void dealCard(int person) {
  int val = deck[cards_dealt];
  handSuits[person][cards[person]] = suits[val];
  handRanks[person][cards[person]] = values[val];
  cards_dealt++;
  cards[person]++;
}

int handTotal[2][2];

void handValue(int person) {
  handTotal[person][0] = 0;
  handTotal[person][1] = 0;
  for (int i = 0; i < 6; i++) {
    if (handRanks[person][i] == 11) {
      handTotal[person][0] += 10;
      handTotal[person][1] += 10;
    } else if (handRanks[person][i] == 12) {
      handTotal[person][0] += 10;
      handTotal[person][1] += 10;
    } else if (handRanks[person][i] == 13) {
      handTotal[person][0] += 10;
      handTotal[person][1] += 10;
    } else if (handRanks[person][i] == 1) {
      handTotal[person][0] += 11;
      handTotal[person][1] += 1;
    } else {
      handTotal[person][0] += handRanks[person][i];
      handTotal[person][1] += handRanks[person][i];
    }
  }
}

byte heart[] = {
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000,
  B00000
};

byte diamond[] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte spade[] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000,
  B00000
};

byte club[] = {
  B01110,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000,
  B00000
};

byte ten[] = {
  B01000,
  B11000,
  B01000,
  B11100,
  B00010,
  B00101,
  B00101,
  B00010
};

byte numBack[] = {  //this is a B
  B11111,
  B10001,
  B10110,
  B10001,
  B10110,
  B10001,
  B11111,
  B11111
};

byte suitBack[] = {  //this is a J
  B11111,
  B11000,
  B11101,
  B11101,
  B10101,
  B11011,
  B11111,
  B11111
};

const int button = 34;
const int xPin = A1;
const int yPin = A0;
int Quit = 1;
int Play = 0;
int gamesPlayed = 0;
int cardBalence;
bool displayDeal = false;
int choice = 3;
int hit = 0;
int stand = 1;
int playerCard;
int dealerCard;
int eepromAdress;
int readCardYet = 0;
int readAddress;

void rfidRead(){
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "09 BC CE B2") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    cardBalence = EEPROM.read(1);
    eepromAdress = 1;
    readCardYet = 1;
    delay(750);

  }
  else if (content.substring(1) == "E3 43 56 33") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    cardBalence = EEPROM.read(2);
    readCardYet = 1;
    eepromAdress = 2;
    delay(750);
  }
    else if (content.substring(1) == "E3 B1 5D 33") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Admin Found");
    Serial.println();
    EEPROM.write(1, 50);
    EEPROM.write(2,50);
    readCardYet = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Resetting");
    lcd.setCursor(0,1);
    lcd.print("Balances");
    delay(2000);
    reboot();   
    
    


  }

 else   {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error: Invalid");
    lcd.setCursor(5,1);
    lcd.print("Card");
    delay(3000);
  }
} 

void displayHands() {
  lcd.setCursor(0, 0);
  lcd.print("D:");
  display(dealer);
  lcd.setCursor(0, 1);
  lcd.print("P:");
  display(player);
}

void display(int person) {
  for (int i = 0; i < 6; i++) {
    if (person == dealer && i == 0 && displayDeal == false) {
      lcd.write((uint8_t)4);
      lcd.write((uint8_t)5);
    } else {
      switch (handRanks[person][i]) {
        case 0:
          lcd.print(" ");
          break;
        case 1:
          lcd.print("A");
          break;
        case 10:
          lcd.write((uint8_t)6);
          break;
        case 11:
          lcd.print("J");
          break;
        case 12:
          lcd.print("Q");
          break;
        case 13:
          lcd.print("K");
          break;
        default:
          lcd.print(handRanks[person][i]);
      }
      switch (handSuits[person][i]) {
        case 1:
          lcd.write((uint8_t)0);
          break;
        case 2:
          lcd.write((uint8_t)1);
          break;
        case 3:
          lcd.write((uint8_t)2);
          break;
        case 4:
          lcd.write((uint8_t)3);
          break;
      }
    }
  }
}

void reset() {
  handTotal[dealer][0] = 0;
  handTotal[dealer][1] = 0;
  handTotal[player][0] = 0;
  handTotal[player][1] = 0;
  cards[dealer] = 0;
  cards[player] = 0;
  cards_dealt = 0;
  displayDeal = false;
  choice = 3;
  for (int i = 0; i < 7; i++) {
    handRanks[dealer][i] = 0;
    handSuits[dealer][i] = 0;
    handRanks[player][i] = 0;
    handSuits[player][i] = 0;
  }
}

int move;
int hover;
int currentBet = 1;
int minBet = 1;
int change = 1;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.createChar(1, diamond);
  lcd.createChar(2, spade);
  lcd.createChar(3, club);
  lcd.createChar(4, numBack);
  lcd.createChar(5, suitBack);
  lcd.createChar(6, ten);
  lcd.clear();
  randomSeed(analogRead(A7));
  byte numDigits = 4;
  byte digitPins[] = {30, 31, 32, 33};
  byte segmentPins[] = {29, 22, 23, 25, 26, 28, 27, 24};
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  bool resistorsOnSegments = true; 
  byte hardwareConfig = COMMON_CATHODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);

  pinMode(button, INPUT);
  digitalWrite(button, HIGH);
  Serial.begin(9600);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("   Welcome to"); 
  lcd.setCursor(0, 1);
  lcd.print("   Blackjack!");
  if (digitalRead(button) == 0) {
    if (gamesPlayed == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please Scan Card");
      while (readCardYet == 0) {
        
        rfidRead();
      }
    }
    while (cardBalence > 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Your Balance is");
      for (int i = 0; i < 4; i++) {
        sevseg.setNumber(cardBalence);
        sevseg.refreshDisplay();
        delay(750);
      }
      gamesPlayed += 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place your Bet");
      lcd.setCursor(0, 1);
      lcd.print("bet: $");
      if (cardBalence >= 5){
        currentBet = 5;
      }
      else {
      currentBet = cardBalence;
      }
      lcd.print(currentBet);
      //Here
      while (digitalRead(button) != 0) {
        lcd.setCursor(6, 1);
        if (analogRead(yPin) > 520 && currentBet <= (cardBalence - minBet)) {
          currentBet = currentBet + change;
          lcd.print(currentBet);
          delay(500);
        } else if (analogRead(yPin) < 480 && currentBet >= (minBet + minBet)) {
          currentBet = currentBet - change;
          if (currentBet == 5) {
            lcd.print(currentBet);
            lcd.print(" ");
          } else {
            lcd.print(currentBet);
          }
          delay(500);
        }
      }
      lcd.clear();
      reset();
      setupDeck();
      int a = 0;  // 1 is case where ace is equal to 1
      int b = 0;
      dealCard(dealer);
      dealCard(player);
      dealCard(dealer);
      dealCard(player);
      dealerCard = 2;
      playerCard = 2;
      handValue(player);
      handValue(dealer);
      displayHands();
      if (handTotal[player][a] > 21) {
        a = 1;
      }
      if (handTotal[dealer][b] > 21) {
        b = 1;
      }
      if (handTotal[player][a] == 21) {  //player natural blackjack make changes so that you can still see your cards beforehand
        displayDeal = true;
        displayHands();
        if (handTotal[dealer][b] == 21) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("You Push");
          delay(2000);
                 
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Player Blackjack");
          delay(2000);
          
          cardBalence += currentBet;
        }
      } else {
        while (playerCard < 6 && handTotal[player][a] < 21 && choice != stand) {
          handValue(player);
          lcd.clear();
          displayHands();
          lcd.setCursor(13, 0);
          lcd.print("Hit");
          lcd.setCursor(12, 1);
          lcd.print("Stay");
          move = 0;
          choice = 3;
          delay(1000);
          while (move == 0) {
            if (analogRead(yPin) > 520) {
              lcd.setCursor(12, 0);
              lcd.blink();
              hover = hit;
            } else if (analogRead(yPin) < 480) {
              lcd.setCursor(11, 1);
              lcd.blink();
              hover = stand;
            }
            if (digitalRead(button) == 0 && hover == hit) {
              choice = hit;
              move = 1;
              delay(1000);
            } else if (digitalRead(button) == 0 && hover == stand) {
              choice = stand;
              move = 1;
              delay(1000);
            }
          }
          if (choice == hit) {
            dealCard(player);
            displayHands();
            handValue(player);
            playerCard += 1;
            choice = 3;
            if (handTotal[player][a] > 21) {
              a = 1;
            }
            lcd.noBlink();            
            delay(2000);
          }
          lcd.noBlink();
          lcd.clear();
          lcd.print(handTotal[player][a]);
        }
        if (handTotal[player][a] > 21) {
          lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(handTotal[player][a]);
            lcd.print(" total");
            lcd.setCursor(0, 1);
            lcd.print("You Bust");
            delay(2000);
            cardBalence -= currentBet;
          }                   
          
        else {
          displayDeal = true;
          displayHands();
          if (handTotal[dealer][b] == 21) {
            lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Dealer Blackjack");
              lcd.setCursor(0, 1);
              lcd.print("You lose");
              delay(2000);
            
            cardBalence -= currentBet;
          } else {
            while (dealerCard < 6 && handTotal[dealer][b] < 17) {
              dealCard(dealer);
              handValue(dealer);
              dealerCard += 1;
              displayHands();
              if (handTotal[dealer][b] > 21) {
                b = 1;
              }
              delay(3000);
            }
            if (handTotal[dealer][b] > 21) {
              lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Dealer Busts");
                lcd.setCursor(0, 1);
                lcd.print("You win");
                delay(2000);
              
              cardBalence += currentBet;
            } else if (handTotal[dealer][b] == handTotal[player][a]) {
              lcd.clear();
                lcd.print("You Push");
                delay(2000);
              
            } else {
              if (handTotal[dealer][b] > handTotal[player][a]) {
                lcd.clear();
                  lcd.print("Dealer Wins");
                  lcd.setCursor(0, 1);
                  lcd.print(handTotal[dealer][b]);
                  lcd.print(" to ");
                  lcd.print(handTotal[player][a]);
                  delay(2000);
               
                cardBalence -= currentBet;
              } else {
                while (digitalRead(button) != 0) {
                  lcd.clear();
                  lcd.print("You Win");
                  lcd.setCursor(0, 1);
                  lcd.print(handTotal[player][a]);
                  lcd.print(" to ");
                  lcd.print(handTotal[dealer][b]);
                  delay(500);
                }
                cardBalence += currentBet;
              }
            }
          }
        }
      }
      //Asks The player if they want to try another game
      EEPROM.write(eepromAdress, cardBalence);
      lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Continue?");
          lcd.setCursor(12, 0);
          lcd.print("Quit");
          lcd.setCursor(12, 1);
          lcd.print("Play");
          move = 0;
          choice = 3;
          delay(1000);
          while (move == 0) {
            if (analogRead(yPin) > 520) {
              lcd.setCursor(11, 0);
              lcd.blink();
              hover = Quit;
            } else if (analogRead(yPin) < 480) {
              lcd.setCursor(11, 1);
              lcd.blink();
              hover = Play;
            }
            if (digitalRead(button) == 0 && hover == Quit) {
              choice = Quit;
              move = 1;
              delay(1000);
            } else if (digitalRead(button) == 0 && hover == Play) {
              choice = Play;
              move = 1;
              delay(1000);
            }
          }
          if (choice == Quit) {
            readAddress = eepromAdress;
            while (1){
        
              lcd.noBlink();
              readCardYet = 0;
             
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Scan Card to");
              lcd.setCursor(0,1);
              lcd.print("Store Balance");
              while (readCardYet == 0) {
                rfidRead();
                }
              lcd.clear();
              if (readAddress == eepromAdress) 
              {
                lcd.setCursor(0,0);
                lcd.print("Card Scanned");
                lcd.setCursor(0,1);
                lcd.print("Come Again Soon");
                delay(500);
                reboot();
            }
              else {
                lcd.setCursor(0,0);
                lcd.print("Error: Wrong");
                lcd.setCursor(4,1);
                lcd.print("Card");
                delay(1000);
              }
            }
          }
          lcd.noBlink();
          lcd.clear();
    }
    if (cardBalence <= 0) {
      while (digitalRead(button) != 0) {   
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Balence = 0");
        lcd.setCursor(1, 1);
        lcd.print("Scan Card Again?");
        delay(500);
        rfidRead();
      }
        gamesPlayed = 0;
    }
    
  }


}
