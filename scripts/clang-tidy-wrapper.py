#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
MSBuild の /t:ClangTidy から呼ばれるラッパースクリプト
clang-tidy.exe の代わりに呼び出され、-p 引数を受け取って
run-clang-tidy.py にフィルタリング付きで委譲する
"""

import sys
import os
import subprocess

def extract_compile_commands_dir(args):
    """ extract the compile_commands.json directory specified with -p from the arguments"""
    for i, arg in enumerate(args):
        if arg.startswith('-p='):
            return arg[3:]
        elif arg == '-p' and i + 1 < len(args):
            return args[i + 1]
    return None

def find_repo_root():
    # カレントディレクトリを取得
    current = os.getcwd()

     # 現在のディレクトリからルートまで遡る
    while current != os.path.dirname(current):  # ドライブルート到達まで
        # .git フォルダまたは Game.sln があればそこがルート
        if os.path.exists(os.path.join(current, '.git')):
            return current
        if os.path.exists(os.path.join(current, 'Game.sln')):
            return current

def generate_compile_commands_with_header_files(compile_commands_path, compile_commands_json):
    # pythonのデフォルトのエンコーディングを変更する
    env = os.environ.copy()
    env['PYTHONUTF8'] = '1'  # デフォルトの UTF-8 を有効にする

    cmds = [
            ['compdb','-p',compile_commands_path,'list']
    ]

    for cmd in cmds:
        try:
            proc = subprocess.run(cmd, env=env, check=True,stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            out = proc.stdout
            if out:
                os.makedirs(os.path.dirname(compile_commands_json),exist_ok=True)
                with open(compile_commands_json,'w') as f:
                    f.write(out)
                print(f"[clang-tidy-wrapper] compdb produced compile_commands.json ({len(out)} bytes)", file=sys.stderr)
                return True
        except FileNotFoundError:
            # compdb コマンドが見つからない
            continue
        except subprocess.CalledProcessError as e:
            continue
    return False

def main():
    # デバッグ出力(標準エラーに出力)
    print(f"[clang-tidy-wrapper] current working directory: {os.getcwd()}", file=sys.stderr)

    repo_root = find_repo_root()
    print(f"[clang-tidy-wrapper] repo root: {repo_root}", file=sys.stderr)
    script_dir = os.path.join(repo_root, 'scripts')
    run_tidy_script = os.path.join(script_dir, 'run-clang-tidy.py')
    
    # -p 引数を抽出
    compile_commands_dir = extract_compile_commands_dir(sys.argv[1:])
    
    if not compile_commands_dir:
        print("[ERROR] specify the compile_commands.json directory as an argument", file=sys.stderr)
        sys.exit(1)
    
    # パス解決
    compile_commands_path = os.path.join(repo_root, compile_commands_dir)
    compile_commands_json = os.path.join(compile_commands_path, 'compile_commands.json')
    
    # 絶対パスに正規化
    compile_commands_path = os.path.abspath(compile_commands_path)
    compile_commands_json = os.path.abspath(compile_commands_json)

    print(f"[clang-tidy-wrapper] compile_commands.json path: {compile_commands_json}", file=sys.stderr)

    # compile_commands.jsonの存在確認
    # if os.path.exists(compile_commands_json):
    #     # ヘッダをcompile_commands.jsonに書き込む
    #     succeeded = generate_compile_commands_with_header_files(compile_commands_path,compile_commands_json)
    #     if not succeeded:
    #         print("[ERROR] compdb not available or failed to generate compile_commands.json", file=sys.stderr)
    #         print("Install compdb (pip install compdb) in the environment or add it to the CI workflow.", file=sys.stderr)
    # else:
    #     print(f"[ERROR] compile_commands.json not found: {compile_commands_json}", file=sys.stderr)
    #     sys.exit(1)

    # run-clang-tidy.pyの存在確認
    if not os.path.exists(run_tidy_script):
        print(f"[ERROR] run-clang-tidy.py not found: {run_tidy_script}", file=sys.stderr)
        sys.exit(1)

   
    # 並列ジョブ数
    jobs = os.cpu_count() or 1
    
    # pythonのデフォルトのエンコーディングを変更する
    env = os.environ.copy()
    env['PYTHONUTF8'] = '1'  # デフォルトの UTF-8 を有効にする

    # run-clang-tidy.py コマンドを構築
    # -source-filter に正規表現を渡して、mtgb または Source フォルダ配下の .cpp ファイルのみを対象にする
    # Windows のパス区切り(\\))と POSIX の(/)の両方にマッチするように [\\/]
    source_filter_regex = r'.*(mtgb|Source)[\\/].*'
    header_filter_regex = r'.*(mtgb|Source)[\\/].*'
    extra_args = [
        '-extra-arg-before=-fms-extensions',
        '-extra-arg-before=-fms-compatibility',
        '-extra-arg-before=-fms-compatibility-version=19.0',
        '-extra-arg-before=-D_MSC_VER=1928',
        '-extra-arg-before=-fdelayed-template-parsing',
        '-extra-arg-before=-target',
        '-extra-arg-before=x86_64-pc-windows-msvc',
    ]

    cmd = [
        'python',
        run_tidy_script,
        '-p', compile_commands_path,
        '-j', str(jobs),
        '-source-filter', source_filter_regex,
        '-header-filter', header_filter_regex,
        '-export-fixes', './scripts/fixes',
    ] + extra_args
    
    print(f"[clang-tidy-wrapper] execute: {' '.join(cmd)}", file=sys.stderr)
    
    try:
        result = subprocess.run(cmd,cwd=repo_root,env=env,check=False)
        exit_code = result.returncode
        print(f"[clang-tidy-wrapper] finish code: {exit_code}", file=sys.stderr)
        sys.exit(exit_code)
    except Exception as e:
        print(f"[ERROR] run-clang-tidy.py error during execuition: {e}", file=sys.stderr)
        sys.exit(1)

# MEMO:このスクリプトが直接実行された場合のみ__name__が'__main__'となる
# 他のスクリプトからimportされた場合は実行されない。__name__がスクリプト名になるから
if __name__ == '__main__':
    main()