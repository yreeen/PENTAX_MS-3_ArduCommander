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

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long t = millis();
  unsigned long count = 0;

  bool reverse_flag = false;
  float speed_scale = 1.0;

  //ボタン入力で正転・逆転、速度変更の係数を変える
  if(digitalRead(FWDx4) == LOW)
    speed_scale = 4.0;
  if(digitalRead(FWDx8) == LOW)
    speed_scale = 8.0;
  if(digitalRead(FWDx16) == LOW)
    speed_scale = 16.0;

  if(digitalRead(REVx4) == LOW){
    speed_scale = 4.0;
    reverse_flag = true;
  }
  if(digitalRead(REVx8) == LOW){
    speed_scale = 8.0;
    reverse_flag = true;
  }
  if(digitalRead(REVx16) == LOW){
    speed_scale = 16.0;
    reverse_flag = true;
  }

  count = t / (1000 / 14.0 / 1.0027855 / speed_scale);
  count %= 4;
  if(reverse_flag)count = 3 - count;

  digitalWrite(pins[0],foward[count][0]);
  digitalWrite(pins[1],foward[count][1]);
  digitalWrite(pins[2],foward[count][2]);
  digitalWrite(pins[3],foward[count][3]);

  delay(1);
  
}
