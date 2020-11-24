Introduction
============

A tracer that logs stack allocation and stack access and ensure the pattern
matches what ``-fstack-clash-protection`` expects.

It can detect:

1. stack allocation greater than ``PAGE_SIZE``
2. stack allocation not being probed

The tool is based on the 0.7.1 version of the **awesome** library `QBDI
<https://github.com/QBDI/QBDI>`_.

Compilation
===========

Linux / macOS
-------------

.. code:: console

    $ # unix
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Usage
=====

Linux
-----

.. code:: console

    LD_PRELOAD=./libstack_clash_tracer.so ./mytarget

macOS
-----

.. code:: console

    sudo DYLD_INSERT_LIBRARIES=./libstack_clash_target.dylib ./mytarget

