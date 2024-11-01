int saltos = 0;

ArrayList<JumpCounter> jumps;

void setup() {
  fullScreen();
  noStroke();
  background(0);

  jumps = new ArrayList<JumpCounter>();
}

void draw() {

  for (int i = jumps.size()-1; i >= 0; i--) {
    JumpCounter jc = jumps.get(i);
    jc.update();
    if (jc.hasEnded()) {
      jumps.remove(i);
    }
  }

  background(0);

  for (int i = 0; i <jumps.size(); i++) {
    JumpCounter jc = jumps.get(i);
    jc.draw();
  }
}

void keyPressed() {
  if (key == '1') {
    saltos++;
    jumps.add(new JumpCounter(saltos));
  }
  if (key == 'z') {
    saltos=0;
  }
}
