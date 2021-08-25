xor_key = [1, 3, 14, 3, 28, 95, 86, 0, 86, 94, 91, 80, 84, 14, 91, 87, 87, 91, 14, 82, 95, 87, 94, 83, 86, 87, 12, 83, 87, 93, 92, 0, 85, 93, 89, 0, 68, 92]
enc_flag = list(b'goodgoodgoodgoodgoodgoodgoodgoodgood!!')

flag = ''
for i in range(len(enc_flag)):
    flag += chr(ord(enc_flag[i])^xor_key[i])
print(flag) # flag{09d11443a4304a6881718c7023d226de}