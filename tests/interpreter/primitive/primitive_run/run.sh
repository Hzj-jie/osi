#!/bin/bash

for i in $(ls -1 ../../../../files/primitives/) ; do
  ./primitive_run "../../../../files/primitives/"$i < /dev/null
done
