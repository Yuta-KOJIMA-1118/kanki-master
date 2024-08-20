int INPUT1 = 0; // センサーからの入力
int HEATER = 13; // センサーのヒーター電源
int SENSER_ON = 12; // センサー電源
int OUTPUT1 = 4; // ステッピングモーターのIN1
int OUTPUT2 = 5;
int OUTPUT3 = 6;
int OUTPUT4 = 7; // ステッピングモーターのIN4

// デモ用と本番用で閾値を変更する必要がある．
// ただし，５時間以上使用するとセンサーが安定するという事例があるため，具体的な値は変化する可能性が高い．
// デモの環境　　　　：キンカン(右URL)を近づける https://www.kinkan.co.jp/products/kinkan20/
// デモの時の低い閾値：200　　部屋でやることを考慮して低めに設定
// デモの時の高い閾値：230　　アンモニアには反応が弱いため低めに設定，これでも反応が悪い
// 本番の環境：トイレ
// 本番の時の低い閾値：600　　トイレのため，高めに設定
// 本番の時の高い閾値：630　　硫黄化合物系には強く反応するため高めに設定
int THRESHOLD_LOW = 200; // 低い方の閾値 これを導入することで，連続で駆動しないようにできる．
int THRESHOLD_HI = 230; // 高い方の閾値


int ANGLE = 45;
int INTERVAL = 20; // onActionとoffActionのインターバル
int TRUE = 1;
int FALSE = 0;

int val = 0;
int interval = 0;
int onFlag = FALSE; // onActionをした後はTRUE, offActionをした後はFALSE
int doFlag = TRUE; // offActionをたらFALSEになる．その後一度でも匂いが閾値を大きく下回ったらTRUEにもどる

int STEP = 512; // モーターの一周のステップ数，モーターの性能に依存する．18BYJ-48は512 5V DC を使用している．

int stepsForward[4][4] = {
  {1,0,0,0},
  {0,1,0,0},
  {0,0,1,0},
  {0,0,0,1}
};

int stepsBackward[4][4] = {
  {0,0,0,1},
  {0,0,1,0},
  {0,1,0,0},
  {1,0,0,0}
};

void setStepperPins(int step[4]) {
  digitalWrite(OUTPUT1, step[0]);
  digitalWrite(OUTPUT2, step[1]);
  digitalWrite(OUTPUT3, step[2]);
  digitalWrite(OUTPUT4, step[3]);
}

void stepForward(int steps) {
  for(int i=0; i<steps; i++) {
    for(int j=0;j<4;j++) {
      setStepperPins(stepsForward[j]);
      delay(5);
    }
  }
  int reset[4] = {0,0,0,0};
  setStepperPins(reset);
}

void stepBackward(int steps) {
  for(int i=0; i<steps; i++) {
    for(int j=0;j<4;j++) {
      setStepperPins(stepsBackward[j]);
      delay(5);
    }
  }
  int reset[4] = {0,0,0,0};
  setStepperPins(reset);
}

int angleToStep(int angle) {
  return (int)((double)angle / 360.0 * (double)STEP);
}

int read(int inputPin)
{
  digitalWrite(SENSER_ON, HIGH);
  delay(5);

  int ret = 0;
  ret = analogRead(inputPin);

  digitalWrite(SENSER_ON, LOW);

  ret = 1023 - ret; // これをしないと，匂いが強い時に小さく，弱い時に大きくなるため，反転させる．
  Serial.println(ret);
  return ret;
}

void heat() {
  digitalWrite(HEATER, HIGH);
  delay(8);
  digitalWrite(HEATER, LOW);
  delay(237);
}

void onAction()
{
  int step = angleToStep(ANGLE);
  Serial.println("onAction");
  Serial.println(step);
  stepForward(step);
  Serial.println("endOnAction");
}

void resetAction()
{
  int step = angleToStep(ANGLE);
  Serial.println("resetAction");
  Serial.println(step);
  stepBackward(step);
  Serial.println("endResetAction");
}

void setup()
{
  pinMode(SENSER_ON, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  pinMode(OUTPUT3, OUTPUT);
  pinMode(OUTPUT4, OUTPUT);

  digitalWrite(HEATER, LOW);
  digitalWrite(SENSER_ON, LOW);

  Serial.begin(9600);
}

void loop()
{
  heat(); //delay(245)を含む
  val = read(INPUT1);

  if(val < THRESHOLD_LOW) {
    doFlag = TRUE;
  }

  if(val > THRESHOLD_HI && onFlag == FALSE && doFlag == TRUE) {
    onAction();
    onFlag = TRUE;
  }
  if(onFlag == TRUE) {
    interval += 1;
  }
  else {
    interval = 0;
  }

  if(interval >= INTERVAL && onFlag == TRUE) {
    resetAction();
    onFlag = FALSE;
    doFlag = FALSE;
  }
}