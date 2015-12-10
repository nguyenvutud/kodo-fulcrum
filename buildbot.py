#!/usr/bin/env python
# encoding: utf-8

import os
import sys
import json
import shutil
import subprocess
from datetime import datetime

project_name = 'kodo-fulcrum'


def run_command(args):
    print("Running: {}".format(args))
    sys.stdout.flush()
    subprocess.check_call(args)


def get_tool_options(properties):
    options = []
    if 'tool_options' in properties:
        # Make sure that the values are correctly comma separated
        for key, value in properties['tool_options'].items():
            if value is None:
                options += ['--{0}'.format(key)]
            else:
                options += ['--{0}={1}'.format(key, value)]

    return options


def configure(properties):
    command = [sys.executable, 'waf']

    if properties.get('build_distclean'):
        command += ['distclean']

    command += ['configure', '--git-protocol=git@']

    if 'waf_bundle_path' in properties:
        command += ['--bundle-path=' + properties['waf_bundle_path']]

    if 'dependency_project' in properties:
        command += ['--{0}-use-checkout={1}'.format(
            properties['dependency_project'],
            properties['dependency_checkout'])]

    command += ["--cxx_mkspec={}".format(properties['cxx_mkspec'])]
    command += get_tool_options(properties)

    run_command(command)


def build(properties):
    command = [sys.executable, 'waf', 'build', '-v']
    run_command(command)


def run_tests(properties):
    command = [sys.executable, 'waf', '-v', '--run_tests']
    run_cmd = '%s'

    if properties.get('valgrind_run'):
        run_cmd = 'valgrind --error-exitcode=1 %s --profile=embedded'
    elif 'test_type' in properties:
        run_cmd += ' --profile={0}'.format(properties['test_type'])

    if run_cmd:
        command += ["--run_cmd={}".format(run_cmd)]

    command += get_tool_options(properties)

    run_command(command)

    # Dry run the benchmarks after the unit tests
    command = [sys.executable, 'waf', '-v']
    command += ['--run_benchmarks', '--run_cmd=%s --dry_run']
    run_command(command)


def run_benchmarks(properties, dry_run=False):

    benchmark_results = os.path.join(os.getcwd(), 'benchmark_results')
    # Make sure that the previous results are deleted
    if os.path.isdir(benchmark_results):
        shutil.rmtree(benchmark_results)

    revision = 'none'
    if 'got_revision' in properties:
        revision = properties['got_revision']
    elif 'revision' in properties:
        revision = properties['revision']

    benchmarks = ['kodo_fulcrum_throughput']

    if properties.get('requested_benchmark'):
        benchmarks = [properties['requested_benchmark']]

    for benchmark in benchmarks:
        # Get a timestamp in the ISO 8601 format
        timestamp = datetime.now().strftime("%Y%m%dT%H%M%S")
        result_file = '{}-{}.json'.format(timestamp, revision[:8])
        result_folder = os.path.join(benchmark_results, benchmark)

        command = [sys.executable, 'waf', '-v']
        command += ['--run_benchmark={}'.format(benchmark)]
        command += ['--result_file={}'.format(result_file)]
        command += ['--result_folder={}'.format(result_folder)]

        run_cmd = '%s --use_console=0 --use_python=0 --use_csv=0 ' \
                  '--use_json=1 --json_file={}'.format(result_file)
        if properties.get('benchmark_arguments'):
            run_cmd += ' {}'.format(properties['benchmark_arguments'])

        command += ["--run_cmd={}".format(run_cmd)]

        run_command(command)

        # Add some metadata to the resulting json file
        filename = os.path.join(result_folder, result_file)
        with open(filename, 'r') as json_file:
            json_data = json.load(json_file)

        json_data.insert(0, {
            'benchmark_name': benchmark,
            'branch': properties['branch'],
            'buildername': properties['buildername'],
            'buildnumber': properties['buildnumber'],
            'commit': revision,
            'cxx_mkspec': properties['cxx_mkspec'],
            'platform': properties['platform'],
            'scheduler': properties['scheduler'],
            'slavename': properties['slavename'],
            'utc_date': datetime.utcnow().isoformat(),
        })

        with open(filename, 'w') as json_file:
            json_file.write(json.dumps(json_data))


def install(properties):
    command = [sys.executable, 'waf', '-v', 'install']

    if 'install_path' in properties:
        command += ['--install_path={0}'.format(properties['install_path'])]
    if properties.get('install_relative'):
        command += ['--install_relative']

    run_command(command)


def coverage_settings(options):
    options['required_line_coverage'] = 98.0


def main():
    argv = sys.argv

    if len(argv) != 3:
        print("Usage: {} <command> <properties>".format(argv[0]))
        sys.exit(0)

    cmd = argv[1]
    properties = json.loads(argv[2])

    if cmd == 'configure':
        configure(properties)
    elif cmd == 'build':
        build(properties)
    elif cmd == 'run_tests':
        run_tests(properties)
    elif cmd == 'install':
        install(properties)
    elif cmd == 'run_benchmarks':
        run_benchmarks(properties)
    else:
        print("Unknown command: {}".format(cmd))


if __name__ == '__main__':
    main()
