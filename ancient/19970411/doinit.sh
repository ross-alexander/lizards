#!/usr/local/bin/bash

PATH=$PATH:.

worldinit -x 32 -y 32 -w ON $1 $2
worldffro $1 $2
worldfill $1 $2
worldpop $1 $2 1 1
worldfix $1 $2


