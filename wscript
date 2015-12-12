#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'kodo-fulcrum'
VERSION = '1.0.0'

import waflib.extras.wurf_options


def options(opt):

    opt.load('wurf_common_tools')


def resolve(ctx):

    import waflib.extras.wurf_dependency_resolve as resolve

    ctx.load('wurf_common_tools')

    ctx.add_dependency(resolve.ResolveVersion(
        name='waf-tools',
        git_repository='github.com/steinwurf/waf-tools.git',
        major=3))

    ctx.add_dependency(resolve.ResolveVersion(
        name='kodo-rlnc',
        git_repository='github.com/steinwurf/kodo-rlnc.git',
        major=1))

    # Internal dependencies
    if ctx.is_toplevel():

        ctx.add_dependency(resolve.ResolveVersion(
            name='gauge',
            git_repository='github.com/steinwurf/gauge.git',
            major=10))

        ctx.add_dependency(resolve.ResolveVersion(
            name='stub',
            git_repository='github.com/steinwurf/stub.git',
            major=4))

        ctx.add_dependency(resolve.ResolveVersion(
            name='gtest',
            git_repository='github.com/steinwurf/gtest.git',
            major=3))


def configure(conf):

    conf.load("wurf_common_tools")


def build(bld):

    bld.load("wurf_common_tools")

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_KODO_FULCRUM_VERSION="{}"'.format(VERSION))

    # Export kodo-fulcrum includes
    bld(name='kodo_fulcrum_includes',
        includes='./src',
        export_includes='./src',
        use=['kodo_rlnc_includes'])

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')

        bld.recurse('examples/encode_decode_fulcrum')

        bld.recurse('benchmark/throughput')

