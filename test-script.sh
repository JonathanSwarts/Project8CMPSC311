# test script for CMPSC 311 Project 8

# Important - edit this file!
#    change "make -n" to the name of your program

while read in
do
  echo 'pr8 test:' $in
  $in
  echo
done <<EOF
  rm -f a b c d e
  make -n
  sleep 1
  touch d
  make -n
  sleep 1
  touch e
  make -n
  sleep 1
  touch c
  make -n
  sleep 1
  touch b
  make -n
  sleep 1
  touch a
  make -n
  sleep 1
  touch c
  make -n
  make -n b c
EOF

