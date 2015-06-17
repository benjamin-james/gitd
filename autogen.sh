#!/bin/sh
autoreconf -i || exit 1
cat << EOF

./configure by default will install gitd.conf to sysconfdir,
usually ${prefix}/etc, so if you want it to install in /etc,
run ./configure with --sysconfdir=/etc/gitd or --sysconfdir=/etc

Also, ./configure by default will use ${prefix} as /usr/local.
So, gitd.conf would usually be placed in /usr/local/etc/gitd.conf,
and the binary would be installed to /usr/local/bin/gitd

          "      m        # 
  mmmm  mmm    mm#mm   mmm# 
 #" "#    #      #    #" "# 
 #   #    #      #    #   # 
 "#m"#  mm#mm    "mm  "#m## 
  m  #                      
   ""                       
EOF

