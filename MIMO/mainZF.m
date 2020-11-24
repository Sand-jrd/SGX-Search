clear
clc

%% Parametres
% -------------------------------------------------------------------------

M       = 2; % Nombre d'Antennes
N       = 2; % Nombre d'échantillions
L       = 1; % Nombre de symboles

simulation_name = 'ZF_'+string(N)+string(L);

% Canal à évanouissment
H       =  (randn(M,N) + 1i*randn(M,N))/sqrt(2);

m       = 4; % Modulation QPSK <=> 4 symboles
phi0    = 0; % Offset de phase our la BPSK

n_frame_max = 10000;% Nombre de bits max � simuler

SNR = 0:0.5:10;     % Points de EbN0 en dB � simuler

% Création du dictionaire des mots de code. 
Cbin = dec2bin(0:2^(N*L*(m/2))-1)'  - '0';


% Création du dictionaire des mots de code. 
Cbin = dec2bin(0:2^(N*L*(m/2))-1)'  - '0';
% -------------------------------------------------------------------------

%% Construction du modulateur
mod_psk = comm.PSKModulator(...
    'ModulationOrder', m, ... % QPSK
    'PhaseOffset'    , phi0, ...
    'SymbolMapping'  , 'Gray',...
    'BitInput'       , true);

%% Construction du demodulateur
demod_psk = comm.PSKDemodulator(...
    'ModulationOrder', m, ...
    'PhaseOffset'    , phi0, ...
    'SymbolMapping'  , 'Gray',...
    'BitOutput'      , true,...
    'DecisionMethod' , 'Log-likelihood ratio');


A    =  [mod_psk([0;0]),mod_psk([1;1]),mod_psk([1;0]),mod_psk([0;1])];

%% Initialisation des vecteurs de r�sultats
ber = zeros(1,length(SNR));

%% Simulation
for i_snr = 1:length(SNR)

    err_stat     = 0; % vecteur r�sultat de stat_erreur
    n_frame      = 0;
    
    while (n_frame < n_frame_max)
        n_frame = n_frame + 1;
        
        %% Emetteur
        
        % Génération des symboles
        s   = randi([0,1], N*L*(m/2), 1);      % Message binaire
        x   = step(mod_psk, s);                % Modulation QPSK

        X   = reshape(x,N,L);
        
                
        %% Canal
       
        Y     = H*X;
        
        % Ajout du bruit
        sigmab = 10^(-SNR(i_snr)/20);
        V   = (sigmab*randn(N,L) + 1i*sigmab*randn(N,L))/sqrt(2);
        Y   = Y + V;
        
        %% Recepteur
        
        Z = (H*(H*H)^-1)*Y;

        [~,I] = min(abs(repmat(reshape(Z,N*L,1),[1,m])-A).^2,[],2);
        
        x_est = A(I).';

        
        Lc      = step(demod_psk, x_est);           % Démodulation QPSK
        s_est   = double(Lc(1:N*L*(m/2)) < 0);      % Décision
                
        if(sum(x-x_est ~= 0))
            err_stat = err_stat+1;
        end
    end
    
    ber(i_snr) = err_stat/n_frame;
    
end

%%
figure(1)
semilogy(SNR,ber);
hold all
ylim([1e-2 1])
grid on
xlabel('$\frac{E_b}{N_0}$ en dB','Interpreter', 'latex', 'FontSize',14)
ylabel('TEB','Interpreter', 'latex', 'FontSize',14)

save("./mat/"+simulation_name,'SNR','ber')