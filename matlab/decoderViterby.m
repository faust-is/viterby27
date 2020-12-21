function  decodedStream = decoderViterby(codeword)
% Декодер Витерби для BLE

% максимальная степень полинома
m = 3;
ConsLen = m + 1;
CodeGen = [17,13]; % octal 17 = [1 1 1 1], octal 13 = [1 0 1 1]
trellis = poly2trellis(ConsLen,CodeGen);

%decodedStream = vitdecMy(codeword,trellis,ConsLen,'trunc','hard');
decodedStream = vitdec(codeword,trellis,ConsLen,'trunc','hard');

end

