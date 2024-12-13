import platform
import os

VERSION = '0.1'

def generate_protobuf_define(src: str, dist: str, proto: list, expt: list, include_dir: str):
    """生成协议枚举类型、回调函数声明头文件和回调注册源文件"""
    
    assert src, 'proto input path error'
    assert dist, 'proto output path error'

    print(f'-- Protobuf input path: {os.getcwd()}\\{src}')
    print(f'-- Protobuf output path: {os.getcwd()}\\{dist}')

    if not os.path.exists(dist):
        os.makedirs(dist)
        print("-- Creating proto output folder")

    proto_data = []
    proto_name_set = []

    for val in proto:
        with open(os.path.join(src, val + '.proto'), 'r', encoding='utf-8') as file:
            package = {'list': []}

            line = file.readline()
            while line:
                line = line.strip()

                if line.startswith('package'):
                    package['package'] = line[8:-1].strip()

                if line.startswith('message'):
                    line = line[8:-1].strip()
                    if line.startswith('CS'):
                        if line in proto_name_set:
                            raise f"{line} redefined."

                        if line in expt:
                            package['list'].append({'proto': line, 'callback': 0})
                        else:
                            package['list'].append({'proto': line, 'callback': 1})
                        
                        proto_name_set.append(line)

                    if line.startswith("SC"):
                        if line in proto_name_set:
                            raise NameError(f"{line} redefined.")

                        package['list'].append({'proto': line, 'callback': 0})
                        proto_name_set.append(line)

                line = file.readline()

            proto_data.append(package)

            print(f"-- \t{file.name} have done")


    with open(os.path.join(dist, 'ProtoType.h'), 'w', encoding='utf-8') as file:
        file.write(f'''/**
 * Protocol ID define here by enum class
 * This file is generated by Python script. Do not edit!!!
 * Python version: v{platform.python_version()}
 * Script version: v{VERSION}
 */\n\n''')

        file.write('#pragma once\n\n')
        file.write('#include <cstdint>\n\n')

        file.write('namespace protocol {\n')
        file.write('\tenum class EProtoType : uint32_t {\n')

        package_index = 1
        proto_index = 1

        for package in proto_data:
            proto_index = 1
            file.write('\t\t// %s\n' % package['package'])

            for proto in package['list']:
                pkg_id = package_index * 1000 + proto_index
                file.write('\t\t%s = %d,\n' % (proto['proto'], pkg_id))

                proto_index += 1

            file.write('\n')
            package_index += 1

        file.write('\t\tPROTO_TYPE_MAX,\n')
        file.write('\t}; // EProtoType\n\n')

        file.write('\tconstexpr const char* ProtoTypeToString(const EProtoType value) {\n')
        file.write('\t\tswitch (value) {\n')

        for package in proto_data:
            file.write(f'\t\t\t// {package['package']}\n')

            for proto in package['list']:
                file.write(f'\t\t\tcase EProtoType::{proto['proto']}: return "{proto['proto']}";\n')

            file.write('\n')

        file.write('\t\t\tdefault: return "UNDEFINED";\n')
        file.write('\t\t} // switch\n')
        file.write('\t}\n\n')
    
        file.write('} // protocol')

    with open(os.path.join(dist, 'proto_def.h'), 'w', encoding='utf-8') as file:
        file.write('''/**
 * Protocol callback function define here.
 * This file is generated by Python script. Do not edit!!!
 * Python version: v%s
 * Script version: v%s
 */\n\n''' % (platform.python_version(), VERSION))

        file.write('#pragma once\n\n')
        file.write('#include <memory>\n')
        file.write('#include <asio/awaitable.hpp>\n\n')

        file.write('class IPackage;\n')
        file.write('class IAbstractPlayer;\n\n')

        file.write('using AConnectionPointer = std::shared_ptr<class UConnection>;\n')
        file.write('using asio::awaitable;\n\n')

        file.write('namespace protocol {\n\n')

        for package in proto_data:
            file.write(f'\t// {package['package']}\n')

            for proto in package['list']:
                if proto['callback'] == 1:
                    file.write(f'\tawaitable<void> {proto['proto']}(const std::shared_ptr<IAbstractPlayer> &plr, IPackage *pkg);\n')

            file.write('\n')

        file.write('} // protocol\n\n')

        file.write('#include "ProtoType.h"\n')
        file.write(f'#include "{include_dir}"\n\n')

        file.write('inline void LoadProtocol(UProtocolSystem *sys) {\n\n')
        file.write('\tusing namespace protocol;\n\n')

        for package in proto_data:
            file.write(f'\t// {package['package']}\n')

            for proto in package['list']:
                if proto['callback'] == 1:
                    file.write(f'\tREGISTER_PROTOCOL({proto['proto']})\n')

            file.write('\n')

        file.write('}')
