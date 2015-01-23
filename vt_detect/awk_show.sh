#!/bin/bash
awk '$1 == 2 {print;}' ./detect.out
