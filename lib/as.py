# save as convert_to_utf8_recursive.py
import os
from chardet import detect

SKIP_EXTENSIONS = {'exe', 'zip', 'jpg', 'png', 'gif', 'pdf', 'docx', 'xlsx', 'dll', 'bin', 'py'}

def convert_file(filepath):
    # 读取原始二进制内容
    with open(filepath, 'rb') as f:
        content = f.read()
        if not content:  # 跳过空文件
            return False

    # 检测文件编码
    result = detect(content)

    try:
        # 解码并重新编码为UTF-8
        decoded_content = content.decode('GBK', errors='replace')
        with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
            f.write(decoded_content)
        return True
    except Exception as e:
        print(f"解码失败 [{filepath}]: {str(e)}")
        return False

def process_directory(root_dir='.'):
    success_count = 0
    fail_count = 0
    
    for root, dirs, files in os.walk(root_dir):
        for filename in files:
            filepath = os.path.join(root, filename)
            
            # 跳过二进制文件
            ext = filename.split('.')[-1].lower()
            if ext in SKIP_EXTENSIONS:
                continue
                
            # 跳过隐藏文件（Unix-like系统）
            if filename.startswith('.'):
                continue
                
            print(f"正在处理: {filepath}", end='\r')
            if convert_file(filepath):
                success_count +=1
            else:
                fail_count +=1

    print(f"\n转换完成！成功: {success_count}, 失败: {fail_count}")

if __name__ == '__main__':
    process_directory()
