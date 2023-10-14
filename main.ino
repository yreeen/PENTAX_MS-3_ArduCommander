//unipolar driver
#define PHASE1 10
#define PHASE2 9
#define PHASE3 6
#define PHASE4 5

//switches assign
#define FWDx4  2
#define FWDx8  3
#define FWDx16 4
#define REVx4  11
#define REVx8  12
#define REVx16 13

//command move speed scale^--
#define COMMAND_SPEED_SCALE 1.0



int pins[4] = {PHASE1, PHASE2, PHASE3, PHASE4};
int foward[4][4] =
{
  {HIGH, HIGH, LOW, LOW},
  {LOW, HIGH, HIGH, LOW},
  {LOW, LOW, HIGH, HIGH},
  {HIGH, LOW, LOW, HIGH},
};
/*
  int reverse[4][4] =
  {
  {LOW, LOW, HIGH, HIGH},
  {LOW, HIGH, HIGH, LOW},
  {HIGH, HIGH, LOW, LOW},
  {HIGH, LOW, LOW, HIGH},
  };
*/

//14cycle/1secでちょうどよい速度になるはず

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("+00:00:00#");
  pinMode(PHASE1, OUTPUT);
  pinMode(PHASE2, OUTPUT);
  pinMode(PHASE3, OUTPUT);
  pinMode(PHASE4, OUTPUT);

  pinMode(FWDx4, INPUT_PULLUP);
  pinMode(FWDx8, INPUT_PULLUP);
  pinMode(FWDx16, INPUT_PULLUP);

  pinMode(REVx4, INPUT_PULLUP);
  pinMode(REVx8, INPUT_PULLUP);
  pinMode(REVx16, INPUT_PULLUP);
}

bool commandMove = false;
float commandSpeedScale = 1.0;
void procLX200command();
String command = "";


void loop() {
  // put your main code here, to run repeatedly:
  unsigned long t = millis();
  unsigned long count = 0;

  bool reverse_flag = false;
  float speed_scale = 1.0;
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == -1)break;
    if (c == 10)break;
    command += char(c);
    //    Serial.print(char(c));
    if (/*c == 0x06 || */
      c == '#' || c == '\n')
    {
      //      if(c == 0x06)Serial.print("ACK ");
      //      else Serial.print(command + String(" "));
      procLX200command();
      command = "";
      break;
    }
  }


  //ボタン入力で正転・逆転、速度変更の係数を変える
  if (digitalRead(FWDx4) == LOW)
  {
    commandMove = false;
    speed_scale = 4.0;
  }
  if (digitalRead(FWDx8) == LOW)
  {
    commandMove = false;
    speed_scale = 8.0;
  }
  if (digitalRead(FWDx16) == LOW)
  {
    commandMove = false;
    speed_scale = 16.0;
  }
  if (digitalRead(REVx4) == LOW)
  {
    commandMove = false;
    speed_scale = 4.0;
    reverse_flag = true;
  }
  if (digitalRead(REVx8) == LOW)
  {
    commandMove = false;
    speed_scale = 8.0;
    reverse_flag = true;
  }
  if (digitalRead(REVx16) == LOW)
  {
    commandMove = false;
    speed_scale = 16.0;
    reverse_flag = true;
  }

  if (commandMove)
  {
    speed_scale = commandSpeedScale;
    if (speed_scale < 0)
    {
      reverse_flag = true;
      speed_scale = -speed_scale;
    }
  }

  count = t / (1000 / 14.0 / 1.0027855 / speed_scale);
  count %= 4;
  if (reverse_flag)count = 3 - count;

  digitalWrite(pins[0], foward[count][0]);
  digitalWrite(pins[1], foward[count][1]);
  digitalWrite(pins[2], foward[count][2]);
  digitalWrite(pins[3], foward[count][3]);

  delay(1);

}

void procLX200command()
{
  //LX200互換コマンド処理
  static bool longFormat = true;
  command.replace("\n", "");
  String ret = "";

  if(command.equals("#"))
  {  ret = "00:00:00#";

  }else if (command.charAt(0) == 0x06)
  {
    ret = "P";
  }
  else if (command.equals(":U#"))
  {
    longFormat = !longFormat;
  }
  else if (command.equals(":GVP#"))
  {
    //機器の名前
    ret = "PENTAX#";
  }
  else if (command.equals(":GVN#"))
  {
    ret = "01.0#";
  }
  else if (command.equals(":GR#") || command.equals(":GS#"))
  {
    if (longFormat)
      ret = "00:00:00#";
    else
      ret = "00:00.0#";
  }
  else if (command.equals(":GD#") || command.equals(":GA#"))
  {
    if (longFormat)
    {
      ret = "+00:00:00#";
    }
    else
    {
      ret = "+00:00#";
    }
  }
  else if (command.equals( ":Me#"))
  {
    commandMove = true;
    commandSpeedScale = 1.0 - COMMAND_SPEED_SCALE;
  }
  else if (command.equals(":Mw#"))
  {
    commandMove = true;
    commandSpeedScale = 1.0 + COMMAND_SPEED_SCALE;
  }
  else if (command.equals(":Qe#") || command.equals(":Qw#") || command.equals(":Q#"))
  {
    commandMove = false;
  }
  else if (command.length() > 2 && command[0] == ':' && command[1] == 'S')
  {
    ret = "1";
  }

  command = "";

  if (ret.length() != 0)
  {
    Serial.println(ret);
    Serial.flush();
  }
}
