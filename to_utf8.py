import os

base = r"e:\YQ Project\SLV258-存贮式数字毫秒计项目\1.代码\STM32F1_ST7789_16Bit  Demo_V1"

for root, dirs, files in os.walk(base):
    dirs[:] = [d for d in dirs if d != 'OBJ']
    for fname in files:
        if not fname.lower().endswith(('.c', '.h', '.s', '.txt')):
            continue
        path = os.path.join(root, fname)
        with open(path, 'rb') as f:
            raw = f.read()
        try:
            raw.decode('utf-8')
            continue  # already UTF-8
        except UnicodeDecodeError:
            pass
        try:
            text = raw.decode('gbk')
        except Exception:
            print(f"SKIP (cannot decode): {path}")
            continue
        with open(path, 'wb') as f:
            f.write(text.encode('utf-8'))
        print(f"OK: {os.path.relpath(path, base)}")
