function symbols = PatternDemapper(metrics,lenMetrics)
% metrics принимает значение от 0 до 1

lenSimbols = floor(lenMetrics/4);
symbols = zeros(1,lenSimbols);

for i = 0:lenSimbols - 1
    error = metrics(i*4+1) + metrics(i*4+2) + 2 - metrics(i*4+3) - metrics(i*4+4);
    if(error > 2)
        % 1100 -> 1
        symbols(i+1) = 1;
    else
        % 0011 -> 0
        symbols(i+1) = 0;
    end
end
end

