%%
%%Parameters 

ki =0.1 ;
kp = 10;
kd = 0.1;
Tf = 1;
h = 0.01;
K = 10;
T = 1;
st=100; %simulation time


%%
%%Coefficeints and reference to follow
c1=(-8).*Tf;
c2=(-2).*h+4.*Tf;
c0=2.*(h+2.*Tf);
ce1=(-8).*kd+2.*h.^2.*ki+(-8).*kp.*Tf;
ce2=4.*kd+(h.*ki+(-2).*kp).*(h+(-2).*Tf);
ce0=4.*kd+(h.*ki+2.*kp).*(h+2.*Tf);
a = exp(h/T);





%% initial conditions and previous readings
y0=0;
uk_1= 0;
uk_2=0;
ek_1=0;
ek_2=0;
r=1 ;%Rference


%%
%%vector to fill
uk=[];
yk= [];

%%
%%loop iteration
t=0:h:st;
yk_act=y0;
for i=1:(length(t))
    
    ek=r-yk_act;
    
    
    uk_act= (1/c0) *((-c1*uk_1- c2*uk_2) + ...
        (ce0*ek + ce1*ek_1+ ce2*ek_2) ); 
    
    yk1= (1/a)*(yk_act + K*(a-1)*uk_act);
    
    yk= [yk; yk1];
    uk=[uk; uk_act];
    yk_act= yk1;
   
   uk_2=uk_1;
   uk_1=uk_act;
   
  
   ek_2=ek_1;
   ek_1=ek;
   
   
    
end 

%%plot the results
hold on 
 plot(t,yk);







