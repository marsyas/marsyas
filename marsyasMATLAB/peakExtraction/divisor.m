function d = divisor(a, b)

c=max(a, b)/min(a, b)

for i=1:ceil(10*c)
   d(i) = mod(c*i, 1);  
   d(i) = min(d(i), 1-d(i));
end

[v l]=min(d);

d = min(a, b)/l;
