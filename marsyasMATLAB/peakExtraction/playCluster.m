function [] = playCluster()

fileName = get(gcbo, 'UserData');

[a, fs]=wavread(fileName);

if(~isempty(a))
sound(a, fs);
end
