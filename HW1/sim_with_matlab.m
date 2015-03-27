clc, clear

% syms K_p K_i K_d T_f K T h s z

s  = tf('s');

Ki = 1;
Kp = 0;
Kd = 0;
Tf = 0;
h  = 0.01;

K  = 10;
T  = 0.1;
st = 1000;
t  = 0:h:st;

F = Kp + Ki/s + Kd*s/(1+Tf/s);
G = K/(1+s*T);
L = F*G;
Gry = L/(1+L);

Fd = c2d(F, h, 'tustin');
Gd = c2d(G, h, 'zoh');

Ld = Fd*Gd;
Gryd = Ld/(1+Ld);

hold on
step(Gryd, t)
step(Gry, t);

