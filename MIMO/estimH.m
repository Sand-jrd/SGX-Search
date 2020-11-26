function [H] = estimH(Y,S,sigb)
%ESTIMH Summary of this function goes here
%   Detailed explanation goes h
[N,~] = size(Y);

SG = sum( (1/sigb) * Y.' ,2) / ( N / sigb );
H  = ( (S^-1) * SG ).' ; 

end

