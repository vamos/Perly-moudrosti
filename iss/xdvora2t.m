%1
[s,Fs] = audioread('xdvora2t.wav');
s = s';
samples = length(s);
times = samples/Fs;
bin = samples/16;
fprintf("Delka vstupniho signalu:    %d s\n",times);
fprintf("Vzorkovaci frekvence:       %d [Hz]\n",Fs);
fprintf("Pocet vzorku:               %d\n",samples);
fprintf("Pocet binarnich symbolu:    %d\n",bin);


%2
alpha = 8;                      %sample
beta = 1;                       %index
sbin = zeros(1,2000);           %1x2000 row vector init
sbindices = zeros(1,2000);
while (alpha <= samples)
    if  s(alpha) > 0 
        sbin(beta) = 1;
        sbindices(beta) = alpha;
    elseif s(alpha) < 0    
        sbin(beta) = 0;
        sbindices(beta) = alpha;
    end
    alpha = alpha + 16;                 %sample increment
    beta = beta + 1;                    %index increment
end
fileID = fopen('xdvora2t.txt','r');     %open original
binput = fscanf(fileID,'%d');           %binary input
binput = binput';                       %columns to rows
eXORcism = xor(binput,sbin);            %comparison
alpha = 1;                              %indices
beta = 0;                               %cnt
while ( alpha <= 2000 )
    if eXORcism(alpha) == 0
        beta = beta + 1;
    end
    alpha = alpha + 1;
end
if ( beta < 2000 )
    fprintf("Symboly se nerovnaji. %d\n",beta);
elseif ( beta == 2000)
    fprintf("Symboly se rovnaji.\n");
end
fclose(fileID);
t = (0:length(s)-1) / Fs;
plot2= plot(t,s);
xlim([0 0.02]);
hold on;
plot2(1).LineWidth = 2;
xlabel('t');
ylabel('s[n],symbols');
tsbindices = (sbindices / Fs ); % x-axis from samples to ms
stem(tsbindices,sbin,'LineStyle','-.',...
     'Color','red',...   
     'LineWidth',2,...
     'Marker','d',...
     'MarkerFaceColor','white',...
     'MarkerEdgeColor','red');
legend('s[n]','symbols');
print('2bin','-dpng');
close 1;


%3
B = [0.0192   -0.0185   -0.0185    0.0192];
A = [1.0000   -2.8870    2.7997   -0.9113];
zplane(B,A);
[z,p,k] = tf2zp(B,A);
title('Zeros and Poles');
text(real(z)+.1,imag(z),'Zero');
text(real(p)+.1,imag(p),'Pole');
print('3ZeroPole','-dpng');
close 1;
if abs(roots(A)) < 1
	fprintf('Filtr je stabilní.\n');
else
	fprintf('Filtr není stabilní.\n');
end

%4
FRV = abs(freqz(B, A));
plot((0 : 511) / 512 * Fs / 2, FRV, 'LineWidth', 2, 'Color', 'black');
title('LowPass Filter')
xlabel('frequency[Hz]');
ylabel('H(f)');
fprintf('LowPass Filter, Mezni frekvence: 500Hz\n');
print('4Filter','-dpng');
close 1;
H = tf(B,A,0.1,'variable','z^-1');
cutoff = bandwidth(H);

%5
ss = filter(B,A,s);
sco = xcorr(s,ss);
fprintf('Nejoptimalnejsi posunuti je zpozdeni o 31986, nebo predbehnuti o 14 vzorku. \n');

%6
ssshifted = circshift(ss,31986);

alpha = 8;                          %sample
beta = 1;                           %index
sshiftbin = zeros(1,2000);          %1x2000 row vector init
sshiftbindices = zeros(1,2000);
while (alpha <= samples)
    if  ssshifted(alpha) > 0        %getting ssshifted binary symbols
        sshiftbin(beta) = 1;
        sshiftbindices(beta) = alpha;
    elseif ssshifted(alpha) < 0    
        sshiftbin(beta) = 0;
        sshiftbindices(beta) = alpha;
    end
    alpha = alpha + 16;             %sample increment
    beta = beta + 1;                %index increment
end

plot6 =plot(t,s);
xlim([0 0.02]);
hold on;

xlabel('t');
ylabel('s[n],ss[n],ssshifted[n],symbols');
plot6ss = plot(t,ss);
plot6ss(1).Color = 'black';
hold on;
tsshiftbindices = ( sshiftbindices / Fs );
plot6 = plot(t,ssshifted);
plot6(1).LineWidth = 2;
plot6(1).Color = 'black';
stem(tsshiftbindices,sshiftbin,'LineStyle','-.',...
     'Color','red',...   
     'LineWidth',2,...
     'Marker','d',...
     'MarkerFaceColor','white',...
     'MarkerEdgeColor','red');
legend('s[n]','ss[n]','ssshifted[n]','symbols');
print('6bin','-dpng');
close 1;

%7
alpha = 1; %cnt
false_samples = 0;

while (alpha <= 2000)
    tmp = xor(sbin(alpha),sshiftbin(alpha));
    if  tmp == 1 %chyba
        false_samples = false_samples + 1; 
    end
    alpha = alpha + 1;
end  
    
fprintf('Pocet chyb pri porovnani s a ssshifted: %d\n',false_samples);
error_rate = (false_samples/bin) * 100;
fprintf('Chybovost: %0.1f procent.\n',error_rate);

%8
s_fft = abs(fft(s));
ss_fft = abs(fft(ss));
s_fft = s_fft(1 : Fs / 2);
ss_fft = ss_fft(1 : Fs / 2);
plot(s_fft,'Color','black');
set(gcf, 'Position',  [100, 100, 500, 200]); % figure resize
hold on;
plot(ss_fft,'Color','red');
hold on;
legend('s[n]','ss[n]');
xlabel('frequency[Hz]');
%ylabel('spectrums');
print('8fft','-dpng');
close 1;

%9
hbins = linspace(-0.8,0.8,70);
hpdf = histogram(s,hbins,'Normalization','pdf','DisplayName','hist');
title('Funkce hustoty rozdělení pravděpodobnosti');
legend('PDF');
hpdf.LineWidth = 2; 
hpdf.FaceColor = 'black';
hpdf.EdgeColor = 'black';
hsum = sum(hpdf.Values);
hsum = hsum * 0.014285714; %value is width of one bin
fprintf('Kontrolni soucet vsech obsahu je: %0.f\n', hsum);   
print('9pdfhist','-dpng');
close 1;

%10
Rauto = xcorr(s);
R = Rauto/samples;
koef = (-50:50);
R = R(samples+koef);
plot(koef,R,'Linewidth',2,'Color','black');
title('Korelační koeficienty');
xlabel('koeficients');
print('10koeficients','-dpng');
close 1;

%11
fprintf('Koeficient R[0]:  %0.4f .\n',R(51));
fprintf('Koeficient R[1]:  %0.4f .\n',R(52));
fprintf('Koeficient R[16]: %0.4f .\n',R(67));

%12
N = length(s); 
L = 50; 
h = zeros(L,L);
xaxis = linspace(min(s),max(s),50);
xrows = xaxis(:);
repx = repmat(xrows,1,N);

srows = s(:)'; 
reprows = repmat(srows,L,1);
[dummy,index1] = min(abs(reprows - repx)); 
index2 = index1(2:N);

omega = 1;
while (omega <= N-1 )
  d1 = index1(omega);
  d2 = index2(omega); 
  h(d1,d2) = h(d1,d2) + 1; 
  omega = omega + 1;
end

surf = (xaxis(2) - xaxis(1)) ^ 2;
p = h / N / surf;
fall = waterfall(xaxis,xaxis,p);
set(gcf, 'Position',  [100, 100, 900, 720]); % figure resize

material dull;
shading interp;
colorbar;
fall.LineWidth = 2;
fall.FaceLighting = 'gouraud';
xlabel('x2');
ylabel('x1');
l1 = light;
l1.Position = [160 400 80];
l1.Style = 'local';
l1.Color = [0 0.8 0.8];
print('12joint','-dpng');
close 1;

%13
thirteen = sum(sum(p) * surf);
fprintf('Jedna se o spravnou funkci rozdeleni p: %0.1f \n',thirteen);

%14
xaxis = xaxis(:); 
X1 = repmat(xaxis,1,L);
xaxis = xaxis'; 
X2 = repmat(xaxis,L,1);
fourteen = sum(sum (X1 .* X2 .* p)) * surf;
fprintf('Koeficient R[1]:  %0.4f .\n',fourteen);
differ = abs(R(52) - fourteen);
fprintf('Rozdil oproti puvodni hodnote: %0.4f\n',differ);