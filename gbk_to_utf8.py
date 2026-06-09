import os, glob

base = r"e:\YQ Project\SLV258-存贮式数字毫秒计项目\1.代码\STM32F1_ST7789_16Bit  Demo_V1"
exts = ('.c', '.h', '.s', '.txt')

converted = []
failed = []

for root, dirs, files in os.walk(base):
    # skip OBJ
    dirs[:] = [d for d in dirs if d != 'OBJ']
    for fname in files:
        if not any(fname.lower().endswith(e) for e in exts):
            continue
        path = os.path.join(root, fname)
        with open(path, 'rb') as f:
            raw = f.read()
        # skip pure ASCII
        if all(b < 0x80 for b in raw):
            continue
        # try GBK first, then latin-1 fallback
        for enc in ('gbk', 'gb2312', 'latin-1'):
            try:
                text = raw.decode(enc)
                break
            except Exception:
                continue
        else:
            failed.append(path)
            continue
        # check if already valid UTF-8
        try:
            raw.decode('utf-8')
            continue  # already UTF-8, skip
        except Exception:
            pass
        out = text.encode('utf-8')
        with open(path, 'wb') as f:
            f.write(out)
        converted.append(os.path.relpath(path, base))

print(f"Converted {len(converted)} files:")
for p in converted:
    print(" ", p)
if failed:
    print(f"Failed: {failed}")
