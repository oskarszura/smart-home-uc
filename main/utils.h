float analogToVoltage(int input) { return (input / 1024.0) * 5.0; }

void riseAlert(int pin) { tone(pin, 4000, 20); }
