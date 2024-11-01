

class JumpCounter {

  int value;
  int lifetime;
  float x, y, r;
  int LF = 50;

  JumpCounter(int _value) {
    value= _value;
    lifetime = LF;
    x = width/2 +random(-16*5, 16*5);
    y = height/2 +random(-9*5, 9*5) - 220;
    r = radians(random(-75, 75));
  }

  void draw() {

    if (lifetime >= LF*0.8) {
      fill(255);
    } else {
      fill(map(lifetime, 0, LF*0.8, 50, 255));
    }
    pushMatrix();
    translate(x, y);
    rotate(r);
    textAlign(CENTER, CENTER);
    textSize(map(lifetime, 0, LF, 1, 1000));
    text(value, 0, 0);
    popMatrix();
  }

  void update() {
    lifetime--;
    r += 0.01;
  }

  boolean hasEnded() {
    return lifetime<0;
  }
}
