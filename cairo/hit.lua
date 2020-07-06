s = "34324.34,-343.34,343.0,120";
for w in string.gmatch(s, "[^,]+") do
   print(w)
end
