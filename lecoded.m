clear variables; close all; clc;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   LE coded
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Example 1 from page 2855.
% TRANSMITTED SYMBOLS (S=2)

% AccessAddres Duration 256 µs
AccessAddres = [0 0 1 1 0 0 1 1 1 1 0 0 1 1 0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 1 1 0 0 1 1 0 0 0 0 1 1 1 1 0 0,...
0 0 1 1 0 0 1 1 1 1 0 0 0 0 1 1 0 0 1 1 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 0 0 1 1 1 1 0 0 0 0 1 1,...
0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 1 1 0 0 0 0 1 1 1 1 0 0 1 1 0 0 1 1 0 0 0 0 1 1 0 0 1 1 1 1 0 0,...
0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0,...
1 1 0 0 0 0 1 1 0 0 1 1 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0 1 1 0 0 1 1 0 0 1 1,...
1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0];

CodingIndicator = [0 0 1 1 1 1 0 0 0 0 1 1 1 1 0 0];

TERM1 = [0 0 1 1 0 0 1 1 1 1 0 0 1 1 0 0 0 0 1 1 0 0 1 1];

PDU = [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 1 0 1 0 0 1 1 0 0 0 0 0 0 0 0 1 1 1 0 1,... 
1 1 1 0 0 1 1 1 0 1 1 1 1 1 1 0 1 0 1 0 0 0 0 1 0 0 0 0 1 0 0 0 1 1 1 1 1 1 1,... 
1 0];

CRC = [0 0 0 1 1 1 0 0 1 0 0 0 0 1 1 1 1 1 1 1 1 0 0 0 0 1 1 1 1 1 0 0 0 0 1 1 0 1 1,... 
0 1 0 0 0 0 0 0 1];

TERM2 = [0 1 0 0 1 1];

%% Total Packet Duration 430 µs (without preamble)
streamTotalPacket = [AccessAddres,CodingIndicator,TERM1,PDU,CRC,TERM2];

% FEC block 1 coded using S=8
lenBlock1 = 296;
lenBlock2 = length(streamTotalPacket) - lenBlock1;

block1Demapped = patternDemapper(streamTotalPacket,lenBlock1);
block1 = decoderViterby(block1Demapped);

AA = block1(1:32);
CI = block1(33:34);

if(isequal(CI,[1 0]))
    %FEC block 2 coded using S=2
    block2 = decoderViterby(streamTotalPacket(lenBlock1 + 1: lenBlock1 + lenBlock2));
else
    %FEC block 2 coded using S=8
    block2Demapped = PatternDemapper(streamTotalPacket(lenBlock1 + 1: lenBlock1 + lenBlock2),lenBlock2);
    block2 = decoderViterby(block2Demapped);
end




