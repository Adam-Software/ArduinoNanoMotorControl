//Автор: ROBOT LTD CO
//Плата ARDUINO NANO
//Интерфейс UART 19200 бод
//Формат посылки ADDR,SPEED1,DIR1,SPEED2,DIR2,CLR,CHECKSUMM;
//ADDR - адрес устройства, SPEED1 - скорость мотора 1 (0-1023), DIR1 -направление мотора 1 (0-1)
//SPEED2 - скорость мотора 2 (0-1023), DIR2 - направление мотора 2 (0-1)
//CLR - очистка счетчиков импульсов (0-нет, 1- очистка счетчика 1, 2 - очистка счетчика 2)
//CHECKSUMM = ADDR+SPEED1+DIR1+SPEED2+DIR2+CLR - контрольная сумма
//';' - конец посылки
//ФОРМАТ возврата ADDR,ID,CNT
//ADDR - адрес устройства, ID - индекс мотора, CNT - счетчик импульсов
//MOTOR1: D5 - PWM , D4 - FEEDBACK , D3 - DIR 
//MOTOR2: D6 - PWM , D7 - FEEDBACK , D8 - DIR 

const int MOTOR1 = 5;                         //используем 5 вывод для управления DC-мотором
const int MOTOR2 = 6;                        //используем 6 вывод для управления DC-мотором

const int PLS1 = 4;                          // Настраиваем 4 вход обратной связи мотора 5
const int PLS2 = 7;                         // Настраиваем 7 вход обратной связи мотора 6
const int DIR1 = 3;                        // Настраиваем 3 вывод на выход напрвления вращения мотора 5
const int DIR2 = 8;                       // Настраиваем 8 вывод на выход напрвления вращения мотора 6

const int speed = 19200;                       // скорость

int val1 = 0;                                 //переменная для хранения считываемого значения 
int val2 = 0;                                //переменная для хранения считываемого значения 

int PlsState1 = 0;                          // Состояние обратной связи 4
int PlsState2 = 0;                          // Состояние обратной связи 7
int count1 = 0;                             // Аккумулятор счетчика импульсов 1
int count2 = 0;                             // Аккумулятор счетчика импульсов 2

int rpm1 = 0;                               // Устанавливаем счётчик оборотов в 0
int rpm2 = 0;                               // Устанавливаем счётчик оборотов в 0

int flag1=0;                                   // Устанавливаем флаг в 0
int flag2=0;                                   // Устанавливаем флаг в 0

int State1 = 0;                                // Устанавливаем состояние направления в 0
int State2 = 0;                                // Устанавливаем состояние направления в 0

int ClrCounter = 0;                            //Очистка аккумулятора счетчиков (0 - ничего не делаем, 1- очистка счетчика 1, 2- очистка счетчика 2)

int address = 25;                               //Адрес устройства
int ID1 = 1;                                   // ID мотора 1
int ID2 = 2;                                  // ID мотора 2
int CheckSum = 0;                             // Аккумулятор контрольной суммы

int data[10];

void setup()
{
    Serial.begin(speed);
    pinMode(PLS1, INPUT_PULLUP);
    pinMode(PLS2, INPUT_PULLUP);
    pinMode (DIR1, OUTPUT);                    //конфигурируем 3 вход как выход
    pinMode (DIR2, OUTPUT);
    pinMode (MOTOR1, OUTPUT);                //конфигурируем 5 вход как выход
    pinMode (MOTOR2, OUTPUT);                //конфигурируем 6 вход как выход    
    analogWrite(MOTOR1, val1);                         // Устанавливаем везде 0(нули)
    analogWrite(MOTOR2, val2);                         // Устанавливаем везде 0(нули)
}

void loop()
{

 if (Serial.available()) {
   char str[30];
   int amount = Serial.readBytesUntil(';', str, 30);
   str[amount] = NULL;
   int data[10];
   int count = 0;
   char* offset = str;
   while (true) {
     data[count++] = atoi(offset);
     offset = strchr(offset, ',');
     if (offset) offset++;
     else break;
   }

    if (data[0] == address) {
    CheckSum = data[0] + data[1] + data[2] + data[3] + data[4] + data[5];  //рассчет контрольной суммы
    }

    if (data[0] == address && CheckSum == data[6]) {                      //проверка адреса и контрольной суммы
    val1 = data[1];
    State1 = data[2];
    val2 = data[3];
    State2 = data[4];
    ClrCounter = data[5];

    if (ClrCounter == 1) {
      count1 = 0;
    }

    if (ClrCounter == 2) {
      count2 = 0;
    }
    
    digitalWrite(DIR1, State1);
    digitalWrite(DIR2, State2);
     
    val1 = map(val1, 0, 1023, 0, 255);        //пропорционально преобразуем измеряемый диапазон значений в выдаваемый диапазон значений
    analogWrite(MOTOR1, val1);                //выдаем на 0 выход преобразованное значение 

    val2 = map(val2, 0, 1023, 0, 255);        //пропорционально преобразуем измеряемый диапазон значений в выдаваемый диапазон значений
    analogWrite(MOTOR2, val2);                //выдаем на 0 выход преобразованное значение      
  } 

}

  PlsState1 = digitalRead(PLS1);
  if (!PlsState1 == HIGH && flag1==0 && State1==1) {
        flag1=1;
        count1=count1+1;
        rpm1=count1/211.2;

        Serial.print(address);
        Serial.print(",");
        Serial.print(ID1);
        Serial.print(",");
        Serial.println(count1);
      }
      if (!PlsState1 == LOW && flag1==1 && State1==1) {
        flag1=0;

      }
  if (!PlsState1 == HIGH && flag1==0 && State1==0) {
        flag1=1;
        count1=count1-1;
        rpm1=count1/211.2;

        Serial.print(address);
        Serial.print(",");
        Serial.print(ID1);
        Serial.print(",");
        Serial.println(count1);
       }
      if (!PlsState1 == LOW && flag1==1 && State1==0) {
        flag1=0;

      }

  PlsState2 = digitalRead(PLS2);
  if (!PlsState2 == HIGH && flag2==0 && State2==1) {
        flag2=1;
        count2=count2+1;
        rpm2=count2/211.2;

        Serial.print(address);
        Serial.print(",");
        Serial.print(ID2);
        Serial.print(",");
        Serial.println(count2);
      }
      if (!PlsState2 == LOW && flag2==1 && State2==1) {
        flag2=0;

      }
  if (!PlsState2 == HIGH && flag2==0 && State2==0) {
        flag2=1;
        count2=count2-1;
        rpm2=count2/211.2;

        Serial.print(address);
        Serial.print(",");
        Serial.print(ID2);
        Serial.print(",");
        Serial.println(count2);
       }
      if (!PlsState2 == LOW && flag2==1 && State2==0) {
        flag2=0;

      }   
}
