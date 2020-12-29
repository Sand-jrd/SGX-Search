#!/bin/bash


while IFS= read -r line; do
  sleep 0.015
  start="$(date -u +%s.%N)"
  ./linux-sgx/linux/installer/bin/here/sgxsdk/SampleCode/SampleEnclave/app "${line::-1}" >> output
  printf "\n" >> output
  end="$(date -u +%s.%N)"
  echo  "$(bc<<<"$end - $start")" >> benchmaks
  printf "\n" >> benchmaks
done < testing.txt
