#!/bin/bash

LOGICS_DIR="../../../../files/logics"
if [ ! -d "$LOGICS_DIR" ]; then
  LOGICS_DIR="../../../../../../files/logics"
fi

for i in $(ls -1 "$LOGICS_DIR"/*.logic 2>/dev/null) ; do
  echo "Running $i"
  ./logic_run "$i" < /dev/null
done
