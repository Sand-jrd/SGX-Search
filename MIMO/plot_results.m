close all
figure(1)

sim_name = ["ML","ZF","MMSE"];
param    = ["22"];
for name = sim_name
    load("mat/"+name+"_"+param)
    semilogy(SNR,ber);
    hold on
end

xlim([0 10])
ylim([1e-2 1])
grid on
xlabel('$\frac{E_b}{N_0}$ en dB','Interpreter', 'latex', 'FontSize',14)
ylabel('TEB','Interpreter', 'latex', 'FontSize',14)
legend(sim_name)
