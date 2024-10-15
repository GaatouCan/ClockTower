import pandas
import os
import sys
import json


def main(argv):
    src_dir = 'xlsx'
    dist_dir = 'json'

    for arg in argv:
        if arg.startswith('--input'):
            src_dir = arg[8:]
        if arg.startswith('--output'):
            dist_dir = arg[9:]

    assert src_dir, '输入路径错误'
    assert dist_dir, '输出路径错误'

    if os.getcwd().endswith("src"):
        parent_dir = os.path.dirname(os.getcwd())
        os.chdir(parent_dir)

    if os.getcwd().endswith("dist"):
        parent_dir = os.path.dirname(os.getcwd())
        os.chdir(parent_dir)

    print('xlsx 输入路径: ' + os.getcwd() + '\\' + src_dir)
    print('json 输出路径: ' + os.getcwd() + '\\' + dist_dir)

    if not os.path.exists(dist_dir):
        os.makedirs(dist_dir)

    count = 0
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            if not file.endswith('.xlsx'):
                continue

            if file.startswith('~'):
                continue

            file_path = os.path.join(root, file)
            df = pandas.read_excel(file_path)
            print('\t已加载 %s' % file_path)

            df = df.drop([0])

            columns = []
            output_data = {}

            row_by_iloc = df.iloc[0]

            for col in df.columns:
                if 's' in row_by_iloc[col]:
                    columns.append(col)

            for idx, row in df.iterrows():
                if idx <= 1:
                    continue

                item = {}
                for f in columns:
                    item[f] = row[f]

                output_data[item["id"]] = item

            if not os.path.exists('%s/%s' % (dist_dir, root[5:])):
                os.makedirs('%s/%s' % (dist_dir, root[5:]))

            with open('%s/%s.json' % (dist_dir, file_path[5:-5]), 'w', encoding='utf-8') as file:
                file.write(json.dumps(output_data, indent=4, ensure_ascii=False))

            # df.to_csv('%s/%s.csv' % (dist_dir, file[:-5]), index=False)
            count += 1

    print('已完成%d个文件转换' % count)


if __name__ == "__main__":
    main(sys.argv)
