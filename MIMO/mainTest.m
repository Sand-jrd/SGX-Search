%%  -- TP MIMO -- %%

clear;
close all;
clc;

%% Variables

M       = 2; % Nombre d'Antennes
N       = 2; % Nombre d'échantillions
L       = 2; % Nombre de symboles
m       = 4; % Modulation QPSK <=> 4 symboles

% Canal à évanouissment
H       =  (randn(M,N) + 1i*randn(M,N))/sqrt(2);

sigmab  = 0.1;    % Energie du bruit
   
% Construction du modulateur
mod_psk = comm.PSKModulator(...
    'ModulationOrder', m, ... % BPSK
    'PhaseOffset'    , 0, ...
    'SymbolMapping'  , 'Gray',...
    'BitInput'       , true);

%% Construction du demodulateur
demod_psk = comm.PSKDemodulator(...
    'ModulationOrder', m, ...
    'PhaseOffset'    , 0, ...
    'SymbolMapping'  , 'Gray',...
    'BitOutput'      , true,...
    'DecisionMethod' , 'Log-likelihood ratio');

%% Creation des Symboles à tramettre

% Génération des symboles
s   = randi([0,1], N*L*(m/2), 1);      % Message binaire
x   = step(mod_psk, s);                % Modulation QPSK

cX   = reshape(x,N,L);

% Ajout du bruit
V   = (sigmab*randn(N,L) + 1i*sigmab*randn(N,L))/sqrt(2);
Y   = H*cX + V;

% scatterplot(Y(1,:));
A    =  [1,-1,0-1i,0+1i];


%% Decodage

% Création du dictionaire des mots de code. 
Cbin = dec2bin(0:2^(N*L*(m/2))-1)'  - '0';
% 
% X_est = "empty";
% for mc=1:2^length(s)
%     
%     C     = step(mod_psk, Cbin(:,mc));    % Modulation QPSK
%     C     = reshape(C,N,L);
%     
%     if( isa(X_est,'string') || norm(Y-H*C,'fro')<norm(Y-H*X_est,'fro') )
%         X_est = C;
%     end
%     
% end

Z = (H*(H*H)^-1)*Y;


[~,I] = min(abs(repmat(reshape(Z,N*L,1),[1,m])-A).^2,[],2);

x_est = A(I).';


%x_est   = reshape(X_est,N*L,1);
Lc      = step(demod_psk, x_est);           % Démodulation QPSK
s_est   = double(Lc(1:N*L*(m/2)) < 0);      % Décision




